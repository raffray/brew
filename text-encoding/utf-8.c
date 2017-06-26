#include "utf-8.h"
#include "utf-16.h"
#include "iso-8859-1.h"
#include "ascii.h"
#include "err.h"
#include "str.h"

/* UTF8 - specs

// http://en.wikipedia.org/wiki/UTF-8
// http://www.utf8-chartable.de/unicode-utf8-table.pl
// http://www.unicode.org/faq/private_use.html#noncharacters
// http://stackoverflow.com/questions/1319022/really-good-bad-utf-8-example-test-data

Bits of     First       Last        Bytes in
code point  code point  code point  sequence 	Byte 1 	        Byte 2 	        Byte 3 	        Byte 4 	        Byte 5 	        Byte 6
 7 	    U+0000 	    U+007F 	        1 	0xxxxxxx
11 	    U+0080    	U+07FF         	2 	110xxxxx 	10xxxxxx
16 	    U+0800 	    U+FFFF 	        3 	1110xxxx 	10xxxxxx 	10xxxxxx
21 	    U+10000 	  U+1FFFFF 	      4 	11110xxx 	10xxxxxx 	10xxxxxx 	10xxxxxx
26 	    U+200000  	U+3FFFFFF 	    5 	111110xx 	10xxxxxx 	10xxxxxx 	10xxxxxx 	10xxxxxx
31 	    U+4000000 	U+7FFFFFFF 	    6 	1111110x 	10xxxxxx 	10xxxxxx 	10xxxxxx 	10xxxxxx 	10xxxxxx

In November 2003, UTF-8 was restricted by RFC 3629 to end at U+10FFFF, in order to match the constraints of the UTF-16 character encoding.
This removed all 5- and 6-byte sequences, and about half of the 4-byte sequences.

According to the UTF-8 definition (RFC 3629) the high and low surrogate halves used by UTF-16 (U+D800 through U+DFFF) are not legal Unicode values,
and their UTF-8 encoding should be treated as an invalid byte sequence.

Whether an actual application should do this is debatable, as it makes it impossible to store invalid UTF-16 (that is, UTF-16 with unpaired surrogate halves)
in a UTF-8 string. This is necessary to store unchecked UTF-16 such as Windows filenames as UTF-8.
*/

// We need to parse through the byte string; every n-byte sequence is converted into it's hexa code-point
//
// If we meet an error... Translate the first byte into the replacement character ("�" (U+FFFD)) and continue parsing with the next byte.



// NEED TO SPLIT THIS FILE IN 2
// 1> Strict utf-8 stuff, as for ascii/iso/utf-16
// 2> more specific stuff, like prints

// PRINT needs
// 1> print one string
//
//
// 2> print all strings
//   In case our buffer ends with a block of zeroes,





void eatZeroes(buffer_t *buf)
{ UC c;
  while(buffer_tell(buf)<buf->usedSize)
  {
    buffer_readUC(buf, &c);
    if(c!=0)
    {
      buffer_seek(buf, -1, SEEK_CUR);
      break;
    }
  }
}

void findNextZero(buffer_t *buf)
{ UC c;
  while(buffer_tell(buf)<buf->usedSize)
  { buffer_readUC(buf, &c);
    if(c==0)   break; }
}


UC print_string_str(UC *data, U4 size, UC type_known, UC type_given)
{ buffer_t buf;

  buffer_open  (&buf, data, size);
  return print_string_buf(&buf, type_known, type_given);
}

UC print_string_buf(buffer_t *buffin, UC type_known, UC type_given)
{
  buffer_t *buf;
  UC firstString_flag =0;

  if(!buffin->usedSize)   return SUCCESS; // Empty string, nothing to print

  buf = string_to_utf8_buf(buffin, type_known, type_given); // We are guaranteed we at least have one zero at the end
//buffer_print(buf);
  buffer_seek(buf, 0, SEEK_SET);
  while(buffer_tell(buf)<buf->usedSize)
  {
    eatZeroes(buf);
    if(firstString_flag==0)   firstString_flag++;
    else                      printf("\n       ");

    fprintf(ostream, "%s", buf->data+buffer_tell(buf));
    findNextZero(buf); //buffer_seek(buf,strlen(buf->data+buffer_tell(buf)),SEEK_CUR);
  }

  free(buf->data);
  free(buf);

  return SUCCESS;
}

buffer_t *string_to_utf8_str(UC *data, U4 size, UC type_known, UC type_given)
{
  buffer_t buf;

  buffer_open(&buf, data, size);

  return string_to_utf8_buf(&buf, type_known, type_given);
}

buffer_t *string_to_utf8_buf(buffer_t *buffin, UC type_known, UC type_given)
{ //                                           true/false     0,1,2 or 3
  // one utf-8  fits into either 1, 2, 3 or 4 bytes
  // one utf-16 fits into either    2    or 4 bytes
  // one codepoint fits into 4 bytes (actually 3 since the max value is 0x10FFFF, but we assign it a U4)

 //  buffer_t buffin;
  buffer_t *buffout = malloc(sizeof(buffer_t));
  U4 codepoint;
  UC type;
  UC enc;
 //  printf("====================== 1 =\n");
 //  buffer_open(buffin, str, len);
  buffer_init_default(buffout          );

  if(type_known)   type = type_given;
  else             buffer_readUC(buffin, &type);

 /*
 Text encoding:
      $00   ISO-8859-1 [ISO-8859-1].                                 Terminated with $00.
      $01   UTF-16 [UTF-16] encoded Unicode [UNICODE] with BOM.      Terminated with $00 00.
      All strings in the same frame SHALL have the same byteorder.
      $02   UTF-16BE [UTF-16] encoded Unicode [UNICODE] without BOM. Terminated with $00 00.
      $03   UTF-8 [UTF-8] encoded Unicode [UNICODE].                 Terminated with $00.
 */
 // so, for case 2, enc = B_ENDIAN
  switch(type)
    {
    case 1:
    case 2:   if(get_UTF16_enc(buffin,            &enc) == SUCCESS) // <== do we need to test for SUCCESS?
	              while(     nextUTF16    (buffin, &codepoint, enc) == SUCCESS)
	                { if (codepoint == 0) // multi-strings
	                    { if(get_UTF16_enc(buffin,            &enc) == SUCCESS) // BOM at start of each new string
		                      if(write_codepoint_to_utf8(buffout,   0) == FAILURE)   pExit("???"); }
	                  else
	                    if(write_codepoint_to_utf8(buffout, codepoint) == FAILURE)   pExit("???");
	                }
              break;
    case 0:   while(nextIso  (buffin, &codepoint) == SUCCESS)   { if(write_codepoint_to_utf8(buffout, codepoint) == FAILURE)   pExit("???"); }   break;
    case 3:   while(nextUTF8 (buffin, &codepoint) == SUCCESS)   { if(write_codepoint_to_utf8(buffout, codepoint) == FAILURE)   pExit("???"); }   break;
    case 0xFF:while(nextAscii(buffin, &codepoint) == SUCCESS)   { if(write_codepoint_to_utf8(buffout, codepoint) == FAILURE)   pExit("???"); }   break;
    default: break;
    }

 //  buffer_close(buffin ); // <== do not use or it would add a '/0' to buffin.data
 //  buffer_close( buffout);
 //  free(buffout->data);
 //  free(buffout)
  buffer_writeUC(buffout, 0x00);

  return buffout;
}

UC nextUTF8(buffer_t *buf, U4 *w) //converts the character(s) at the current position in the "buf" stream into its codepoint equivalent.
{
  UC byte1, byte2, byte3, byte4;
  U4 res;

  if(buffer_readUC(buf, &byte1) == FAILURE)                                             return FAILURE;
  if(byte1 < 0x80)                 { *w = byte1;                                        return SUCCESS; } // Ascii
  if(byte1 < 0xC0)                 { *w = ERR_CHAR;                                     return SUCCESS; } // "Extended" Ascii... not-allowed
  if(byte1 < 0xC2)                 { *w = ERR_CHAR;                                     return SUCCESS; } // C0 or C1; Overlong encoding

 // 2-byte encoding
 //  Bits of code point    First code point      Last code point       Bytes in sequence	       Byte 1 	        Byte 2
 //  11                    U+0080                U+07FF                2                        110xxxxx        10xxxxxx
  if(byte1 < 0xE0) // 1110 0000
    { if( buffer_readUC(buf, &byte2) == FAILURE)                                        return FAILURE;
      if((byte2 & 0xC0) != 0x80) // 1000 0000, continuation byte
	/**/                       { *w = ERR_CHAR;   buffer_seek(buf, -1, SEEK_CUR);   return SUCCESS; }

      res = (byte1 & 0x1F)<<6 | (byte2 & 0x3F);
      if(res < 0x80)               { *w = ERR_CHAR;   buffer_seek(buf, -1, SEEK_CUR);   return SUCCESS; } // Overlong Encoding
      /**/                           *w = res;                                          return SUCCESS; }

 // 3-byte encoding
 //
 //  Bits of code point    First code point      Last code point       Bytes in sequence	       Byte 1 	        Byte 2 	        Byte 3
 //  16                    U+0800                U+FFFF                3                        1110xxxx         10xxxxxx        10xxxxxx
  if(byte1 < 0xF0) // 1111 0000
    { if( buffer_readUC(buf, &byte2) == FAILURE)                                        return FAILURE;
      if((byte2 & 0xC0) != 0x80)   { *w = ERR_CHAR;   buffer_seek(buf, -1, SEEK_CUR);   return SUCCESS; }
      if( buffer_readUC(buf, &byte3) == FAILURE)                                        return FAILURE;
      if((byte3 & 0xC0) != 0x80)   { *w = ERR_CHAR;   buffer_seek(buf, -2, SEEK_CUR);   return SUCCESS; }

      res = (byte1 & 0x0F)<<12 | (byte2 & 0x3F)<<6 | (byte3 & 0x3F);
      if(res < 0x800)              { *w = ERR_CHAR;   buffer_seek(buf, -2, SEEK_CUR);   return SUCCESS; } // overlong encoding
      if((res>0xD7FF)&&(res<0xE000)){*w = ERR_CHAR;   buffer_seek(buf, -2, SEEK_CUR);   return SUCCESS; } // UTF-16 surrogates
      /**/                           *w = res;                                          return SUCCESS; }

 // 4-byte encoding
 //
 //  Bits of code point    First code point      Last code point       Bytes in sequence	       Byte 1 	        Byte 2 	        Byte 3 	        Byte 4
 //  21                    U+10000               U+1FFFFF              4                        11110xxx 	10xxxxxx 	10xxxxxx 	10xxxxxx
  if(byte1 < 0xF5)
    { if( buffer_readUC(buf, &byte2) == FAILURE)                                        return FAILURE;
      if((byte2 & 0xC0) != 0x80)   { *w = ERR_CHAR;   buffer_seek(buf, -1, SEEK_CUR);   return SUCCESS; }
      if( buffer_readUC(buf, &byte3) == FAILURE)                                        return FAILURE;
      if((byte3 & 0xC0) != 0x80)   { *w = ERR_CHAR;   buffer_seek(buf, -2, SEEK_CUR);   return SUCCESS; }
      if( buffer_readUC(buf, &byte4) == FAILURE)                                        return FAILURE;
      if((byte4 & 0xC0) != 0x80)   { *w = ERR_CHAR;   buffer_seek(buf, -3, SEEK_CUR);   return SUCCESS; }

      res = (byte1 & 0x0F)<<18 | (byte2 & 0x3F)<<12 | (byte3 & 0x3F)<<6 | (byte4 & 0x3F);
      if(res <  0x10000)           { *w = ERR_CHAR;   buffer_seek(buf, -3, SEEK_CUR);   return SUCCESS; } // overlong encoding
      if(res > 0x10FFFF)           { *w = ERR_CHAR;   buffer_seek(buf, -3, SEEK_CUR);   return SUCCESS; } // limit of UTF-8
      /**/                           *w = res;                                          return SUCCESS; }
  /**/                               *w = ERR_CHAR;                                     return SUCCESS;
}

UC write_codepoint_to_utf8_1byte(buffer_t *buf, U4 cp)   { buffer_writeUC(buf, cp);                                                                                                   return SUCCESS; }
UC write_codepoint_to_utf8_2byte(buffer_t *buf, U4 cp)   {                                                buffer_writeU2(buf, ((cp<<2)&    0x1F00) + (cp&    0x003F) +     0xC080);   return SUCCESS; }
UC write_codepoint_to_utf8_3byte(buffer_t *buf, U4 cp)   { buffer_writeUC(buf, ((cp>>12)&0x0F) + 0xE0);   buffer_writeU2(buf, ((cp<<2)&    0x3F00) + (cp&    0x003F) +     0x8080);   return SUCCESS; }
UC write_codepoint_to_utf8_4byte(buffer_t *buf, U4 cp)   { buffer_writeU4(buf, ((cp<< 6)&0x07000000) + ((cp<<4)&0x003F0000) + ((cp<<2)&0x00003F00) + (cp&0x0000003F) + 0xF0808080);   return SUCCESS; }
UC write_codepoint_to_utf8   (buffer_t *buf, U4 cp)
{ if (is_nonPrintable(cp))   return write_codepoint_to_utf8_3byte(buf, ERR_CHAR);
  if(cp<0x80    )            return write_codepoint_to_utf8_1byte(buf,       cp);
  if(cp<0x800   )            return write_codepoint_to_utf8_2byte(buf,       cp);
  if(cp<0x10000 )            return write_codepoint_to_utf8_3byte(buf,       cp);
  if(cp<0x110000)            return write_codepoint_to_utf8_4byte(buf,       cp);
  /**/                       return FAILURE;  /* Cannot happen with a codepoint we generated */
}

UC is_nonPrintable(U4 cp)
{ if(((cp>0) && (cp<0x05)) || (cp==0x06) || ((cp>0x0F) && (cp<0x20)) || ((cp>0x7E) && (cp<0xA0)))   return true;
  /**/                                                                                              return false; }




UC print_string_obsolete(UC *str, U4 len, UC type_known, UC type_given)
{

    buffer_t *buf;
    U4 dataSize = 0;
    UC *data;

    if(len==0)   return SUCCESS;

    buf = string_to_utf8_str(str, len, type_known, type_given);
   //  buffer_print(buf);
    data = buf->data;
    dataSize = buf->usedSize;
    while(true)
      { while((*data) == '\0') data++; //strip zeroes
        if(data>=buf->data+dataSize)   return SUCCESS;
        { if (data == buf->data)   fprintf(ostream,          "%s", data);
  	else                     fprintf(ostream, "\n       %s", data); }
        data += strlen_(data);
      }

    free(buf->data);
    free(buf);

   //printf("?want to use print_string_str?\n");
    return SUCCESS;
}
