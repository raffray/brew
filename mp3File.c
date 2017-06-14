#include "mp3File.h"
#include "mp3Frame.h"
#include "mp3Stream.h"
#include "fstream.h"
#include "bstream.h"
#include "vbrFrame.h"
#include "extern.h"
#include "err.h"
#include "str.h"
#include "byte.h"
#include "mfile.h"

#include "id3v2Tag_header.h"
#include "mp3Frame_chain_list.h"


UC open_mpFile(mp3File_t *file, char *fname, UC streamType) // <== used once, to check for mp-stream.
{ memset(file, 0, sizeof(mp3File_t));
  file->streamType = streamType;
  scopy(&(file->filename), fname);
  TRY(mopen(file, fname) ,{})

  return SUCCESS;
}

UC setMp3File(mp3File_t *file, char *fname, U4 offset, UC streamType) // <== used when we need to process a file we know has an mp-stream.
{ // offset corresponds to the start of the stream, set when creating file list.
  UC res = open_mpFile(file, fname, streamType);
  mseek(file, offset, SEEK_SET);
  setVbrHeader(file);

  return res;
}

// set_fileSize done during setMp3File()
U4 get_fileSize   (mp3File_t *file         )   { if(file->streamType == FSTREAM)  return get_fstreamSize(&(file->fs)      );   else   return get_bstreamSize(&(file->bs)      ); }
VD set_fileSize   (mp3File_t *file, U4 size)   { if(file->streamType == FSTREAM)         set_fstreamSize(&(file->fs), size);   else          set_bstreamSize(&(file->bs), size); }
char *get_filename(mp3File_t *file)            { return file->filename;                 }

void closeMp3File               (mp3File_t *file)
{ if (file->streamType == FSTREAM)   f_close(&(file->fs));
  else                               b_close(&(file->bs), get_filename(file));

  // clean id3v2Tag
  clean_id3v2Tag(&(file->id3v2Tag));

  // clean vbrFrame

  // clean chain_list
  if(    get_stream_chainCount(file) !=0)   ; // <=== FREEING TO BE DONE    //free( (file->chainList).chain   );
//  if(get_tag_frameCount(file) !=0)   free( (file->tag ).frameList);
  free(    get_filename(file)); // file->filename
}

void set_firstFrame_offset       (mp3File_t *file, U4  offset) {        file->firstFrame_offset = offset;   }
U4   get_firstFrame_offset       (mp3File_t *file)             { return file->firstFrame_offset;            }
