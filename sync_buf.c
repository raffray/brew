/*
  function for re-synchronising buffers
  Pulled out of buffer.c because it is not generic enough. It uses a buffer for a particular case
*/

#include "sync_buf.h"
#include "buffer.h"
#include "byte.h"
#include "base_types.h"

UC resyncBuf2(buffer_t *buf)
{ U4 bytes;
  buffer_t newBuf;

  buffer_init2(&newBuf, buf->size);  // if no byte-sequence is found for resync, the new data will be identical. Otherwise, it will be shortened
//  printU4(buf->size, NL);   buffer_print(buf);
  buffer_seek(buf, 0, SEEK_SET);

  while(buffer_tell(buf)<buf->size)
    { buffer_readU4(buf, &bytes);
      // replace FF0000 with FF00
      if     ((bytes&0xFFFFFF00) == 0xFF000000)   { buffer_writeU2(&newBuf, 0xFF00);   buffer_seek(buf, -1, SEEK_CUR); }
      // replace %11111111 00000000 111xxxxx with %11111111 111xxxxx
      else if((bytes&0xFFFFE000) == 0xFF00E000)   { buffer_writeUC(&newBuf, 0xFF);  
	                                            buffer_writeUC(&newBuf, (UC)(bytes>> 8));   buffer_seek(buf, -1, SEEK_CUR);  }
      else		                          { buffer_writeUC(&newBuf, (UC)(bytes>>24));   buffer_seek(buf, -3, SEEK_CUR);  } }

  buffer_swapData(buf, &newBuf);
//  printU4(buf->size, NL);   buffer_print(buf);

  return SUCCESS;
}

UC resyncBuf(char *str, U4 *size) // <=== move to different file
{
  buffer_t buf;
  U4 finalSize = *size;
  U4  readPos = 0;
  U4 writePos = 0;
  U4 bytes;

  buffer_open(&buf, str, *size);
  while(readPos<*size)
    { buffer_seek(&buf, readPos, SEEK_SET);
      buffer_readU4(&buf, &bytes);   readPos+=4;
      // replace FF0000 with FF00
      if     ((bytes&0xFFFFFF00) == 0xFF000000)   { buffer_seek(&buf, writePos, SEEK_SET);   buffer_writeU2(&buf, 0xFF00);            writePos+=2;   readPos--;   finalSize--; }
      // replace %11111111 00000000 111xxxxx with %11111111 111xxxxx
      else if((bytes&0xFFFFE000) == 0xFF00E000)   { buffer_seek(&buf, writePos, SEEK_SET);   buffer_writeUC(&buf, 0xFF);  
	                                                                                     buffer_writeUC(&buf, (UC)(bytes>> 8));   writePos+=2;   readPos--;   finalSize--; }
      else		                          { buffer_seek(&buf, writePos, SEEK_SET);   buffer_writeUC(&buf, (UC)(bytes>>24));   writePos++;    readPos-=3;               } }
  *size = finalSize;
  buffer_close(&buf);

  return SUCCESS;
}

