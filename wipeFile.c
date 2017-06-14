#include "wipeFile.h"
#include "includes.h"
#include "extern.h"
#include "err.h"
#include "mio.h"
#include "mp3File.h"
#include "byte.h"
#include "fileList.h"
#include "vbrFrame.h"
#include "mp3Frame.h"

UC   wipe_file(fileLink_t *link)
{
  /*
 mp3File_t  mpFile;
  char      *buffer;
  U4       data_size;
  char    *filename = link->filename;
  U4         offset = link->offset;
  char   *vbrBuffer;
  U4 vbrBuffer_size = 0;
  UC vbrFrame_OK;
//  char *beforeBuffer;
  U4      before_size;// = get_stream_start_offset(&mpFile);
  U4   start_offset;
  U4     end_offset;

  TRY(setMp3File    (&mpFile,   filename, offset, BSTREAM) , fprintf(ostream, "Could not open file \"%s\"\n", filename);)
  setMp3_offsets    (&mpFile            , offset);
  if(retrieveTag    (&mpFile,      0) == SUCCESS)  printFrameIDs(&mpFile, ALL, get_tag_size(&mpFile));
  TRY( processMp3Data(&mpFile, offset) , closeMp3File(&mpFile);)
  */

    /*
    // DO NOT confuse,     get_stream_isVbr
                       and get_vbrFrame_flag // it might exist and the file isn't vbr

** When doing a fix **
1> file is cbr and there is no vbrFrame ...
  DO NOTHING
2> file is cbr and there is  a vbrFrame ...
    we save everything before and everything after the vbr frame
    we dump the 2 blocks
3> file is vbr and there is no vbrFrame ...
    we create a xingFrame
    if successful
      we save everything before and everything after the start of the first frame
      we dump the before, the xingFrame and the after
    else DO NOTHING and display WARNING
4> file is vbr and there is  a vbrFrame ...
    either same as above
    OR     check vbrFrame first and re-create only if it is bad

IN THIS WIPE FUNCTION:
we get the data, the streams
if file isVbr, we create the vbrFrame

check if any work needs to be done before gathering buffers and re-dumping

*/

  /*
  if(get_vbrFrame_flag(&mpFile) == true)   before_size =     get_vbrFrame_offset(&mpFile);
  else                                     before_size = get_stream_start_offset(&mpFile); 
                                      start_offset = get_stream_start_offset(&mpFile);
                                        end_offset =   get_stream_end_offset(&mpFile);

  // <== get_tag_start_offset == offset
  // <== but it looks better this way :D
  fprintf(ostream, "Information about file, before the wipe\n");
  fprintf(ostream, "Before mp3 data : ");   printU4(               before_size, SP);//   printU4(offset, NL);
  fprintf(ostream,  "= %7"PRId32" bytes\n"       ,                    before_size);
  fprintf(ostream,    "       Mp3 data : ");   printU4(  end_offset - before_size, SP);
  fprintf(ostream,  "= %7"PRId32" bytes\n"       ,       end_offset - before_size    );
  fprintf(ostream, "After  mp3 data : ");   printU4(  get_fileSize(&mpFile) -   end_offset, SP);
  fprintf(ostream,  "= %7"PRId32" bytes\n",              get_fileSize(&mpFile) -   end_offset    );

//  if(before_size != 0)   {            beforeBuffer = malloc(before_size);   mseek(&mpFile,  0           , SEEK_SET);   mreadStr(beforeBuffer, before_size, &mpFile); }
// if before_size == after_size == 0
//   if isCBR and has no vbrFrame
//     DO NOTHING
//   else
//     GET TO WORK

// <===
//  if((start_offset == 0) && (get_fileSize(&mpFile) == end_offset))
//  if ((get_stream_isVbr(%mpFile) == false ) && (get_vbrFrame_flag() == false))
//  { //// print message?//// return SUCCESS; }
  vbrFrame_OK = createXingFrame               (&mpFile, &vbrBuffer, &vbrBuffer_size); // will fail if file is CBR
//  if(vbrFrame_OK == SUCCESS) printf("SUCCESS\n");
//  else printf("FAILURE\n");

  data_size = end_offset - start_offset;   buffer = malloc(  data_size);   mseek(&mpFile,  start_offset, SEEK_SET);   mreadStr(&mpFile, buffer,         data_size);

//  printU4(before_size, NL);
//  printU4(start_offset, NL);

  closeMp3File                    (&mpFile);
  resetFile(filename);      
  TRY( setMp3File(&mpFile, filename,  0, FSTREAM) ,   fprintf(ostream, "Could not open file \"%s\"\n", filename);)

//  if(   before_size != 0)   { mwriteStr(beforeBuffer,       before_size,      &mpFile);   free(beforeBuffer); }


  //if(get_stream_isVbr(&mpFile) == true)    //if(vbrBuffer_size != 0)
    if (vbrFrame_OK == SUCCESS)
      { mwriteStr(&mpFile,   vbrBuffer,    vbrBuffer_size);   free(   vbrBuffer); }
//                          mwriteStr(      buffer,         1024,      &mpFile);   free(      buffer);
//  exit(423);
                          mwriteStr(&mpFile,      buffer,         data_size);   free(      buffer);

  closeMp3File                    (&mpFile);
*/
  return SUCCESS;
}
