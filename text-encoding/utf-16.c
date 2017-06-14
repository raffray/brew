#include "utf-16.h"
#include "byte.h"
#include "extern.h"

// http://en.wikipedia.org/wiki/UTF-16
// http://www.utf8-chartable.de/unicode-utf8-table.pl

    /* UTF16

*** U+0000 to U+D7FF and U+E000 to U+FFFF

The first plane (code points U+0000 to U+FFFF) contains the most frequently used characters and is called the Basic Multilingual Plane or BMP. 
Code points in this range are numerically equal to the corresponding code points.
Within this plane, code points U+D800 to U+DFFF (see below) are reserved for lead and trail surrogates.


*** U+010000 to U+10FFFF

Code points from the other planes (called Supplementary Planes) are encoded in UTF-16 by pairs of 16-bit code units called surrogate pairs, by the following scheme:

Lead \ Trail 	DC00 	DC01 	…    	DFFF
D800 	        010000 	010001 	… 	0103FF
D801 	        010400 	010401 	… 	0107FF
  ⋮ 	        ⋮ 	⋮ 	⋱ 	⋮
DBFF 	        10FC00 	10FC01 	… 	10FFFF

    0x010000 is subtracted from the code point, leaving a 20 bit number in the range 0..0x0FFFFF.
    ** The top ten bits (a number in the range 0..0x03FF) are added to 0xD800 to give the first code unit or lead/high surrogate, 
       which will be in the range 0xD800..0xDBFF.
    ** The low ten bits (         in the range 0..0x03FF) are added to 0xDC00 to give the second code unit or trail/low surrogate, 
       which will be in the range 0xDC00..0xDFFF.

Since the ranges for the lead surrogates, trail surrogates, and valid BMP characters are disjoint, searches are simplified: 
it is not possible for part of one character to match a different part of another character. It also means that UTF-16 is 
self-synchronizing on 16-bit words: whether a code unit starts a character can be determined without examining earlier code units. 
UTF-8 shares these advantages, but many earlier multi-byte encoding schemes did not allow unambiguous searching and could only be 
synchronized by re-parsing from the start of the string. UTF-16 is not self-synchronizing if one byte is lost or if traversal starts at a random byte.

Because the most commonly used characters are all in the Basic Multilingual Plane, handling of surrogate pairs is often not thoroughly tested. 
This leads to persistent bugs and potential security holes, even in popular and well-reviewed application software (e.g. CVE-2008-2938, CVE-2012-2135).[3]


**** U+D800 to U+DFFF

The Unicode standard permanently reserves these code point values for UTF-16 encoding of the lead and trail surrogates, and they will never be 
assigned a character, so there should be no reason to encode them. The official Unicode standard says that no UTF forms, including UTF-16, can 
encode these code points.

However UCS-2, UTF-8, and UTF-32 can encode these code points in trivial and obvious ways, and large amounts of software does so even though 
the standard states that such arrangements should be treated as encoding errors. It is possible to unambiguously encode them in UTF-16 by using 
a code unit equal to the code point, as long as no sequence of two code units can be interpreted as a legal surrogate pair (that is, as long as 
a lead surrogate is never followed by a trail surrogate). The majority of UTF-16 encoder and decoder implementations translate between encodings
as though this were the case.
    */

UC get_UTF16_enc(buffer_t *buf, UC *enc)
{ U2 val;

  if(buffer_readU2(buf, &val) == FAILURE)   return FAILURE;

  if(val == 0xFFFE)   { *enc = L_ENDIAN;   return SUCCESS; }
  if(val == 0xFEFF)   { *enc = B_ENDIAN;   return SUCCESS; }

  //if no BOM, we default to Big Endian encoding.
  buffer_seek(buf, -2, SEEK_CUR);
  /**/                { *enc = B_ENDIAN;   return SUCCESS; }
}

UC nextUTF16(buffer_t *buf, U4 *w, UC enc) // gets the codepoint value corresponding to the utf-16 character (either 2 or 4 bytes)
{ U2 val;                                  // if not enough bytes available, char will be discarded
  U2 low;
  // *** Code points U+0000 to U+D7FF  // U+D800 to U+DBFF // U+DC00 to U+DFFF // U+E000 to U+FFFF
  //                 regular              lead                trail               regular

  if(buffer_readU2(buf, &val) == FAILURE)                return FAILURE; // <== TRY macro
  if (enc == L_ENDIAN)   val = swapU2(val);

  // ***             U+0000 to U+D7FF and U+E000 to U+FFFF
  if(((val>0) && (val<0x05)) || (val==0x06) || ((val>0x0F) && (val<0x20)) || ((val>0x7E) && (val<0xA0)))  // <=== ????
    /**/                              { *w = ERR_CHAR;   return SUCCESS; }
  if ((val<0xD800) || (val>0xDFFF))   { *w = val;        return SUCCESS; }

  // ***   lead in:  U+D800 -- U+DBFF    
  if ((val<0xD800) || (val>0xDBFF))   { *w = ERR_CHAR;   return SUCCESS; } // lead surrogate expected but not found

  if (buffer_readU2(buf, &low) == FAILURE)               return FAILURE;   // lead surrogate found, but no data available after; char discarded
  if (enc == L_ENDIAN)   low = swapU2(low);
  // ***  trail in:  U+DC00 -- U+DFFF
  if ((low<0xDC00) || (low>0xDFFF))   { *w = ERR_CHAR;   buffer_seek(buf, -1, SEEK_CUR);   return SUCCESS; }  // trail surrogate expected but not found

  *w = (low - 0xDC00) + ((val - 0xD800) << 10) + 0x10000;
  return SUCCESS;
}



// my print : ﺗﺪﺭﯾﺲ ﻭ ﯾﺎﺩﮔﯿﺮﯼ ﺯﺑﺎﻥ
// original : ﺗﺪﺭﯾﺲ ﻭ ﯾﺎﺩﮔﯿﺮﯼ ﺯﺑﺎﻥ  <--- except it displays in reverse on webpage, because of the RTL (Right To Left "encoding")
//
// Text displays ok within pluma


/* UTF 16

v  = 0x10300
v′ = v - 0x10000
   = 0x00300
   = 0000 0000 0011 0000 0000
vh = v′ >> 10
   = 00 0000 0000 // higher 10 bits of v′
vl = v′ & 0x3FF
   = 11 0000 0000 // lower  10 bits of v′
w1 = 0xD800 + vh
   = 1101 1000 0000 0000
   +        00 0000 0000
   = 1101 1000 0000 0000
   = 0xD800 // first code unit of UTF-16 encoding
w2 = 0xDC00 + vl
   = 1101 1100 0000 0000
   +        11 0000 0000
   = 1101 1111 0000 0000
   = 0xDF00 // second code unit of UTF-16 encoding

 */

/*
   
   0001 0000 1111 1111  1111 1111
-  0000 0001 0000 0000  0000 0000
=  0000 1111 1111 1111  1111 1111
   11 1111 1111     11  1111 1111
>> and &
   0000 0011  1111 1111            0000 0011  1111 1111
                                 & 0000 0011  1111 1111
=                                  0000 0011  1111 1111
+  1101 1000  0000 0000          + 1101 1100  0000 0000
=  1101 1011  1111 1111            1101 1111  1111 1111
   D    B     F    F               D    F     F    F
 */

/*
vh = w1 - 0xD800
vl = w2 - 0xDC00 

vl = 0xDF00 - 0xDC00
   =   1101 1111  0000 0000
     - 1101 1100  0000 0000
   =   0000 0011  0000 0000
       .... ..11  0000 0000

vh = 0xD800 - 0xD800
   =   1101 1000  0000 0000
     - 1101 1000  0000 0000
   =   0000 0000  0000 0000
       .... ..00  0000 0000

vl + (vh << 10)
=  0000  0000 0011  0000 0000
+  0001  0000 0000  0000 0000
=  0001  0000 0011  0000 0000
   1     0    3     0    0 



*/
