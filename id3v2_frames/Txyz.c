#include <ctype.h>
#include "includes.h"
#include "Txyz.h"
#include "id3v2Tag_frame.h"
#include "mp3File.h"
#include "extern.h"
#include "byte.h"
#include "err.h"
#include "str.h"
#include "text-encoding/utf-8.h"

/*
<Header for 'Text information frame', ID: "T000" - "TZZZ", excluding "TXXX"
Text encoding    $xx
Information    <text string according to encoding>

!! TCON will be treated separately !!

Text encoding:
     $00   ISO-8859-1 [ISO-8859-1].                                 Terminated with $00.
     $01   UTF-16 [UTF-16] encoded Unicode [UNICODE] with BOM.      Terminated with $00 00.
     All strings in the same frame SHALL have the same byteorder.

Id3v2.4 ONLY
     $02   UTF-16BE [UTF-16] encoded Unicode [UNICODE] without BOM. Terminated with $00 00.
     $03   UTF-8 [UTF-8] encoded Unicode [UNICODE].                 Terminated with $00.
*/

void print_Txyz(mp3File_t *file, U4 frameNb)
{// char   *data   = get_id3v2Tag_frame_data    (file, frameNb);
 // U4  dataSize   = get_id3v2Tag_frame_dataSize(file, frameNb);
  buffer_t * buf = get_id3v2Tag_frame_buffer  (file, frameNb);
//printf("START PRINTING STRING\n");
  print_string_buf(buf, false, 0);
//printf("END   PRINTING STRING\n");
}


UC replace_badChar(buffer_t *buf)
{ // the utf-8 string in "buf is properly formed
  // Technically, any char in the string <= 7F (0%0111 1111) can only be found in single-byte characters
  // and therefore we could just replace every instance of forbidden char without affecting multi-byte characters
  UC byte;

//  if(
  buffer_readUC(buf, &byte)/* == FAILURE)        return FAILURE*/;
  if(byte == '/')   { buffer_seek(buf, -1, SEEK_CUR);   buffer_writeUC(buf, '_'); } // <== other forbidden characters than '/'?
  if(byte < 0xC2)                                       return SUCCESS;
  if(byte < 0xE0)     buffer_seek(buf,  1, SEEK_CUR);   return SUCCESS; // 2-byte encoding
  if(byte < 0xF0)     buffer_seek(buf,  2, SEEK_CUR);   return SUCCESS; // 3-byte encoding
  if(byte < 0xF5)     buffer_seek(buf,  3, SEEK_CUR);   return SUCCESS; // 4-byte encoding
}

UC find_strID_index(mp3File_t *file, char *strID, U4 *index)
{
  U4 i;

  for(i=0; i<get_id3v2Tag_frameCount(file); i++)
    if(get_id3v2Tag_frame_frameID(file, i) == str2id((UC*)strID))
      { *index = i;   return SUCCESS; }
  return FAILURE; // strID not found
}

UC *get_Txyz_string4rename(mp3File_t *file, char *strID)
{
  U4     index;
  UC   *data;
  U4  dataSize;

  buffer_t *buf;
  UC *bufString;
  U4 firstZero;

  UC *str;

  if (find_strID_index(file, strID, &index) == FAILURE)   return NULL;

  data = get_id3v2Tag_frame_data    (file, index);
  dataSize = get_id3v2Tag_frame_dataSize(file, index);


  buf = string_to_utf8_str(data, dataSize, false, 0);
  bufString = buf->data;
  //  trackName = get_Txyz_string(data, dataSize, false, 0);
  firstZero = strlen_(bufString);

  // We are working on a string already in utf-8 format

  // if we have a multi string... consider only the first one.
  // replace all non-allowed characters by '_'
  // List of such characters:  /

  buffer_seek(buf, 0, SEEK_SET);
  while(buffer_tell(buf)<=firstZero)
    replace_badChar(buf);

//  buffer_close(buf); // <== this would add a '/0' to buf->data
  str = buf->data;
  free(buf);

  return str;
}




void setForRenameStr(mp3File_t *file, U4 frameNb, char **to_string) // all such strings are of the T... form
{
  /*
 frame_t *frame = get_tag_frame(file, frameNb);
  U4 endFrame_offset = frame->start_offset + frame->size;
  UC enc; // encoding

  // case UTF16... Str should be NULL
  mseek(file, frame->start_offset, SEEK_SET);
  mreadUC(file, &enc);
  getString(file, enc, endFrame_offset);
//  printf(" <----%s\n", Str);
  scopy(to_string, Str);
     //printString(file, c, endFrame_offset);

//  if (to_string!=NULL) printf("        -- %s\n", *to_string);
//  else                 printf("NULL\n");

  free(Str);
  */
}
