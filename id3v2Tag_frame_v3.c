#include "includes.h"
#include "extern.h"
#include "byte.h"
#include "err.h"
#include "id3v2Tag.h"
#include "id3v2Tag_frame.h"
#include "id3v2Tag_frame_v3.h"

//#define  NB_SELECT_FRAMES 16

// in v2.4: all text fields (not TXXX) can contain multiple values separated by a null character.
// in v2.3: can contain multiple values separated by the "/" character. (Full list: TPE1 TCOM TEXT TOLY TOPE)

/*
// if v1 is not present, v1_xtd isn't either (shouldn't)
// but v1_xtd is optional

#define TIT2 0
#define TPE1 1
#define TALB 2
#define TCON 3
#define TIT2 0
#define TIT2 0
#define TIT2 0
#define TIT2 0

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
UC selectFrames     [NB_SELECT_FRAMES][ 5] = { "TIT2", "TPE1", "TPE2", "TALB", "TRCK", "TYER", "TCON", "TCOM", 
					       "TPE3", "POPM", "COMM", "TPOS", "TMOO", "TDAT", "TDRC", "APIC"}; // APIC (picture) & TPOS (disc 1/2 for example)
UC selectFrames_desc[NB_SELECT_FRAMES][13] = { "Title", "Artist", "Album Artist", "Album", "Track#", "Year", "Genre", "Composer", 
					       "Conductor", "Pop. Meter", "Comment", "Compilation", "Mood" ,"Date", "Rec. Date", "Picture"} ;
*/

id3v2Tag_frame_select_ frame_select[FRAME_COUNT_V3] = { {0, "TIT2", "Title"}, {0, "TPE1", "Title"} };

UC process_id3v2Tag_frame_flags_v3(frame_info_t *info, buffer_t *buf, U4 end)
{
  U2 flags;
  UC ret;

  buffer_readU2(buf, &flags);

  info->compression_flag = ((flags &0x0080) == 0x0080); // if set, 4 bytes will be appended to the frame header. for 'decompressed size' 
  info->encryption_flag  = ((flags &0x0040) == 0x0040); // If set, 1 byte  will be appended to the frame header. indicating with which method it was encrypted
  info->grouping_flag    = ((flags &0x0020) == 0x0020); // If set, 1 byte  will be appended to the frame header. a group identifier
  info->unsync_flag      = 0; // <== useless, unused in v3
  info->dataLength_flag  = 0; // <== useless, unused in v3

  info->bytesSkipped = info->grouping_flag + info->encryption_flag + 4*info->compression_flag; // + 4*info->dataLength_flag; // == 0 here

  if(buffer_tell(buf)+info->bytesSkipped>end)   return FAILURE; //<== just added; ??
  if (info->compression_flag)   ret = buffer_readU4(buf, &(info->decompressedSize));   if((ret==FAILURE) || buffer_tell(buf)>end)  return FAILURE;
  if (info-> encryption_flag)   ret = buffer_readUC(buf, &(info->encryptionMethod));   if((ret==FAILURE) || buffer_tell(buf)>end)  return FAILURE;
  if (info->   grouping_flag)   ret = buffer_readUC(buf, &(info->groupID         ));   if((ret==FAILURE) || buffer_tell(buf)>end)  return FAILURE;

  return SUCCESS;
}

U4 get_id3v2Tag_frameSize_v3(U4 val)   { return val; }
U4 set_id3v2Tag_frameSize_v3(U4 val)   { return val; }

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
*/
