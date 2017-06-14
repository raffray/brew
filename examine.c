#include "examine.h"
#include "includes.h"
#include "extern.h"
#include "err.h"
#include "mio.h"
#include "str.h"
#include "mp3File.h"
#include "byte.h"
#include "mfile.h"
#include "id3v2Tag.h"
#include "id3v1Tag.h"
#include "fileList.h"
#include "mp3StreamAnalysis.h"
#include "mp3Stream.h"

UC         printMode = ALL;     // ALL for everything, SELECT for only the track's length

buffer_t *makeV2fromV1Tag(mp3File_t *);

UC print_tags(fileLink_t *link)
{ mp3File_t mpFile;
  char *filename = link->filename;
  U4     offset = link->offset;
  //buffer_t *v2;

  setMp3File        (&mpFile, filename,  offset, FSTREAM);

  if(retrieve_id3v2Tag    (&mpFile) == SUCCESS)   print_id3v2Tag    (&mpFile);
//  if(retrieve_id3v1Tag    (&mpFile) == SUCCESS)   print_id3v1Tag    (&mpFile);
//  if(retrieve_id3v1_xtdTag(&mpFile) == SUCCESS)   print_id3v1_xtdTag(&mpFile);

//  v2 = makeV2fromV1Tag(&mpFile);
  /*
  printf("=================================\n");
  { char *buffer_in;
    FILE *out = fopen("out.mp3", "w");

    buffer_in = malloc(get_fileSize(&mpFile));
    mseek(&mpFile, 0, SEEK_SET);
    mreadStr(&mpFile, buffer_in, get_fileSize(&mpFile));

    fwrite(v2->data,1,v2->size,out);
    fwrite(buffer_in,1,get_fileSize(&mpFile),out);
    fclose(out);
    free(buffer_in); }
  printf("=================================\n");
  */
  closeMp3File(&mpFile);

  return SUCCESS;
}

UC examine_file(fileLink_t *link)
{ mp3File_t mpFile;
  char *filename = link->filename;
  U4     offset = link->offset;
//  UC res;
  /**/  //TRY( setMp3File(&mpFile, filename,  offset, BSTREAM); ,     fprintf(ostream, "Could not open file \"%s\"\n", filename);)
  setMp3File    (&mpFile, filename,  offset, BSTREAM); // <== FSTREAM?
//  setMp3_offsets(&mpFile           , offset);
  /**/                                             //     We know it's an mp3 file; we will parse all of it

  /**/
  /*
    if((printMode ==  ALL) && (retrieveTag2      (&mpFile, 0) == SUCCESS))   print_id3v2Tag_frameList(&mpFile);
    else if (printMode == SCAN)   { mseek(&mpFile, 0, SEEK_SET);   retrieveTagHeader2(&mpFile, 0); }
  */
//  res =  retrieve_id3v2Tag(&mpFile);

    if(printMode ==  ALL)
      {
	if(retrieve_id3v2Tag    (&mpFile) == SUCCESS)   print_id3v2Tag    (&mpFile);
	if(retrieve_id3v1Tag    (&mpFile) == SUCCESS)   print_id3v1Tag    (&mpFile);
	if(retrieve_id3v1_xtdTag(&mpFile) == SUCCESS)   print_id3v1_xtdTag(&mpFile);
      }
// && (res == SUCCESS))   print_id3v2Tag(&mpFile);
//    else if (printMode == SCAN)   { mseek(&mpFile, 0, SEEK_SET);   retrieveTagHeader2(&mpFile, 0); }

  /*
    what is needed for a silent scan?
    we need to retrieve the tag so the prog knows we have a tag (and not possible junk) at the start of the file.
    ----------> meaning we retrieve the full tag regardless of wether we do a full or a silent scan??? <====
   */

  TRY( processMp3Data(&mpFile, offset) , closeMp3File(&mpFile);)// <== it would fail on a non-mp3 file

    switch(printMode)
      {
      case ALL   : printMp3Info(&mpFile);   break;
      case SCAN  : silentScan  (&mpFile);   break;
      default    :                          break;
      }

//  dumpSelectTag_v0(&mpFile);

  closeMp3File(&mpFile);
  if (hasWarning == true)   fprintf(ostream, "\n");
//  /**/hasWarning = false;


  return SUCCESS;
}
