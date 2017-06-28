#include "includes.h"
#include "extern.h"
#include "brew_types.h"
#include "id3v1Tag.h"
#include "mp3File.h"
#include "byte.h"
#include "mfile.h"
#include "err.h"

UC get_id3v1Tag_presenceFlag(mp3File_t *file)   { return (file->id3v1Tag).presenceFlag; }

VD check_id3v1Tag_presence(mp3File_t *file)
{ id3v1Tag_t *tag = &(file->id3v1Tag);
  U4 val;

  // if(get_fileSize(file)<128) { return pExit("not an mp3 file\n"); } // <== impossible if file already is detected to be an mp3
  /**/                                                                 //     it has at least a minimum number of mp3 frames

  mseek(file, - ID3V1_TAG_LENGTH, SEEK_END);
  mreadU4(file, &val);

  tag->presenceFlag = (val&0xFFFFFF00) == 0x54414700;
}

UC retrieve_id3v1Tag(mp3File_t *file)
{ id3v1Tag_t *tag = &(file->id3v1Tag);

  memset(tag, 0, sizeof(id3v1Tag_t));

  check_id3v1Tag_presence(file);
  if( !tag->presenceFlag ) return FAILURE;

  mseek(file, - ID3V1_TAG_LENGTH +3, SEEK_END);
  mreadStr(file, tag->title  , 30);
  mreadStr(file, tag->artist , 30);
  mreadStr(file, tag->album  , 30);
  mreadStr(file, tag->year   ,  4);
  mreadStr(file, tag->comment, 31);
  tag->flag = tag->comment[28];
  if(tag->flag==0)    tag->track = tag->comment[29];
    /**/              tag->genre = tag->comment[30];

  // <==
  retrieve_id3v1_xtdTag(file);
  return SUCCESS;
}

UC print_id3v1Tag(mp3File_t *file)
{ id3v1Tag_t *tag = &(file->id3v1Tag);

  /* Tags can use extended ascii characters too */

  if( !tag->presenceFlag ) return FAILURE;

  fprintf(ostream, "\n*******\nId3v1 Tag\n");

  fprintf(ostream, "Title   : %s\n", tag->title);
  fprintf(ostream, "Artist  : %s\n", tag->artist);
  fprintf(ostream, "Album   : %s\n", tag->album);
  fprintf(ostream, "Year    : %s\n", tag->year);
  fprintf(ostream, "Comment : %s\n", tag->comment);

  if(tag->flag==0)   fprintf(ostream, "Track#  : %d\n"   ,           tag->track);
  /**/               fprintf(ostream, "Genre   : %s (%d)\n", tconArray[tag->genre], tag->genre);
  fprintf(ostream, "*******\n");

  return SUCCESS;
}
