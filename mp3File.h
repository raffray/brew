#ifndef MP3FILE_
#define MP3FILE_

#include "includes.h"
#include "brew_types.h"
#include "bstream.h"
#include "fstream.h"
#include "id3v1Tag.h"
#include "id3v1_xtdTag.h"
#include "id3v2Tag.h"
#include "vbrFrame.h"
#include "mp3Frame_chain_list.h"
#include "length_map.h"

//  printSection_short(get_chain_end_offset(file, get_chain_end_offset(file, get_chainCount(file)-1)));
typedef struct mp3File
{
  char           *filename;

  id3v2Tag_t         id3v2Tag;
  id3v1Tag_t         id3v1Tag;
  id3v1_xtdTag_t id3v1_xtdTag;
  UC            vbrFrame_flag; // <== put this in vbrFrame_t. And in general, put flags in the "type" itself
  vbrFrame_t         vbrFrame;

  // file stream
  UC            streamType;
  fstream_t             fs;
  bstream_t             bs;

  // mp3 stream
  U4     firstFrame_offset; // <==  stream_start_offset if vbrFrame_flg==false /// vbrFrame_offset if true
  U4   stream_start_offset;
  U4     stream_end_offset;

  U4             byteCount;
  UC                 isVbr;
  U2               bitRate;
  UC             isVlength;
  length_map_t         map;
  chain_list_t  chain_list;
//  UC non_equivHeaders_flag;

} mp3File_t;

UC      open_mpFile    (mp3File_t *, char *,     UC);
UC       setMp3File    (mp3File_t *, char *, U4, UC);
U4     get_fileSize    (mp3File_t *);
VD     set_fileSize    (mp3File_t *, U4);
char *get_filename(mp3File_t *);

void closeMp3File(mp3File_t *);

void set_firstFrame_offset(mp3File_t *, U4);
U4   get_firstFrame_offset(mp3File_t *    );

#endif //  MP3_FILE
