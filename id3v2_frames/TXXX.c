#include <ctype.h>
#include "includes.h"
#include "TXXX.h"
#include "id3v2Tag_frame.h"
#include "mp3File.h"
#include "extern.h"
#include "byte.h"
#include "text-encoding/unicode.h"
#include "err.h"
#include "str.h"

/*
  <Header for 'User defined text information frame', ID: "TXXX">
  Text encoding $xx
  Description <text string according to encoding> $00 (00)
  Value <text string according to encoding>
*/

void print_TXXX(mp3File_t *file, U4 frameNb)
{ UC              *data     = get_id3v2Tag_frame_data    (file, frameNb);
  U4                 dataSize = get_id3v2Tag_frame_dataSize(file, frameNb);
//  buffer_t           data_buffer;
//  U4 enc;

  UC  *utf8_string;
  U4 utf8_string_len;
  buffer_t *utf8_buf;

  UC  *desc_string; // short description
  U4 desc_string_len;

  UC  *text_string; // comment itself
  U4 text_string_len;

  { utf8_buf        = string_to_utf8_str(data, dataSize, false, 0);
    utf8_string     = utf8_buf->data;
    utf8_string_len = utf8_buf->usedSize;
    free(utf8_buf);

      desc_string_len = strlen_(utf8_string);
      desc_string = utf8_string; // utf8_string is always NULL terminated
      text_string_len = strlen_(utf8_string+desc_string_len+1);
      if(text_string_len+desc_string_len+1<=utf8_string_len)   text_string = utf8_string + desc_string_len+1;
      else text_string = NULL;

      fprintf(ostream, "(%s): %s", desc_string, text_string);

      free(utf8_string);
    }
//    printf("...");
}
