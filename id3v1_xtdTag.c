#include "includes.h"
#include "extern.h"
#include "brew_types.h"
#include "id3v1_xtdTag.h"
#include "id3v1Tag.h"
#include "mp3File.h"
#include "byte.h"
#include "mfile.h"
#include "err.h"

UC get_id3v1_xtdTag_presenceFlag(mp3File_t *file)   { return (file->id3v1_xtdTag).presenceFlag; }

VD check_id3v1_xtdTag_presence(mp3File_t *file)
{ id3v1_xtdTag_t *tag = &(file->id3v1_xtdTag);
  U4 val;

  //  if (get_fileSize(file) < ID3V1_TAG_LENGTH + XTD_TAG_LENGTH) { return pExit("not an mp3 file\n"); }
  // <== impossible if file already is detected to be an mp3 it has at least a minimum number of mp3 frames

  mseek(file, - ID3V1_TAG_LENGTH - XTD_TAG_LENGTH, SEEK_END);
  mreadU4(file, &val);

  tag->presenceFlag = val == 0x5441472B;
}

UC retrieve_id3v1_xtdTag(mp3File_t *file)
{ id3v1_xtdTag_t *tag = &(file->id3v1_xtdTag);

  memset(tag, 0, sizeof(id3v1_xtdTag_t));

  check_id3v1_xtdTag_presence(file);
  if( !tag->presenceFlag ) return FAILURE;

  mseek(file, - ID3V1_TAG_LENGTH - XTD_TAG_LENGTH +4, SEEK_END);
  mreadStr(file, tag->title  , 60);
  mreadStr(file, tag->artist , 60);
  mreadStr(file, tag->album  , 60);
  mreadStr(file, tag->speed  ,  1);
  mreadStr(file, tag->start_time, 6);
  mreadStr(file, tag->  end_time, 6);

  return SUCCESS;
}

UC print_id3v1_xtdTag(mp3File_t *file)
{ id3v1_xtdTag_t *tag = &(file->id3v1_xtdTag);

  /* Tags can use extended ascii characters too */

  if( !tag->presenceFlag ) return FAILURE;

  fprintf(ostream, "\n*******\nId3v1 Tag ****\n");

  fprintf(ostream, "Title   : %s\n", tag->title);
  fprintf(ostream, "Artist  : %s\n", tag->artist);
  fprintf(ostream, "Album   : %s\n", tag->album);

  fprintf(ostream, "Speed   : ");
  switch(atoi((char*)tag->speed))
    { case 0: fprintf(ostream, "Unset\n"); break;
    case 1: fprintf(ostream, "Slow\n"); break;
    case 2: fprintf(ostream, "Medium\n"); break;
    case 3: fprintf(ostream, "Fast\n"); break;
    case 4: fprintf(ostream, "Hardcore\n"); break;
    default: break;
    }


  return SUCCESS;
}
