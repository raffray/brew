#include "includes.h"
#include "mp3File.h"
#include "extern.h"
#include "byte.h"
#include "mfile.h"
#include "err.h"
#include "id3v2Tag.h"
#include "id3v2Tag_frame.h"
#include "id3v2Tag_frame_v3.h"
#include "id3v2Tag_frame_v4.h"
#include "frameID.h"
#include "id3v2_frames/COMM.h"
#include "id3v2_frames/POPM.h"
#include "id3v2_frames/Txyz.h"
#include "id3v2_frames/TCON.h"
#include "id3v2_frames/TXXX.h"
#include "id3v2_frames/APIC.h"
#include "buffer.h"
#include "sync_buf.h"


#define  NB_SELECT_FRAMES 16

/*
Fields from id3v1Tag
  char title  [31];  --> TIT2
  char artist [31];  --> TPE1
  char album  [31];  --> TALB
  char year   [ 5];  --> TYER
  char comment[31];  --> COMM
  UC track;          --> TRCK
  UC genre;          --> TCON

Fields from id3v1Tag_xtd
  char title  [61];  --> TIT2
  char artist [61];  --> TPE1
  char album  [61];  --> TALB
  char genre  [61];  --> TCON
  Speed
  Start time
  End   time
 */

/*
#define TIT2  0
#define TPE1  1
#define TALB  2
#define TYER  3
#define COMM  4
#define TRCK  5
#define TCON  6

#define TPE2  7
#define TCOM  8
#define TPE3  9
#define POPM 10
#define TPOS 11
#define TMOO 12
#define TDAT 13
#define TDRC 14
#define APIC 15

typedef id3_frame
{
// U4 frameID;
  char strID[5];
  char description[13];
} id3_frame_t;

id3_frame_t id3_frames[NB_SELECT_FRAME];

{
id3_frames[TIT2] = {"TIT2", "Title"};
id3_frames[TPE1] = {"TPE1", "Artist"};
id3_frames[TALB] = {"TALB", "Album"};
id3_frames[TYER] = {"TYER", "Year"};
id3_frames[COMM] = {"COMM", "Comment"};
id3_frames[TRCK] = {"TRCK", "Track#"};
id3_frames[TCON] = {"TCON", "Genre"};

id3_frames[TPE2] = {"TPE2", "Album Artist"}
id3_frames[TCOM] = {"TCOM", "Composer"};
id3_frames[TPE3] = {"TPE3", "Conductor"};
id3_frames[POPM] = {"POPM", "Pop. Meter"};
id3_frames[TPOS] = {"TPOS" , "Compilation"};
id3_frames[TMOO] = {"TMOO", "MOOD"};
id3_frames[TDAT] = {"TDAT", "Date"};
id3_frames[TDRC] = {"TDRC", "Rec. Date"};
id3_frames[APIC] = {"TDRC", "Picture"};
}
 */

UC selectFrames     [NB_SELECT_FRAMES][ 5] = { "TIT2", "TPE1", "TPE2", "TALB", "TRCK", "TYER", "TCON", "TCOM",
					       "TPE3", "POPM", "COMM", "TPOS", "TMOO", "TDAT", "TDRC", "APID"}; // APIC (picture) & TPOS (disc 1/2 for example)
UC selectFrames_desc[NB_SELECT_FRAMES][13] = { "Title"    , "Artist"    , "Album Artist", "Album"      , "Track#", "Year", "Genre"    , "Composer",
					       "Conductor", "Pop. Meter", "Comment"     , "Compilation", "Mood"  ,"Date" , "Rec. Date", "Picture"} ;

id3v2Tag_frameLink_t *get_id3v2Tag_frame                 (mp3File_t *file, U4 idx) { return  get_id3v2Tag_frameList(file)->ptrList[idx];              }
U4                    get_id3v2Tag_frame_frameID         (mp3File_t *file, U4 idx) { return  get_id3v2Tag_frame(file, idx)->frameID;                  }
U4                    get_id3v2Tag_frame_dataSize        (mp3File_t *file, U4 idx) { return  get_id3v2Tag_frame(file, idx)->buf->usedSize;                 }
U4                    get_id3v2Tag_frame_compression_flag(mp3File_t *file, U4 idx) { return (get_id3v2Tag_frame(file, idx)->info)->compression_flag; }
U4                    get_id3v2Tag_frame_decompressedSize(mp3File_t *file, U4 idx) { return (get_id3v2Tag_frame(file, idx)->info)->decompressedSize; }
U4                    get_id3v2Tag_frame_encryption_flag (mp3File_t *file, U4 idx) { return (get_id3v2Tag_frame(file, idx)->info)-> encryption_flag; }
U4                    get_id3v2Tag_frame_encryptionMethod(mp3File_t *file, U4 idx) { return (get_id3v2Tag_frame(file, idx)->info)->encryptionMethod; }
U4                    get_id3v2Tag_frame_grouping_flag   (mp3File_t *file, U4 idx) { return (get_id3v2Tag_frame(file, idx)->info)->   grouping_flag; }
U4                    get_id3v2Tag_frame_groupID         (mp3File_t *file, U4 idx) { return (get_id3v2Tag_frame(file, idx)->info)->         groupID; }
U4                    get_id3v2Tag_frame_dataLength_flag (mp3File_t *file, U4 idx) { return (get_id3v2Tag_frame(file, idx)->info)-> dataLength_flag; }
U4                    get_id3v2Tag_frame_dataLength      (mp3File_t *file, U4 idx) { return (get_id3v2Tag_frame(file, idx)->info)->      dataLength; }
UC                   *get_id3v2Tag_frame_data            (mp3File_t *file, U4 idx) { return  get_id3v2Tag_frame(file, idx)->buf->data;                    }
buffer_t             *get_id3v2Tag_frame_buffer          (mp3File_t *file, U4 idx) { return  get_id3v2Tag_frame(file, idx)->buf;                    }


UC process_id3v2Tag_frame_flags(mp3File_t *file, frame_info_t *info, buffer_t *buf, U4 end, UC version)
{ if(version==3) return process_id3v2Tag_frame_flags_v3(      info, buf, end);
  if(version==4) return process_id3v2Tag_frame_flags_v4(file, info, buf, end);
  return FAILURE; //unreached code
}

U4 get_id3v2Tag_frameSize(U4 val, UC ver)
{ if(ver == 3) return get_id3v2Tag_frameSize_v3(val);
  if(ver == 4) return get_id3v2Tag_frameSize_v4(val);
  return FAILURE; //unreached code
}

U4 set_id3v2Tag_frameSize(U4 val, UC ver)
{ if(ver == 3) return set_id3v2Tag_frameSize_v4(val);
  if(ver == 4) return set_id3v2Tag_frameSize_v4(val);
  return FAILURE; //unreached code
}

// example "TIT2" --> TIT2 == 0x 54 49 54 32
U4 str2id(UC str[4]) { return (str[0]<<24) + (str[1]<<16) + (str[2]<<8) + str[3]; }

UC isSelectFrame(U4 id)
{ U4 i;

  for(i=0; i< NB_SELECT_FRAMES; i++)
    if (id == str2id(selectFrames[i])) return true;
  return false;
}

U4 retrieve_id3v2Tag_frame(mp3File_t *file, buffer_t *tagBuffer)
{ id3v2Tag_frameList_t *list  = get_id3v2Tag_frameList(file);

  U4    frameID;
  U4  frameSize = 0;
  U4       size;
  U4    tagDataSize = tagBuffer->usedSize;
  UC *frameData;
  U4    frameDataSize;
  buffer_t *frameBuffer;
  UC tagVersion = get_id3v2Tag_version(file);
  frame_info_t *frameInfo;
  UC status;

  buffer_readU4(tagBuffer, &frameID);

  if (!is_tag_valid(frameID)) return 0;

  /* else */
  buffer_readU4(tagBuffer, &size);
  frameSize = get_id3v2Tag_frameSize(size, tagVersion);
  frameInfo = malloc(sizeof(frame_info_t));

  status =  process_id3v2Tag_frame_flags(file, frameInfo, tagBuffer, tagDataSize, tagVersion);

  if   ( (buffer_tell(tagBuffer) + frameSize > tagDataSize) || (status==FAILURE)
	 /*|| (frameInfo->bytesSkipped>frameSize)*/)   { free(frameInfo);   return 0; } // not a frame, we're done
  else //                                                                                 |         check         |
    {
      frameDataSize = frameSize - frameInfo->bytesSkipped; // <=== make sure frameSize > bytesSkipped
      frameData = malloc(frameDataSize);
      buffer_readStr(tagBuffer  , frameData, frameDataSize);
      frameBuffer = malloc(sizeof(buffer_t));
      buffer_open(frameBuffer   , frameData, frameDataSize);

      if (frameInfo->unsync_flag) // only in v4
	{
//printf("----- YES 1\n");
	resyncBuf2(frameBuffer);
//printf("----- YES 2\n");      exit(42);
	}
//printf("----- NO\n");      exit(42);

      push_id3v2Tag_frameLink(list, frameID, frameBuffer, frameInfo);
    }

  return frameSize;
}


UC print_id3v2Tag_frame(mp3File_t *file, U4 i)
{ U4 frameID = get_id3v2Tag_frame_frameID  (file, i);

  /*
    if  encryption... skip processing
    if compression... skip processing. Until I have a sample mp3 with a compressed frame to test
   */

  if (get_id3v2Tag_frame_dataSize  (file, i) == 0)   return FAILURE;
  /*
  if (get_id3v2Tag_frame_encryption_flag (file, i))      { fprintf(ostream, "Compressed frame -- Display to be implemented --\n");   return FAILURE; }
  if (get_id3v2Tag_frame_compression_flag(file, i))      { fprintf(ostream, "Encrypted frame -- Display to be implemented --\n");   return FAILURE; }
  */
  if      ( is_TCON_tag(frameID) )  print_TCON(file, i);
  else if ( is_TXXX_tag(frameID) )  print_TXXX(file, i);
  else if ( is_Txyz_tag(frameID) )  print_Txyz(file, i);

  else if ( is_COMM_tag(frameID) )  print_COMM(file, i);
  else if ( is_POPM_tag(frameID) )  print_POPM(file, i);
  else if ( is_APIC_tag(frameID) )  print_APIC(file, i);

  else    fprintf(ostream, " -- Display to be implemented --");

  fprintf(ostream, "\n");

  return SUCCESS;
}

U4 dump_id3v2Tag_frame(mp3File_t *file, buffer_t *tagBuffer, U4 i)
{
  id3v2Tag_frameLink_t *frame = get_id3v2Tag_frame        (file, i);

    if ((!frame->info->compression_flag) && (!frame->info->encryption_flag))
      {
	if (isSelectFrame(frame->frameID))
	  {
	    //Addendum, if COMM frame, only dump if the "Short content description" is empty
	    if(is_COMM_tag(frame->frameID) && !has_empty_shortDesc_COMM(file,i))
	      {} // skip it
	    else
	      { // The frame-header
		buffer_writeU4(tagBuffer, frame->frameID);
		buffer_writeU4(tagBuffer, set_id3v2Tag_frameSize(frame->buf->usedSize, 4));
		buffer_writeU2(tagBuffer, 0); // flags

		// The frame
		buffer_writeStr(tagBuffer, frame->buf->data, frame->buf->usedSize); // printU4str(frame->frameID, SP); printf("%d\n", frame->dataSize);
	      }
	  }

	else //if(frame->frameID == str2id("APIC");
	  {
	    // extractPicture
	  }
      }

  return frame->buf->usedSize;
}

/*
VD dumpFrame_v3(mp3File_t *inFile, mp3File_t *buFile, U4 i)
{
  U4 frameID  = get_id3v2Tag_frame_frameID (inFile, i); // <== ?? better to use     id3v2Tag_frameLink_t *frame = get_id3v2Tag_frame(inFile, i)
  U4 dataSize = get_id3v2Tag_frame_dataSize(inFile, i); //             and then     get_id3v2Tag_frame_dataSize(frame);
  U2 newFlags;                                          // more efficient but does it matter?
  UC compression_flag = get_id3v2Tag_frame_compression_flag(inFile, i);
  UC  encryption_flag =  get_id3v2Tag_frame_encryption_flag(inFile, i);
  UC    grouping_flag =    get_id3v2Tag_frame_grouping_flag(inFile, i);

  char *data  = get_id3v2Tag_frame_data    (inFile, i);

  newFlags    = (compression_flag * 0x0080) +
                (encryption_flag  * 0x0040) +
                (grouping_flag    * 0x0020);

  mseek(buFile, 0, SEEK_END);
  mwriteU4(buFile, frameID);
  mwriteU4(buFile, dataSize);

  mwriteU2(buFile, newFlags);
  if (compression_flag)   mwriteU4(buFile, get_id3v2Tag_frame_decompressedSize(inFile, i));
  if ( encryption_flag)   mwriteUC(buFile, get_id3v2Tag_frame_encryptionMethod(inFile, i));
  if (   grouping_flag)   mwriteUC(buFile, get_id3v2Tag_frame_groupID         (inFile, i));

  mwriteStr(buFile, data, dataSize);
}

VD dumpFrame_v4(mp3File_t *inFile, mp3File_t *buFile, U4 i)
{
  U4 frameID  = get_id3v2Tag_frame_frameID (inFile, i); // <== ?? better to use     id3v2Tag_frameLink_t *frame = get_id3v2Tag_frame(inFile, i)
  U4 dataSize = get_id3v2Tag_frame_dataSize(inFile, i); //             and then     get_id3v2Tag_frame_dataSize(frame);
  U2 newFlags;                                          // more efficient but does it matter?
  UC    grouping_flag =    get_id3v2Tag_frame_grouping_flag(inFile, i);
  UC compression_flag = get_id3v2Tag_frame_compression_flag(inFile, i);
  UC  encryption_flag =  get_id3v2Tag_frame_encryption_flag(inFile, i);
  UC  dataLength_flag =  get_id3v2Tag_frame_dataLength_flag(inFile, i);

  char *data  = get_id3v2Tag_frame_data    (inFile, i);

  newFlags    = (grouping_flag    * 0x0040) +
                (compression_flag * 0x0008) +
                (encryption_flag  * 0x0004) +
                (dataLength_flag  * 0x0001);

  mseek(buFile, 0, SEEK_END);
  mwriteU4(buFile, frameID);
  mwriteU4(buFile, dataSize);

  mwriteU2(buFile, newFlags);
  if (   grouping_flag)   mwriteUC(buFile, get_id3v2Tag_frame_groupID         (inFile, i));
  if (compression_flag)   mwriteU4(buFile, get_id3v2Tag_frame_decompressedSize(inFile, i));
  if ( encryption_flag)   mwriteUC(buFile, get_id3v2Tag_frame_encryptionMethod(inFile, i));
  if ( dataLength_flag)   mwriteU4(buFile, get_id3v2Tag_frame_dataLength      (inFile, i));

  mwriteStr(buFile, data, dataSize);
}
*/
