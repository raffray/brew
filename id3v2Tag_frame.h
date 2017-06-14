#ifndef ID3V2TAG_FRAME_
#define ID3V2TAG_FRAME_

#include "includes.h"
#include "brew_types.h"
#include "buffer.h"

/*
  All the frames consist of a frame header followed by one or more fields containing the actual information. The layout of the frame header:

  Frame ID       $xx xx xx xx (four characters)
  Size           $xx xx xx xx
  Flags          $xx xx
*/


/*
4> frame flags
    v2.3			|  v2.4
    %abc00000 %ijk00000		|  %0abc0000 %0h00kmnp
				|
a - Tag alter preservation	|  a - Tag alter preservation
b - File alter preservation	|  b - File alter preservation
c - Read only			|  c - Read only
				|
i - Compression			|
j - Encryption			|  h - Grouping identity
k - Grouping identity		|
				|  k - Compression
				|  m - Encryption
  				|  n - Unsynchronisation
				|  p - Data length indicator
////////////
newflags = flags & 0x00E0        | &0x004D

we disregard flags a, b anc
we keep compression, encryption and grouping
///////////
*/


extern U4 strLen;
extern U4 idx;
//extern char *Str;

typedef struct frame_info
{
  UC compression_flag;   U4 decompressedSize;
  UC encryption_flag;    UC encryptionMethod;
  UC grouping_flag;      UC groupID;
  UC unsync_flag;                       // id3v2.4 only  // <== unused since we store frames re-synched
  UC dataLength_flag;    U4 dataLength; // id3v2.4 only
  UC bytesSkipped;
} frame_info_t;

typedef struct id3v2Tag_frameLink // <=== We will usually refer to a frameLink as a frame ???
{
  U4   frameID;
  buffer_t *buf;        //  char *data;   U4  dataSize; // the size of the content; does not include the header
  frame_info_t *info;

  id3v2Tag_frameLink_t  *next;
} id3v2Tag_frameLink_t;

id3v2Tag_frameLink_t *get_id3v2Tag_frame(mp3File_t *, U4);
U4    get_id3v2Tag_frame_frameID        (mp3File_t *, U4);
U4    get_id3v2Tag_frame_dataSize       (mp3File_t *, U4);                        //U4 get_id3v2Tag_frame_realSize (mp3File_t *, U4);
U4    get_id3v2Tag_frame_compression_flag     (mp3File_t *, U4);
U4    get_id3v2Tag_frame_decompressedSize     (mp3File_t *, U4);
U4    get_id3v2Tag_frame_encryption_flag      (mp3File_t *, U4);
U4    get_id3v2Tag_frame_encryptionMethod     (mp3File_t *, U4);
U4    get_id3v2Tag_frame_grouping_flag        (mp3File_t *, U4);
U4    get_id3v2Tag_frame_groupID              (mp3File_t *, U4);
U4    get_id3v2Tag_frame_dataLength_flag      (mp3File_t *, U4);
U4    get_id3v2Tag_frame_dataLength           (mp3File_t *, U4);
char *get_id3v2Tag_frame_data           (mp3File_t *, U4);
buffer_t *get_id3v2Tag_frame_buffer     (mp3File_t *, U4);

UC process_id3v2Tag_frame_flags   (mp3File_t *, frame_info_t *, buffer_t *, U4,  UC);
U4              str2id(UC [4]);
UC       isSelectFrame(U4);

U4 get_id3v2Tag_frameSize(U4, UC);
U4 set_id3v2Tag_frameSize(U4, UC);

U4 retrieve_id3v2Tag_frame(mp3File_t *, buffer_t *);
UC print_id3v2Tag_frame(mp3File_t *, U4);
U4 dump_id3v2Tag_frame(mp3File_t *, buffer_t *, U4);

/*
void printChar      (UC);
void print_unicodeC (U2, UC);
void printIsoStr    (mp3File_t *, U4);
void printUnicodeStr(mp3File_t *, U4);
void printUTF16Str  (mp3File_t *, U4);
UC   printUTF8Char  (mp3File_t *, U4);
void printUTF8Str   (mp3File_t *, U4);
void getString      (mp3File_t *, UC         , U4);
void printString_    (char*         , U4);
void printFrame_     (mp3File_t *, U4         , UC);
U4   dumpFrame      (mp3File_t *, mp3File_t *, U4);
*/

#endif // ID3V2TAG_FRAME_
