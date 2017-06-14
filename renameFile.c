#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>

#include "includes.h"
#include "extern.h"
#include "renameFile.h"
#include "filename.h"
#include "mp3File.h"
#include "fileList.h"
#include "id3v2_frames/Txyz.h"

#define  NB_RENAME_FRAMES  5

 //  "TPE2"        , "TALB" , "TRCK"  , "TPE1"  , "TIT2" 
 //  "Album Artist", "Album", "Track#", "Artist", "Title"
#define T_Album_Artist 0
#define T_Album        1
#define T_Track_Number 2
#define T_Artist       3
#define T_Title        4

//                                             "Album Artist"  , "Album"        , "Track#", "Artist"        , "Title"
char default_strings[NB_RENAME_FRAMES][15] = { "Unknown Artist", "Unknown Album",      "0", "Unknown Artist", "Unknown Track" };

UC  reName_file(fileLink_t *link)
{
 mp3File_t mpFile;
  char *filename = link->filename;
  U4     offset = link->offset;
  char *albumArtist;
  char *album;
  char *trackNb;
  UC    trackNum; // if >255... too bad :)
  char  trackNb2[3];
  char *artist;
  char *trackName;

  buffer_t buf;
  UC res;

  setMp3File    (&mpFile, filename,  offset, FSTREAM);
  
  res =  retrieve_id3v2Tag(&mpFile);
  if(res == FAILURE) return SUCCESS; // <== or FAILURE???

  artist      = get_Txyz_string4rename(&mpFile, "TPE1");
  albumArtist = get_Txyz_string4rename(&mpFile, "TPE2");
  album       = get_Txyz_string4rename(&mpFile, "TALB"); //  if(album       == NULL)   album       = "Unknown Album" ;
  trackNb     = get_Txyz_string4rename(&mpFile, "TRCK");
  trackName   = get_Txyz_string4rename(&mpFile, "TIT2"); //  if(trackName   == NULL)   trackName   = "Unknown Track" ;

  buffer_init(&buf);

//  if(trackName == NULL)  return SUCCESS; // FAILURE ?

  /*  if(albumArtist!=NULL)   buffer_append(&buf, albumArtist, true);
      else                    buffer_append(&buf, artist, true);*/
  if     (albumArtist!=NULL)  buffer_append(&buf, albumArtist, true);
  else
  {    if(artist     ==NULL)  buffer_append(&buf, "Unknown Artist", true);
    else                      buffer_append(&buf, artist, true);
  }

  mkdir(buf.data, S_IRWXU); // <=== should make sure we are within the working directory; if artist == "../something" ... :)

  if(album!=NULL)   buffer_append(&buf, album, true);
  else              buffer_append(&buf, "Unknown Album", true);

  mkdir(buf.data, S_IRWXU); // <=== should make sure we are within the working directory; if artist == "../something" ... :)

  if(trackNb!=NULL)
    { trackNum    = atoi(trackNb);
      sprintf(trackNb2, "%d", trackNum);
      if(trackNum <10)   buffer_append(&buf, "0", false);
      buffer_append(&buf, trackNb2, false); // <====== in case trackNb is of the form 5/12... it will be transformed, for display, into 5_12; we only want the 5
      buffer_append(&buf, " - "  , false); }

  if((artist!=NULL) && (albumArtist!=NULL) && strcmp(artist, albumArtist))
    { buffer_append(&buf, artist, false);
      buffer_append(&buf, " - " , false); }

  if(trackName!=NULL)   buffer_append(&buf, trackName, false);
  else                  buffer_append(&buf, "Unknown Track", false);
  

  free(albumArtist);
  free(album);
  free(trackNb);
  free(artist);
  free(trackName);

  if(filename_check(&buf, ".mp3") == SUCCESS)
    {
      rename(filename, buf.data); //resetFile(buf.data);
      fprintf(ostream, "%s --> %s\n", filename, buf.data);
    }
  else
      fprintf(ostream, "cannot move %s --> %s\n", filename, buf.data);

  return SUCCESS;
}
