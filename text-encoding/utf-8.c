#include "utf-8.h"
#include "err.h"
//#include "str.h"

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
