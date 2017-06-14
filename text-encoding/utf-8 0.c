#include "buffer.h"
#include "extern.h"
    /* UTF8

Bits of     First       Last        Bytes in
code point  code point  code point  sequence 	Byte 1 	        Byte 2 	        Byte 3 	        Byte 4 	        Byte 5 	        Byte 6
 7 	    U+0000 	U+007F 	        1 	0xxxxxxx
11 	    U+0080 	U+07FF         	2 	110xxxxx 	10xxxxxx
16 	    U+0800 	U+FFFF 	        3 	1110xxxx 	10xxxxxx 	10xxxxxx
21 	    U+10000 	U+1FFFFF 	4 	11110xxx 	10xxxxxx 	10xxxxxx 	10xxxxxx
26 	    U+200000 	U+3FFFFFF 	5 	111110xx 	10xxxxxx 	10xxxxxx 	10xxxxxx 	10xxxxxx
31 	    U+4000000 	U+7FFFFFFF 	6 	1111110x 	10xxxxxx 	10xxxxxx 	10xxxxxx 	10xxxxxx 	10xxxxxx

We got from each byte wether we need to read more bytes
then we convert as, for example   11110aaa 10bbbbbb 10cccccc 10dddddd
                                  00000000 000aaabb bbbbcccc ccdddddd  = UNICODE value
                                              11111 11111111 11111111 = 1F FF FF
                                              10011 11111111 11111111 = 1F FF FF

   110aaaaa 10bbbbbb
    
From  0000 0000
To    0111 1111 --> 1-byte ascii
From  1000 0000
To    1011 1111 --> continuation byte
Equal 1100 0000 = C0
         0 0000   10bb bbbb
             00     bb bbbb
                  00bb bbbb  <--- should be encoded as 1-byte 0xxxxxxx
Equal 1100 0001 = C0
         0 0001   10bb bbbb
             01     bb bbbb
                  01bb bbbb  <--- should be encoded as 1-byte 0xxxxxxx
From  1100 0010
To    1101 1111 --> 2-byte encoding
From  1110 0000
To    1110 1111 --> 3-byte encoding
From  1111 0000
To    1111 0100 --> 4-byte encoding

From  1111 0101 --> code-point > 0x10FFFF, limit of Unicode

    */

/*
We need to parse through the byte string
every n-byte sequence is converted to it's hexa code-point

ERRORS
*    the red invalid bytes in the above table
*    an unexpected continuation byte
*    a start byte not followed by enough continuation bytes
*   an Overlong Encoding as described above
*    A 4-byte sequence (starting with 0xF4) that decodes to a value greater than U+10FFFF

if we meet an error... 
                      Translate the first byte by the replacement character ("�" (U+FFFD)) and continue parsing with the next byte. 

 */

//    D    F      B    F
// 1101 1111   1011 1111
// ...1 1111   ..11 1111
//       111   1111 1111
//         7      F    F

//    E    0      A    0       8    0
// 1110 0000   1010 0000    1000 0000
// .... 0000   ..10 0000    ..00 0000
//             0000 1000    0000 0000
//                     8       0    0

//    E    0      8    0       8    0
// 1110 0000   1000 0000    1000 0000
// .... 0000   ..00 0000    ..00 0000
//             0000 0000    0000 0000
//                     0       0    0
//
// which should be encoded in 2 bytes.

//    E    0      8    0       A    F
// 1110 0000   1000 0000    1010 1111
// .... 0000   ..00 0000    ..10 1111
//             0000 0000    0010 1111
//                     0       2    F
//

//    F    0      8    F       B    F       B    F
// 1110 0000   1000 1111    1011 1111    1011 1111
// .... 0000   ..00 1111    ..11 1111    ..11 1111
//                          1111 1111    1111 1111
//                             F    F       F    F
//

//    C    2      A    2
// 1100 0010   1010 0010
// ...0 0010   ..10 0010
//       000   1010 0010
//         0      A    2

//    C    E      B    A
// 1100 1110   1011 1010
// ...0 1110   ..11 1010
//       011   1011 1010
//         3      B    A

UC nextUTF8(buffer_t *buf, wchar_t *w)
{
  UC byte1, byte2, byte3, byte4;
  wchar_t err = 0xFFFD;
  wchar_t res;
// U2 c;

//wprintf(L"\n");

// Some control characters should be allowed:
  if(buffer_readUC(buf, &byte1) == FAILURE)   return FAILURE;
//  if( byte1 == 0x09)                          { *w = byte1;   return SUCCESS; } // horizontal Tab  aka '\t'
//  if( byte1 == 0x0A)                          { *w = byte1;   return SUCCESS; } // Line Feed       aka '\n'
// if( byte1 == 0x0B)                          { *w = byte1;   return SUCCESS; } // Vertical   Tab  aka '\t'
//  if( byte1 == 0x0D)                          { *w = byte1;   return SUCCESS; } // Carriage Return aka '\r'

//  if( byte1 == 0)                             { *w = err;     return SUCCESS; } // control character
//  if( byte1 <  0x20)                          { *w = err;     return SUCCESS; } // control character
  if( byte1 <  0x80)                          { *w = byte1;   return SUCCESS; }
  if( byte1 <  0xC2)                          { *w = err;     return SUCCESS; } // overlong encoding
//wprintf(L"- 1 -> ");
  if(byte1 <   0xE0) // 1110 0000
    { // 2-byte encoding
      if(buffer_readUC(buf, &byte2) == FAILURE)   return FAILURE;
      if((byte2 & 0xC0) != 0x80) // 1000 0000, continuation byte
	{ *w = err;   buffer_seek(buf, -1, SEEK_CUR);   return SUCCESS; }

//  Bits of     First       Last        Bytes in
//  code point  code point  code point  sequence 	Byte 1 	        Byte 2
//  11          U+0080      U+07FF      2               110xxxxx        10xxxxxx

      res = (byte1 & 0x1F)<<6 | (byte2 & 0x3F);   // wprintf(L"%02X %02X\n", (UC)(res>> 8), (UC)res);  
      //   if(res<0x80) { *w = err;     return SUCCESS; } // overlong encoding   // already handled here by cases byte1 == C1 or C2
      if(res<0x80)   { *w = err;  buffer_seek(buf, -1, SEEK_CUR);   return SUCCESS; } // control character
      *w = res;
      return SUCCESS;
    }

//wprintf(L"- 2 -> ");
  if(byte1 <   0xF0) // 1111 0000
    { // 3-byte encoding
      if(buffer_readUC(buf, &byte2) == FAILURE)   return FAILURE;
      if((byte2 & 0xC0) != 0x80)   { *w = err;   buffer_seek(buf, -1, SEEK_CUR);   return SUCCESS; }
      if(buffer_readUC(buf, &byte3) == FAILURE)   return FAILURE;
      if((byte3 & 0xC0) != 0x80)   { *w = err;   buffer_seek(buf, -2, SEEK_CUR);   return SUCCESS; }

//  Bits of     First       Last        Bytes in
//  code point  code point  code point  sequence 	Byte 1 	        Byte 2 	        Byte 3
//  16          U+0800      U+FFFF      3               1110xxxx        10xxxxxx        10xxxxxx

      res = (byte1 & 0x0F)<<12 | (byte2 & 0x3F)<<6 | (byte3 & 0x3F);   // wprintf(L"%02X %02X\n", (UC)(res>> 8), (UC)res);  
      if(res<0x800)   { *w = err;  buffer_seek(buf, -2, SEEK_CUR);   return SUCCESS; } // overlong encoding
      *w = res;
      return SUCCESS;
    }

//wprintf(L"- 3 -> ");
  if(byte1 <   0xF5) // 1111 0000
    { // 4-byte encoding
      if(buffer_readUC(buf, &byte2) == FAILURE)   return FAILURE;
      if((byte2 & 0xC0) != 0x80)   { *w = err;   buffer_seek(buf, -1, SEEK_CUR);   return SUCCESS; }
      if(buffer_readUC(buf, &byte3) == FAILURE)   return FAILURE;
      if((byte3 & 0xC0) != 0x80)   { *w = err;   buffer_seek(buf, -2, SEEK_CUR);   return SUCCESS; }
      if(buffer_readUC(buf, &byte4) == FAILURE)   return FAILURE;
      if((byte4 & 0xC0) != 0x80)   { *w = err;   buffer_seek(buf, -3, SEEK_CUR);   return SUCCESS; }

//  Bits of     First       Last        Bytes in
//  code point  code point  code point  sequence 	Byte 1 	        Byte 2 	        Byte 3 	        Byte 4
//  21          U+10000     U+1FFFFF 	4               11110xxx 	10xxxxxx 	10xxxxxx 	10xxxxxx

      res = (byte1 & 0x0F)<<18 | (byte2 & 0x3F)<<12 | (byte3 & 0x3F)<<6 | (byte4 & 0x3F);
//    wprintf(L"%02X %02X %02X %02X\n", (UC)(res>> 24), (UC)(res>> 16), (UC)(res>> 8), (UC)res);  
      if(res<0x10000)   { *w = err;  buffer_seek(buf, -3, SEEK_CUR);   return SUCCESS; } // overlong encoding
      *w = res;
      return SUCCESS;
    }

//wprintf(L"- 4 -> ");
  *w = err;
  return SUCCESS;
}
