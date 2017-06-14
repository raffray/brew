#include "buffer.h"
#include "byte.h"

extern UC CPU_ENDIANNESS;

#define MIN_BUFF_SIZE 256

UC checkBufferSize(buffer_t *buf, U4 dataSize)   // dataSize is the size of data we want to write
{ U4 neededSize;

  if(dataSize> MAX32 - buffer_tell(buf))   return FAILURE;
  neededSize   =       buffer_tell(buf) + dataSize;

  buf->toSync  = true;
  if (neededSize <= buf->  size)           return SUCCESS;  // no modification
  buf->  size  =     neededSize;
  if (neededSize <= buf->bufferSize)       return SUCCESS;
  if (neededSize <= (MAX32 >> 1)   )   buf->bufferSize = neededSize<<1; 
  else                                 buf->bufferSize = MAX32; 

  buf->data = realloc(buf->data,     buf->bufferSize);

  return SUCCESS;
}

char *get_buffer_string(buffer_t *buf)   { return buf->data; }
U4    get_buffer_size  (buffer_t *buf)   { return buf->size; }
/*
VD buffer_open2(buffer_t *buf, char **data, U4 size)
{ buf->      size = size;
  buf->bufferSize = size;
  buf->      data = *data;
  buf->    cursor = 0;
  buf->    toSync = false; }
*/
VD buffer_open(buffer_t *buf, char *data, U4 size)
{ buf->      size = size;
  buf->bufferSize = size;
  buf->      data = data;
  buf->    cursor = 0;
  buf->    toSync = false; }

VD buffer_init(buffer_t *buf)
{ buf->       size =  0;
  buf-> bufferSize =  MIN_BUFF_SIZE;
  buf->       data = malloc(  MIN_BUFF_SIZE);
  memset(buf->data, 0, MIN_BUFF_SIZE);
  buf->     cursor = 0;
  buf->     toSync = false; }

VD buffer_init2(buffer_t *buf, U4 size)
{ buf->       size =  0;
  buf-> bufferSize =  size;
  buf->       data = malloc(size);
  memset(buf->data, 0, size);
  buf->     cursor = 0;
  buf->     toSync = false; }

VD buffer_swapData(buffer_t *buf, buffer_t *new)
{ buf->       size =  new->size;
  buf-> bufferSize =  new->bufferSize;
  free(buf->data);
  buf->data        = new->data;
//  buf->     cursor = 0;
//  buf->     toSync = false;
 }

void buffer_print(buffer_t *buf)
{ U4 i, j;
  U4 nbLines   = buf->bufferSize/16; //  buf->size/16;
  U4 remainder = buf->bufferSize%16; //  buf->size%16;

  for(i=0; i<nbLines    ; i++)
    { for(j=0; j<16       ; j++)
	if((j+i      *16)<buf->size)
	  printf("%02X ", *(buf->data +j+i      *16));
	  //printUC(      *(buf->data +j+i      *16), SP);
	else printf(".. ");
      printf("\n"); }
  for    (j=0; j<remainder; j++)
    if((j+nbLines*16)<buf->size)
	  printf("%02X ", *(buf->data +j+nbLines*16));
      //printUC(          *(buf->data +j+nbLines*16), SP);
    else printf(".. ");
  printf("\n\n");
} 

UC buffer_read(buffer_t *buf, void *str, U4 len)
{
  U4 avail;
  U4 newCursor = buf->cursor + len; 
  UC ret = SUCCESS;
  // we will read len characters from buf->cursor.
  // if buf->cursor + len > buf->size, we are attempting to read more characters than there are available.
  // we will read all that's available and complete the rest with zeroes
  //                                |                      |                    |
  //                                cursor                 size                 newCursor

  memset(str, 0, len);

  if( buf->cursor    > buf->size)   {                                   return FAILURE; }
  if(   newCursor    > buf->size)   { avail = buf->size - buf->cursor;  ret  = FAILURE; }
  else                                avail = len;

  if(avail>0)
    memcpy(str, buf->data + buf->cursor,  avail);

  buf->cursor = newCursor; 
  return ret;
}

UC buffer_readStr(buffer_t *buf, void *str, U4 len)   { UC ret = buffer_read(buf, str, len);                     return ret; }
UC buffer_readU4 (buffer_t *buf, U4   *v          )   { UC ret = buffer_read(buf, v  , 4  );  *v = swapU4(*v);   return ret; }
UC buffer_readU2 (buffer_t *buf, U2   *v          )   { UC ret = buffer_read(buf, v  , 2  );  *v = swapU2(*v);   return ret; }
UC buffer_readUC (buffer_t *buf, UC   *v          )   { UC ret = buffer_read(buf, v  ,   1);                     return ret; }

UC buffer_write(buffer_t *buf, void *str, U4 len)
{
  if (checkBufferSize(buf, len) == FAILURE)   return FAILURE; //pExit("buffer overflow!");
  memcpy(buf->data + buf->cursor, str, len);
  buf->cursor += len;

  return SUCCESS;
}

UC buffer_writeStr(buffer_t *buf, void *str, U4 len)   {                  return buffer_write(buf, str, len); } 
UC buffer_writeU4 (buffer_t *buf, U4 v             )   { v = swapU4(v);   return buffer_write(buf, &v ,   4); }
UC buffer_writeU2 (buffer_t *buf, U2 v             )   { v = swapU2(v);   return buffer_write(buf, &v ,   2); }
UC buffer_writeUC (buffer_t *buf, UC v             )   {                  return buffer_write(buf, &v ,   1); } 

UC buffer_seek(buffer_t *buf, S4 offset, int origin) // <== handle better
{ // seeking is always possible. Reading/wrting out of file bounds isn't!
  switch(origin)
    {
    case SEEK_SET : buf->cursor = 0;           break;
    case SEEK_END : buf->cursor = buf->size;   break;
    default       :                            break;
    }
  buf->cursor += offset;
  return SUCCESS;
}

U4 buffer_tell (buffer_t *buf)   { return buf->cursor; }

UC buffer_clean(buffer_t *buf)
{ U2 bytes;
  buffer_t newBuf;
  U4 lastNonZero;
  UC z;

  if(buf->size<2)   return SUCCESS; //nothing to be done

  buffer_init2(&newBuf, buf->size);

  buffer_seek(buf, -1, SEEK_END);

  while(buffer_tell(buf) != 0)
    {
      buffer_readUC(buf, &z);//      printU4(buffer_tell(buf)-1, SP);  printf(" --> "); printUC(z,NL);
      if(z!=0x00)   break;//      printf("did not break\n");
      buffer_seek(buf, -2, SEEK_CUR);
    }

  if(buffer_tell(buf) == 0) // the string is made up of only zeroes
    {//      buffer_writeUC(&newBuf, 0x00); //      printf("Null string\n");
    }
  else
    {
      lastNonZero = buffer_tell(buf)-1;
      buffer_seek(buf, lastNonZero, SEEK_SET);
      buffer_readUC(buf, &z); //      printU4(lastNonZero, SP);  printf(" ---> "); printUC(z, NL);       // exit(4);
      
      
      buffer_seek(buf, 0, SEEK_SET);
      
      while(buffer_tell(buf)<=lastNonZero)
	{ buffer_readU2(buf, &bytes);//	  printf("++++++++++++++++++++++++++++ "); printU2(bytes,NL);
	  // replace 0000 with 00
	  if     (bytes == 0x0000)   { /*buffer_writeUC(&newBuf,           0x00); */  buffer_seek(buf, -1, SEEK_CUR); }
	  else		         { buffer_writeUC(&newBuf, (UC)(bytes>>8));   buffer_seek(buf, -1, SEEK_CUR);  } 
	}//      buffer_writeUC(&newBuf, 0x00);
    } 

  buffer_writeUC(&newBuf, 0x00);
  buffer_swapData(buf, &newBuf);//  printU4(buf->size, NL);   buffer_print(buf);

  return SUCCESS;
}

UC buffer_changeSize(buffer_t *buf, U4 newSize) // States that we only consider the first "newSize" bytes of the buffer
{ if (newSize > buf->bufferSize)   return FAILURE;
  buf->size = newSize;             return SUCCESS; }

UC buffer_close(buffer_t *buf)
{
  buffer_seek(buf,0,SEEK_SET);

  if(buf->toSync == true)  { buffer_seek(buf, 0, SEEK_END);  buffer_writeUC(buf, 0); buffer_seek(buf,-1,SEEK_CUR);  buf->size--; }

  return SUCCESS;
}
