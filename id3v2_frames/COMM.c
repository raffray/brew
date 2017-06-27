#include "includes.h"
#include "COMM.h"
#include "id3v2Tag_frame.h"
#include "mp3File.h"
#include "extern.h"
#include "byte.h"
//#include "text-encoding/utf-8.h"
#include "text-encoding/unicode.h"
#include "err.h"

/*
<Header for 'Comment', ID: "COMM">
Text encoding           $xx
Language                $xx xx xx
Short content descrip.  <text string according to encoding> $00 (00)
The actual text         <full text string according to encoding>


Example

00000150   43 4F 4D 4D 00 00 00 3A 00 00 01 65 6E 67 FF FE   COMM...:...eng..
00000160   00 00 FF FE 47 00 65 00 72 00 6D 00 61 00 6E 00   ....G.e.r.m.a.n.
00000170   79 00 2C 00 20 00 41 00 75 00 67 00 75 00 73 00   y.,. .A.u.g.u.s.
00000180   74 00 20 00 32 00 35 00 2C 00 20 00 31 00 39 00   t. .2.5.,. .1.9.
00000190   38 00 34 00 00 00 00 00 00 00 00 00 00 00 00 00   8.4.............

43 4F 4D 4D 00 00 00 3A 00 00
C  O  M  M
01
65 6E 67
e  n  g
FF FE 00 00
FF FE 47 00 65 00 72 00 6D 00 61 00 6E 00 79 00 2C 00 20 00 41 00 75 00 67 00 75 00 73 00 74 00 20 00 32 00 35 00 2C 00 20 00 31 00 39 00 38 00 34 00
      G     e     r     m     a     n     y     ,           A     u     g     u     s     t           2     5     ,           1     9     8     4

1 byte  --> encoding
3 bytes --> language... meaning: we need to make sure those are 3 ascii printable characters.
N bytes --> 2 strings, this data will be re-encoded into utf8

01
   65 6E 67
   e  n  g
            FF FE 00 00
                        FF FE 47 00 65 00 72 00 6D 00 61 00 6E 00 79 00
                              G     e     r     m     a     n     y
----UTF8----> 00 47 65 72 6D 61 6E 79 00
                 G  e  r  m  a  n  y
------------> COMM : ()[eng]: Germany

*/


UC has_empty_shortDesc_COMM(mp3File_t *file, U4 frameNb)
{ UC              *data     = get_id3v2Tag_frame_data    (file, frameNb);
  U4               dataSize = get_id3v2Tag_frame_dataSize(file, frameNb);
  buffer_t *utf8_buf;
  UC res;

  if(dataSize<4)   return false; // <== ??

  utf8_buf        = string_to_utf8_str(data+4, dataSize-4, true, *data);
  res = (utf8_buf->data == 0); //strlen(utf8_buf->data);

  free (utf8_buf->data);
  free(utf8_buf);

  return (res == 0);
}

U4 nth_utf8_string_start(buffer_t *str_buf, U4 nth)
{
  UC c;

  buffer_seek(str_buf, 0, SEEK_SET);
  while(nth>0)
  {
    buffer_readUC(str_buf, &c);
    if(c==0) nth--;
  }
  return str_buf->cursor;
}

void print_COMM(mp3File_t *file, U4 frameNb)
{ UC              *data     = get_id3v2Tag_frame_data    (file, frameNb);
  U4               dataSize = get_id3v2Tag_frame_dataSize(file, frameNb);

  buffer_t buf;
  UC enc;
  UC langBytes[4];
  UC *short_desc;
  UC *comm;
  buffer_t *str_buf;

  if(dataSize<4) {}
  else
  {
    buffer_open   (&buf, data, dataSize);
    buffer_readUC (&buf, &enc);
    buffer_readStr(&buf, &langBytes, 3);
    langBytes[3]=0;

    str_buf = string_to_utf8_str(data+4, dataSize-4, true, enc);
    short_desc = str_buf->data + nth_utf8_string_start(str_buf, 0);
    comm       = str_buf->data + nth_utf8_string_start(str_buf, 1);

    fprintf(ostream, "(%s)[%s]: %s", short_desc, langBytes, comm);
  }
}
