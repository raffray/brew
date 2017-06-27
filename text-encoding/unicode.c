#include "unicode.h"

#include "utf-8.h"
#include "utf-16.h"
#include "iso-8859-1.h"
#include "ascii.h"

#include "err.h"
#include "str.h"



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

  buffer_t *buffout = malloc(sizeof(buffer_t));
  U4 codepoint;
  UC type;
  UC enc;

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

  switch(type)
    {
    case 1:   if(get_UTF16_enc(buffin,            &enc) == SUCCESS) // <== FAILURE would mean we reached the end of the buffer
	              while(     nextUTF16    (buffin, &codepoint, enc) == SUCCESS)
	                { if (codepoint == 0) // multi-strings
	                    { if(get_UTF16_enc(buffin,            &enc) == SUCCESS) // BOM at start of each new string
		                      write_codepoint_to_utf8(buffout,   0); }
	                  else
	                    write_codepoint_to_utf8(buffout, codepoint);
	                }
              break;
    case 2:   while(nextUTF16(buffin, &codepoint, B_ENDIAN) == SUCCESS)  write_codepoint_to_utf8(buffout, codepoint); break;
    case 0:   while(nextIso  (buffin, &codepoint          ) == SUCCESS)  write_codepoint_to_utf8(buffout, codepoint); break;
    case 3:   while(nextUTF8 (buffin, &codepoint          ) == SUCCESS)  write_codepoint_to_utf8(buffout, codepoint); break;
    case 0xFF:while(nextAscii(buffin, &codepoint          ) == SUCCESS)  write_codepoint_to_utf8(buffout, codepoint); break;
    default: break;
    }

  buffer_writeUC(buffout, 0x00);

  return buffout;
}


UC is_nonPrintable(U4 cp)
{ if(((cp>0) && (cp<0x05)) || (cp==0x06) || ((cp>0x0F) && (cp<0x20)) || ((cp>0x7E) && (cp<0xA0)))   return true;
  /**/                                                                                              return false; }
UC write_codepoint_to_utf8_1byte(buffer_t *buf, U4 cp)   { buffer_writeUC(buf, cp);                                                                                                   return SUCCESS; }
UC write_codepoint_to_utf8_2byte(buffer_t *buf, U4 cp)   {                                                buffer_writeU2(buf, ((cp<<2)&    0x1F00) + (cp&    0x003F) +     0xC080);   return SUCCESS; }
UC write_codepoint_to_utf8_3byte(buffer_t *buf, U4 cp)   { buffer_writeUC(buf, ((cp>>12)&0x0F) + 0xE0);   buffer_writeU2(buf, ((cp<<2)&    0x3F00) + (cp&    0x003F) +     0x8080);   return SUCCESS; }
UC write_codepoint_to_utf8_4byte(buffer_t *buf, U4 cp)   { buffer_writeU4(buf, ((cp<< 6)&0x07000000) + ((cp<<4)&0x003F0000) + ((cp<<2)&0x00003F00) + (cp&0x0000003F) + 0xF0808080);   return SUCCESS; }
UC write_codepoint_to_utf8   (buffer_t *buf, U4 cp)
{ if (is_nonPrintable(cp))  return write_codepoint_to_utf8_3byte(buf, ERR_CHAR);
  if(cp<0x80    )           return  write_codepoint_to_utf8_1byte(buf,       cp);
  if(cp<0x800   )           return  write_codepoint_to_utf8_2byte(buf,       cp);
  if(cp<0x10000 )           return  write_codepoint_to_utf8_3byte(buf,       cp);
  if(cp<0x110000)           return  write_codepoint_to_utf8_4byte(buf,       cp);
  return SUCCESS; // Unreachable point
}
