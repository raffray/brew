#include "includes.h"
#include "vbrFrame.h"
#include "extern.h"
#include "mp3Frame.h"
#include "mp3Stream.h"
#include "mp3File.h"
#include "byte.h"
#include "mfile.h"
#include "err.h"
#include "brew_types.h"
#include "prints.h"

UC xingOffset[4][2] = { {32, 17},  // Stereo       - V1/ V2&3
			{32, 17},  // Joint Stereo - V1/ V2&3
			{32, 17},  // Dual Channel - V1/ V2&3
			{17, 9} }; // Mono         - V1/ V2&3


void set_vbrFrame_flag           (mp3File_t *file, UC   flag) {        file->vbrFrame_flag    = flag;   } // <== set_stream_vbrFrame_flag?
UC   get_vbrFrame_flag           (mp3File_t *file)            { return file->vbrFrame_flag;             }
void set_vbrFrame_offset         (mp3File_t *file, U4   offset) {      file->vbrFrame.offset    = offset;   }
U4   get_vbrFrame_offset         (mp3File_t *file)            { return file->vbrFrame.offset;             } // <== get_vbrFrame_start_offset
void set_vbrFrame_idOffset       (mp3File_t *file, U4 idOffset) {      file->vbrFrame.idOffset  = idOffset; }
U4   get_vbrFrame_idOffset       (mp3File_t *file)            { return file->vbrFrame.idOffset;           }
void set_vbrFrame_type           (mp3File_t *file, U4  type) { file->vbrFrame.type = type;              }
U4   get_vbrFrame_type           (mp3File_t *file)           { return file->vbrFrame.type;              }
void set_vbrFrame_xingFlags      (mp3File_t *file, U4 flags) {        file->vbrFrame.xingFlags = flags; }
U4   get_vbrFrame_xingFlags      (mp3File_t *file)           { return file->vbrFrame.xingFlags;         }
void set_vbrFrame_frameCount_flag(mp3File_t *file, UC flag)    { file->vbrFrame.frameCount_flag = flag; }
UC   get_vbrFrame_frameCount_flag(mp3File_t *file)      { return file->vbrFrame.frameCount_flag;        }
void set_vbrFrame_frameCount     (mp3File_t *file, U4 count) {        file->vbrFrame.frameCount = count;       }
U4   get_vbrFrame_frameCount     (mp3File_t *file)           { return file->vbrFrame.frameCount;        }
void set_vbrFrame_byteCount_flag(mp3File_t *file, UC flag)     { file->vbrFrame.byteCount_flag = flag; }
UC   get_vbrFrame_byteCount_flag(mp3File_t *file)      { return file->vbrFrame.byteCount_flag;        }
void set_vbrFrame_byteCount     (mp3File_t *file, U4 count)  {        file->vbrFrame.byteCount = count;        }
U4   get_vbrFrame_byteCount     (mp3File_t *file)            { return file->vbrFrame.byteCount;         }
void set_vbrFrame_frameSize     (mp3File_t *file, U4  size)  {        file->vbrFrame.frameSize =  size;        }
U4   get_vbrFrame_frameSize     (mp3File_t *file)            { return file->vbrFrame.frameSize;         }
void set_vbrFrame_header        (mp3File_t *file, U4 header) {        file->vbrFrame.header =  header;        }
U4   get_vbrFrame_header        (mp3File_t *file)            { return file->vbrFrame.header;         }
void set_vbrFrame_length        (mp3File_t *file, float length) { file->vbrFrame.length =  length;        }
float get_vbrFrame_length       (mp3File_t *file)           { return file->vbrFrame.length;         }

void set_vbrFrame_version       (mp3File_t *file, U2   ver) {        file->vbrFrame.version = ver;   } // VBRI ONLY
U2   get_vbrFrame_version       (mp3File_t *file)           { return file->vbrFrame.version;         } // VBRI ONLY
void set_vbrFrame_delay         (mp3File_t *file, U2 delay) {        file->vbrFrame.delay   = delay; } // VBRI ONLY
U2   get_vbrFrame_delay         (mp3File_t *file)           { return file->vbrFrame.delay;           } // VBRI ONLY
void set_vbrFrame_tocFlag       (mp3File_t *file, UC  flag) {        file->vbrFrame.tocFlag =  flag; } // --> unnecessary ?
UC   get_vbrFrame_tocFlag       (mp3File_t *file)           { return file->vbrFrame.tocFlag;         } // --> unnecessary ?
void set_vbrFrame_qInd          (mp3File_t *file, U2   ind) {        file->vbrFrame.qInd    =   ind; }
U2   get_vbrFrame_qInd          (mp3File_t *file)           { return file->vbrFrame.qInd;            }


void setVbrHeader(mp3File_t *file)
{ U4 vbrID;// = 0; //init unnecessary. Makes valgrind happy
  UC xing_offset;
  UC vbri_offset;
  UC idOffset;
  U4 start_offset = mtell(file);
  U4 header;
  mp3Header_t hdr;
  UC isXing = false;
  UC isVbri = false;

  set_firstFrame_offset(file, start_offset);
  getMp3Header(file, &hdr, &header);
  set_vbrFrame_frameSize(file, hdr.frameSize);
  set_vbrFrame_length   (file, hdr.length);

  xing_offset = xingOffset[hdr.channelMode][hdr.versionId-1];
  vbri_offset = 32;

  // TRY#1 Xing or Info
  idOffset  = xing_offset;   mseek(file, 4 + idOffset, SEEK_CUR);
  mreadU4(file, &vbrID);

  // it will be a VBR frame if it doesn't actually overlap the next frame
  //            hdr              tag   flags
  // condition: 4   + idOffset + 4   + 4     > frameSize
  // we could have an empty frame with flags == 0 <-- useless but valid
  if( ((vbrID==XING) || (vbrID==INFO)) && (hdr.frameSize >= 4+idOffset+4+4) )   isXing = true;
  else
    { // TRY#2 VBRI
      mseek(file, -4 - idOffset - 4, SEEK_CUR);
      idOffset  = vbri_offset;   mseek(file, 4 + idOffset, SEEK_CUR);
      mreadU4(file, &vbrID);

      if( (vbrID==VBRI               ) && (hdr.frameSize >= 4+idOffset+4+4) )   isVbri = true;
      else   { set_vbrFrame_flag(file,  false);	  set_stream_start_offset   (file, start_offset); }
    }

// if(isXing==true)   printf("=== is     xing ===>\n");   else   printf("=== is not xing ===>\n");
// if(isVbri==true)   printf("=== is     vbri ===>\n");   else   printf("=== is not vbri ===>\n");

  if((isXing==true) || (isVbri==true))
    { set_vbrFrame_header(file, header);
      set_vbrFrame_type(file, vbrID);   set_vbrFrame_idOffset(file, idOffset);
      set_vbrFrame_flag(file,  true);   set_vbrFrame_offset(file, start_offset);
      set_stream_start_offset(file, start_offset + hdr.frameSize); }
  if(isXing==true) processXingHeader(file);
  if(isVbri==true) processVbriHeader(file);

  mseek(file, start_offset, SEEK_SET); // back to start of header
}
/* -----------------------------------------------------------
  * A Vbr header may be present in the ancillary
  * data field of the first frame of an mp3 bitstream
  * The Vbr header (optionally) contains
  *      frames      total number of audio frames in the bitstream
  *      bytes       total number of bytes in the bitstream
  *      toc         table of contents

  * toc (table of contents) gives seek points
  * for random access
  * the ith entry determines the seek point for
  * i-percent duration
  * seek point in bytes = (toc[i]/256.0) * total_bitstream_bytes
  * e.g. half duration seek point = (toc[50]/256.0) * total_bitstream_bytes
  */

/*
To generate a new TOC
 -> compute the total track length.
 -> backtrack to begining of stream and find frame# for which accumulated time is 1% of total track time
 -> searched value will be the (accumulated_data_time/total_data_time)*255/100
 -> keep going for each successive i%

*/

void processXingHeader(mp3File_t *file)
{
  UC     idOffset  = get_vbrFrame_idOffset(file);
  U4 start_offset  = get_vbrFrame_offset  (file);
  U4        flags;
  U4   frameCount;
  U4    byteCount;
  U4         qInd;

  // Do we check that the frame can hold the values it says it has?
  // Do we do it here?
  // Maybe have functions for reporting (normal or SCAN
  // or we declare the frame invalid  --  set_vbrFrame_flag(file,  false); <== I like that
  // or consider the field invalid and we need to change the flag value

  set_vbrFrame_frameCount_flag(file, false);
  set_vbrFrame_byteCount_flag (file, false);
  set_vbrFrame_tocFlag   (file,      false);
  // qInd_flag

  mseek(file, start_offset, SEEK_SET);
  mseek(file, 4+idOffset+4, SEEK_CUR);
  mreadU4(file, &flags);   set_vbrFrame_xingFlags(file, flags);
  if (flags & 0x01) { mreadU4(file, &frameCount);   set_vbrFrame_frameCount_flag(file, true);   set_vbrFrame_frameCount(file, frameCount); }
  if (flags & 0x02) { mreadU4(file, & byteCount);   set_vbrFrame_byteCount_flag (file, true);   set_vbrFrame_byteCount (file,  byteCount); }
  if (flags & 0x04) { mseek(file, 100, SEEK_CUR);   set_vbrFrame_tocFlag   (file,      true); /* check that we have enough room for 100 values ?? */}
  if (flags & 0x08) { mreadU4(file, &qInd);         set_vbrFrame_qInd      (file,      qInd); }

  mseek(file, start_offset, SEEK_SET); // back to start of header
}

void processVbriHeader(mp3File_t *file)
{
  UC     idOffset  = get_vbrFrame_idOffset(file);
  U4 start_offset  = get_vbrFrame_offset  (file);
  U4   frameCount;
  U4    byteCount;
  U2         qInd;
  U2      version;
  U2        delay;

  mseek(file, start_offset, SEEK_SET);
  mseek(file, 4+idOffset+4, SEEK_CUR);
  mreadU2(file, &version);      set_vbrFrame_version   (file,    version);
  mreadU2(file,   &delay);      set_vbrFrame_delay     (file,      delay);
  mreadU2(file,    &qInd);      set_vbrFrame_qInd      (file,       qInd);
  mreadU4(file, & byteCount);   set_vbrFrame_byteCount (file,  byteCount);
  mreadU4(file, &frameCount);   set_vbrFrame_frameCount(file, frameCount);

  mseek(file, start_offset, SEEK_SET); // back to start of header
}

void printVbrHeader(mp3File_t *file/*, mp3Header_t *hdr*/)
{
  U4 vbrID =  get_vbrFrame_type(file);

  fprintf(ostream, "\n*****  VBR Frame present ***********");
  if((vbrID==XING) || (vbrID==INFO)) printXingHeader(file);
  else
  if( vbrID==VBRI                  ) printVbriHeader(file);
  fprintf(ostream, "************************************\n");
}

void printXingHeader(mp3File_t *file)
{
  U4 flags      = get_vbrFrame_xingFlags(file);
  U4 frameCount = get_vbrFrame_frameCount(file);
  U4  byteCount = get_vbrFrame_byteCount (file);
  U4       qInd = get_vbrFrame_qInd      (file);

  fprintf(ostream, "\n*****  Mp3 Info tag   ID = ");   printU4str(get_vbrFrame_type(file), SP);   fprintf(ostream, "****\n");
  if (flags & 0x01)
    { fprintf(ostream, "Frame Count   :   ");   print_int_commas(frameCount);
      fprintf(ostream, "         ( "); printU4(frameCount, SP);   fprintf(ostream, ")\n"); }
  if (flags & 0x02)
    { fprintf(ostream, "Audio Data    :   ");   print_byteCount("", byteCount, SP);
      fprintf(ostream, "   ( ");       printU4(byteCount, SP);    fprintf(ostream, ")\n"); }
  if (flags & 0x04) { fprintf(ostream, "TOC table     :  present\n");   mseek(file, 100, SEEK_CUR); }
  if (flags & 0x08) { fprintf(ostream, "Quality       :  ");   fprintf(ostream, "%d\n", qInd); }
}

void printVbriHeader(mp3File_t *file)
{
  U2 version    = get_vbrFrame_version   (file);
  U2 delay      = get_vbrFrame_delay     (file);
  U2 qInd       = get_vbrFrame_qInd      (file);
  U4 byteCount  = get_vbrFrame_byteCount (file);
  U4 frameCount = get_vbrFrame_frameCount(file);

  fprintf(ostream, "\n*****  Mp3 Info tag   ID = VBRI ****\nVersion           : %d\n", version);
  fprintf(ostream, "Delay         : %f\n", (float) delay);
  fprintf(ostream, "Quality       : %d\n", qInd);
  fprintf(ostream, "Frame Count   :   ");   print_int_commas(frameCount);
  fprintf(ostream, "         ( "); printU4(frameCount, SP);   fprintf(ostream, ")\n");
  fprintf(ostream, "Audio Data    :   ");   print_byteCount("", byteCount, SP);
  fprintf(ostream, "   ( ");       printU4(byteCount, SP);    fprintf(ostream, ")\n");
}

/*
operation 1: get file offset to framecount


 */

UC testSize(U4 byteCount, U4 frameSize, mp3File_t *file)
{ if(byteCount > frameSize)   { printWarning(file);   fprintf(ostream, "Xing header too small: ");   return FAILURE; }   return SUCCESS; }

UC checkXingHeader(mp3File_t *file)
{
  UC     idOffset =  get_vbrFrame_idOffset(file);
  U4        flags = get_vbrFrame_xingFlags(file);
  U4    frameSize = get_vbrFrame_frameSize(file);
  U4      byteAcc;
  // Do we check that the frame can hold the values it says it has?
  // Do we do it here?
  // Maybe have functions for reporting (normal or SCAN
  // or we declare the frame invalid  --  set_vbrFrame_flag(file,  false); <== I like that
  // or consider the field invalid and we need to change the flag value

  // Case frameHeader = FF FE 14 00 --> frameSize = 32
  /*
    FF FE 14 00                                       <-- Frame tag
                44 55 66 77 88 99 AA BB CC DD EE FF
    00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    FF FA A0 08                                       <-- next frame
                58 69 6E 67                           <-- 32 bytes further: XING
                            .. .. .. ..               <-- flags
                                        .. .. .. ..   <-- frameCount if present
  */
  // a Xing frame was detected eventhough it would overlap the next frame
  //4 for Frame-tag + offset + 4 for XING tag + 4 for flags
  byteAcc = 4+idOffset+4+4;
  // <=== already tested to detect XING frame
  //  TRY(testSize(byteAcc, frameSize, file), fprintf(ostream, "VBR Frame overlaps following frame\n");)

  if (flags & 0x01) { byteAcc +=   4;   TRY(testSize(byteAcc, frameSize, file) , fprintf(ostream, "Not enough room for frameCount\n");); } // we took care of mentioning if frameCount was present already
  if (flags & 0x02) { byteAcc +=   4;   TRY(testSize(byteAcc, frameSize, file) , fprintf(ostream, "Not enough room for byteCount\n");); } // same for byteCount
  if (flags & 0x04) { byteAcc += 100;   TRY(testSize(byteAcc, frameSize, file) , fprintf(ostream, "Not enough room for TOC\n");); }
  if (flags & 0x08) { byteAcc +=   4;   TRY(testSize(byteAcc, frameSize, file) , fprintf(ostream, "Not enough room for quality indicator\n");); }

  return SUCCESS;
}

UC createXingFrame(mp3File_t *file, char **frameBlock, U4 *size)
{
  // from vbr-frame study
  // we will take ---> vbr_header = (first_header & 0xFFFF0FFF) + 0x00006000
  // it will fit frameCount, byteCount and TOC
  // and keep the frame specs. Only the bitrate is changed. Set to the minimum value that works

  U4  byteCount =  get_stream_byteCount(file);  // includes the vbrFrame when one is present
  U4 frameCount = get_stream_frameCount(file);  // same here

  // U4 chainCount = get_stream_chainCount(file);  // == 1; has been checked prior to calling.
  // maybe check in here and return FAILURE if conditions not met
  mp3Header_t   vbr_hdr;   U4   vbr_header;
  U4 xing_offset;
  U4 u4Val;

  if(get_vbrFrame_flag(file) == true)
    {
      byteCount  -= get_vbrFrame_frameSize(file);
      frameCount -= 1;
    }

  // we need a VBR stream       a stream where all frames have same length (in ms)   and only one chain
  if ( !(get_stream_isVbr(file)   &&   !get_stream_isVlength(file)   && (get_stream_chainCount(file) == 1)) ) // <== if chainCount == 1, then all frames in stream are equiv, then length is constant??
    return FAILURE;                                                                                           //                                              +- channelMode

  // set vbr_header // this is not the minimal size; bitrate index = 6 works for all cases, no matter
  vbr_header = (get_chain_first_header(file, 0) & 0xFFFF0FFF) + 0x00006000; // <=== maybe if first_hdr.frameSize is big enough, we keep it as a vbr_hdr
  // <== test if size < 120+xing_offset ?
  // get framesize
  setMp3Header(&vbr_hdr, vbr_header);
  *size = vbr_hdr.frameSize;
  *frameBlock = malloc(  *size);
  memset(*frameBlock, 0, *size);
//  printU4(byteCount, NL);
  byteCount  +=          *size;
  frameCount += 1;
  //get_vbrFrame... offset or something, no need to recompute
  xing_offset = xingOffset[vbr_hdr.channelMode][vbr_hdr.versionId-1];
  // write vbr_header
  u4Val = swapU4(vbr_header);   memcpy(*frameBlock, &u4Val, 4);
  u4Val = swapU4(XING);         memcpy(*frameBlock + 4+xing_offset,          &u4Val, 4);  // write "XING"
  u4Val = swapU4(0x00000007);   memcpy(*frameBlock + 4+xing_offset +4,       &u4Val, 4);  // write flags (0x00 00 00 07) ; no quality indicator
  u4Val = swapU4(frameCount);   memcpy(*frameBlock + 4+xing_offset +4 +4,    &u4Val, 4);  // write frameCount
  u4Val = swapU4( byteCount);   memcpy(*frameBlock + 4+xing_offset +4 +4 +4, &u4Val, 4);  // write  byteCount

  // generate TOC
  { mp3Header_t      hdr;
    U4 header;
    float chainLength = 0; // aka track length since there's only one chain
    float accLength = 0;
    float accSize = 0;
    UC         n = 0;
    UC val;
    U4 j;

      mseek(file, get_stream_start_offset(file), SEEK_SET);
      for(j=0; j<get_chain_frameCount(file,0); j++)
	{ getMp3Header(file, &hdr, &header);   chainLength += hdr.length;   mseek(file,  hdr.frameSize, SEEK_CUR); }

      mseek(file, get_stream_start_offset(file), SEEK_SET);
      for(j=0; j<frameCount; j++)
	{
	  getMp3Header(file, &hdr, &header);

	  accLength  += hdr.length;
	  accSize += hdr.frameSize;
	  if(accLength > chainLength*n/100)
	    if(n<100)
	      { val = (UC)(256.0*accSize/byteCount);
		memcpy(*frameBlock +xing_offset+20+n, &val, 1);
		n++;
	      }
	  mseek(file,  hdr.frameSize, SEEK_CUR);
	}
    }

  return SUCCESS;
}
