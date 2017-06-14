#include "includes.h"
#include "extern.h"
#include "err.h"
#include "mio.h"
#include "mp3File.h"
#include "byte.h"
#include "mfile.h"
#include "fileList.h"
#include "mp3Frame.h"

UC extractPics_file(fileLink_t *link)
{ mp3File_t mpFile;
  char *filename = link->filename;
  U4     offset  = link->offset;

      //TRY( setMp3File(&mpFile, filename,  offset, BSTREAM),     fprintf(ostream, "Could not open file \"%s\"\n", filename);)
  setMp3File    (&mpFile, filename,  offset, FSTREAM);

  retrieve_id3v2Tag(&mpFile);

  extractPictures(&mpFile);
  closeMp3File(&mpFile);

  return SUCCESS;
}
