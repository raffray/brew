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

  /* Tags can utilise extended ascii characters too */

  if( !tag->presenceFlag ) return FAILURE;

  Fprintf("\n*******\nId3v1 Tag ****\n");

  /*
  fprintf(ostream, "Title   : ");   print\_string(tag->title  , strlen(tag->title  ), true, 0);   fprintf(ostream, "\n");
  fprintf(ostream, "Artist  : ");   print\_string(tag->artist , strlen(tag->artist ), true, 0);   fprintf(ostream, "\n");
  fprintf(ostream, "Album   : ");   print\_string(tag->album  , strlen(tag->album  ), true, 0);   fprintf(ostream, "\n");
  */
  fprintf(ostream, "Title   : %s\n", tag->title);
  fprintf(ostream, "Artist  : %s\n", tag->artist);
  fprintf(ostream, "Album   : %s\n", tag->album);

  fprintf(ostream, "Speed   : ");
  switch(atoi(tag->speed))
    { case 0: fprintf(ostream, "Unset\n"); break;
    case 1: fprintf(ostream, "Slow\n"); break;
    case 2: fprintf(ostream, "Medium\n"); break;
    case 3: fprintf(ostream, "Fast\n"); break;
    case 4: fprintf(ostream, "Hardcore\n"); break;
    default: break;
    }


  return SUCCESS;
}






/*
UC is_id3v1_xtd_present(mp3File_t *file)
{ U4 tag;

  if (        get_fileSize(file) < ID3V1_TAG_LENGTH + XTD_TAG_LENGTH) { return pExit("not an mp3 file\n"); }
  if (!get_id3v1Tag_presenceFlag(file))   return false;
  mseek(file, - ID3V1_TAG_LENGTH - XTD_TAG_LENGTH, SEEK_END);
  mreadU4(file, &tag);
  if(tag != 0x5441472B)   return false;

  return true;
}

UC print_id3v1_xtd_info(mp3File_t *file)
{ char buf[61];
  UC speed;


  if( !get_id3v1Tag_presenceFlag    (file) ) return FAILURE;
  if( !is_id3v1_xtd_present(file) ) return FAILURE;

  Fprintf("\n*******\nId3v1 Extended Tag\n");

  mseek(file, - ID3V1_TAG_LENGTH - 4, SEEK_END);
  mreadStr(file, buf, 60);   buf[30] = '\0';   fprintf(ostream, "Title      : %s\n", buf);
  mreadStr(file, buf, 60);   buf[30] = '\0';   fprintf(ostream, "Artist     : %s\n", buf);
  mreadStr(file, buf, 60);   buf[30] = '\0';   fprintf(ostream, "Album      : %s\n", buf);
  mreadUC (file, &speed );                     fprintf(ostream, "Speed      : "         );
  switch(speed)
    {
    case   0:                                    fprintf(ostream,    "Unset\n");   break;
    case   1:                                    fprintf(ostream,     "Slow\n");   break;
    case   2:                                    fprintf(ostream,   "Medium\n");   break;
    case   3:                                    fprintf(ostream,     "Fast\n");   break;
    case   4:                                    fprintf(ostream, "Hardcore\n");   break;
    default :                                    fprintf(ostream,        "?\n");   break;
    }

  mreadStr(file, buf, 60);   buf[30] = '\0';   fprintf(ostream, "Genre      : %s\n", buf);
  mreadStr(file, buf,  6);   buf[6]  = '\0';   fprintf(ostream, "Start Time : %s\n", buf);
  mreadStr(file, buf,  6);   buf[6]  = '\0';   fprintf(ostream, "End   Time : %s\n", buf);


//  mreadStr(file, buf, 28);   buf[28] = '\0';   fprintf(ostream, "Comment : %s"  , buf);
//  mreadUC (&flag  );   mreadUC (&track );   mreadUC (&genre );
//  if(flag!=0)                                  fprintf(ostream, "%c%c\n", flag    , track);   // the end of the comment
//  else                                         fprintf(ostream, "\nTrack#  : %d\n", track);
  ////                                         fprintf(ostream, "Genre   : %s (%d)\n", tconArray[genre], genre);

  Fprintf("*******\n");

  return SUCCESS;
}
*/
