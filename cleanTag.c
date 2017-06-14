#include "cleanTag.h"
#include "includes.h"
#include "id3v2Tag.h"
#include "extern.h"
#include "err.h"
#include "mio.h"
#include "mp3File.h"
#include "byte.h"
#include "fileList.h"
#include "mp3Frame.h"


UC cleanTag_file(fileLink_t *link)
{
  mp3File_t  mpFile;
//  U4        tagSize; 
//  U4  actualTagSize; // as meant for an id3 Tag; the frames + the padding. Does not include the id3 header
//  U4       dataSize;
//  char *buffer_data; 
  char    *filename = link->filename;
  U4         offset = link->offset;
//  char *outName = "out.mp3";
//  mp3File_t  outFile;

  TRY( setMp3File       (&mpFile, filename, offset, FSTREAM) ,     fprintf(ostream, "Could not open file \"%s\"\n", filename);)
//  setMp3_offsets        (&mpFile          , offset);

//  dataSize = get_fileSize(&mpFile);
//  buffer_data = malloc(dataSize);

  retrieve_id3v2Tag(&mpFile);// == SUCCESS)   printFrameIDs(&mpFile, ALL, get_tag_size(&mpFile));
 
//  resetFile        (         outName);    
//  setMp3File       (&outFile, outName, 0, FSTREAM);

//  mseek(&mpFile, 0, SEEK_SET);
//  mreadStr (& mpFile, buffer_data, dataSize);
//  mwriteStr(&outFile, buffer_data, dataSize);

//  dump_id3v2Tag(&mpFile, &outFile);
  dump_id3v2Tag(&mpFile);

  closeMp3File(& mpFile);
//  closeMp3File(&outFile);

//  free(buffer_data);

  return SUCCESS;
}
