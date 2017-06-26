#include "includes.h"
#include "COMM.h"
#include "id3v2Tag_frame.h"
#include "mp3File.h"
#include "extern.h"
#include "byte.h"
#include "text-encoding/utf-8.h"
#include "err.h"

/*
<Header for 'Comment', ID: "COMM">
Text encoding           $xx
Language                $xx xx xx
Short content descrip.  <text string according to encoding> $00 (00)
The actual text         <full text string according to encoding>
*/

// 00 l1 l2 l3 xx ... xx 00 xx ... xx <= no null termination, the frame length dictates where the string stops.
// 01 l1 l2 l3 FF FE xx yy ... xx yy 00 00 FF FE xx yy ... xx yy
// 01 l1 l2 l3 FE FF xx yy ... xx yy 00 00 FE FF xx yy ... xx yy
//
// yet... why the double Fx Fx? It's redundant; the first one states the endianness already.
// would one change it for the 2nd string? that's plain stupid IMHO

/*

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
		    <==== Do NOT forget to handle the BOM for the second string if utf16 encoded.

*/

/*
typedef struct COMM_frame
{

} COMM_frame_t;
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

void print_COMM(mp3File_t *file, U4 frameNb)
{ UC              *data     = get_id3v2Tag_frame_data    (file, frameNb);
  U4               dataSize = get_id3v2Tag_frame_dataSize(file, frameNb);

  buffer_t buf;
  UC enc;
  UC langBytes[3];


  if(dataSize<4) {}
  else
  {
    buffer_readUC(&buf, &enc);
    buffer_readStr(&buf, &langBytes, 3);
  }
}

void print_COMM_old(mp3File_t *file, U4 frameNb)
{ UC              *data     = get_id3v2Tag_frame_data    (file, frameNb);
  U4               dataSize = get_id3v2Tag_frame_dataSize(file, frameNb);

  /*
  buffer_t           data_buffer;
  buffer_t          *buf = get_id3v2Tag_frame_buffer(file, frameNb);

  UC            enc; // text encoding
  char  *lang_string; // printable language string (in utf-8 format)
  char   langBytes[3];
  buffer_t *lang_buf;

  char  *utf8_string;
  U4 utf8_string_len;
  buffer_t *utf8_buf;

  char  *desc_string; // short description
  U4 desc_string_len;

  char  *text_string; // comment itself
  U4 text_string_len;
  U4 end_str1;
  U4 end_str2;
  U4 i;
  UC c;
  buffer_t buffout;
  U4 firstU4;
  char *str;
  UC count = 0;
  U4 firstZero  = 0;
  U4 secondZero = 0;
  */


  if(dataSize<4) {   fprintf(ostream, "\n");/*error*/ }
  else
    {
//      buffer_readU4 (buf, &firstU4);
//      enc = (UC)(firstU4>>24);

      // 01 65 6E 67 FF FE 00 00 FF FE 47 00 65 00 72 00 6D 00 61 00 6E 00
      //    e  n  g
      // buf ---> 01 65 6E 67 00 47 65 72 6F 61 6E 00
      //
      // from 4th byte; we need to count 2 00
      // if frame is properly formed, we should count EXACTLY 2 zeros

//      for(i=4; i<buf->size; i++)   { buffer_readUC(buf, &c);   if(c==0) break;}

      // the rest of the buffer should contain 2 strings
      // 1) string_to_utf8(...
      // 2) find first nonZero character

// ///// first 4 bytes of buf are the encoding and 3 bytes for language; example

      //if first 0 encountered is the last of the string/buffer, there's only the short desc

//      buffer_seek(buf, 4, SEEK_SET);
//      lang_buf    = string_to_utf8_str(buf->data+4, buf->size -4, true, 0);

      /*
      buffer_seek(lang_buf, 0, SEEK_SET);
      for(i=0; i<lang_buf->size; i++)
	{
	  buffer_readUC(lang_buf, &c);
	  printUC(c, NL);
	  if(c==0)
	    {
	      count++;
	      if(count==1)
		{
		  end_str1 = i;
		}
	      else if(count==2)
		{
		  end_str2 = i;
		}
	    }
	}
      */

//      printf("\n");
//      buffer_print(buf);
//      buffer_print(lang_buf);
//      printf("\n\n");


//      printU4(end_str1, NL);
//      printU4(end_str2, NL);
//      printU4(i, NL);

//      printf("[%s]\n", lang_buf->data);
//      printf("[%s]\n", lang_buf->data+1);

//      exit(42);

      /*
     for(   ; i<buf->size; i++)   { buffer_readUC(buf, &c);   if(c==0) break;}
      end_str2 = i-1;

      // 01
//      fprintf(ostream, "(%s)[%s]: %s", desc_string, lang_string, text_string);
      buffer_init(&buffout);
      buffer_writeUC(&buffout, 0x28);

      buffer_seek(buf, 4);
//      buffer_rearStr(buf, str, start_str1
*/


      /*
      buffer_open   (&data_buffer, data, dataSize);
      buffer_readUC (&data_buffer, &enc);
      buffer_readStr(&data_buffer, langBytes, 3);
      buffer_close  (&data_buffer);

      lang_buf    = string_to_utf8_str(langBytes, 3, true, 0);
      lang_string = lang_buf->data;
      free(lang_buf);

      // the short description
      utf8_buf        = string_to_utf8_str(data+4, dataSize -4, true, enc);


//	the buffer's data would be like:   31 32 33 00 54 65 73 74 00
//	print first string as short desc, second as the comment itself.


      utf8_string     = utf8_buf->data;
      utf8_string_len = utf8_buf->size;
      free(utf8_buf);

      desc_string_len = strlen(utf8_string);
      desc_string = utf8_string; // utf8_string is always NULL terminated
      text_string_len = strlen(utf8_string+desc_string_len+1);
      if(text_string_len+desc_string_len+1<=utf8_string_len)   text_string = utf8_string + desc_string_len+1;
      else text_string = NULL;

      // //// //
//      if(desc_string_len == 0) printf(" [EMPTY] --> ");
//      else                     printf(" [NOT EMPTY] --> ");
      fprintf(ostream, "(%s)[%s]: %s", desc_string, lang_string, text_string);


      free(lang_string);
      free(utf8_string);
      */
    }
}
