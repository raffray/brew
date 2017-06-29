#include "buffer.h"
#include "byte.h"
#include "extern.h"

#define MIN_BUFF_SIZE 128//2048

UC buffer_expand(buffer_t *buf, U4 dataSize)   // dataSize is the size of data we want to write
{ U4 neededSize;
  U4 enteringDataSize = buf->dataSize;

 if(dataSize> MAX32 - buffer_tell(buf))   return FAILURE;
  neededSize   =       buffer_tell(buf) + dataSize;

  buf->toSync  = true;
  if (neededSize <= buf->usedSize)        return SUCCESS;  // no modification
  buf-> usedSize  =    neededSize;
  if (neededSize <= buf->dataSize)        return SUCCESS;
  if (neededSize <= (MAX32 >> 1)   )   buf->dataSize = neededSize<<1;
  else                                 buf->dataSize = MAX32;

  //  printf("Resizing buffer; %d ---> %d\n", enteringDataSize, buf->dataSize);
  buf->data = realloc(buf->data,     buf->dataSize);
  memset(buf->data+enteringDataSize, 0, buf->dataSize-enteringDataSize); // initialize buffer's extended data

  return SUCCESS;
}

//char *buffer_get_data(buffer_t *buf)   { return buf->data; }
//U4    buffer_get_size  (buffer_t *buf)   { return buf->usedSize; }

/*
VD buffer_open2(buffer_t *buf, char **data, U4 size)
{ buf->      usedSize = size;
  buf->dataSize = size;
  buf->      data = *data;
  buf->    cursor = 0;
  buf->    toSync = false; }
*/
VD buffer_open(buffer_t *buf, UC *data, U4 size)
{ buf->usedSize = size;
  buf->dataSize = size;
  buf->    data = data;
  buf->  cursor = 0;
  buf->  toSync = false; }

VD buffer_init_default(buffer_t *buf)   { buffer_init_setSize(buf, MIN_BUFF_SIZE); }

VD buffer_init_setSize(buffer_t *buf, U4 size)
{ buf-> usedSize =  0;
  buf-> dataSize =  size;
  buf->       data = malloc(size);
  memset(buf->data, 0, size);
  buf->     cursor = 0;
  buf->     toSync = false; }

VD buffer_swapData(buffer_t *buf, buffer_t *new)
{ buf-> usedSize =  new->usedSize;
  buf-> dataSize =  new->dataSize;
  free(buf->data);
  buf->data        = new->data;
//  buf->     cursor = 0;
//  buf->     toSync = false;
 }

void buffer_print(buffer_t *buf)
{ U4 i, j;
  U4 nbLines   = buf->dataSize/16; //  buf->size/16;
  U4 remainder = buf->dataSize%16; //  buf->size%16;

  for(i=0; i<nbLines    ; i++)
    { for(j=0; j<16       ; j++)
	      if((j+i      *16)<buf->usedSize)
	        printUC(      *(buf->data +j+i      *16), SP);
	      else fprintf(ostream, ".. ");
          fprintf(ostream, "\n"); }
      for    (j=0; j<remainder; j++)
        if((j+nbLines*16)<buf->usedSize)
          printUC(          *(buf->data +j+nbLines*16), SP);
        else fprintf(ostream, ".. ");
      fprintf(ostream, "\n\n");
   }

UC buffer_read(buffer_t *buf, void *str, U4 len)
{
  U4 avail;
  U4 newCursor = buf->cursor + len;
  UC ret = SUCCESS;
  // we will read len bytes from buf->cursor.
  // if buf->cursor + len > buf->size, we are attempting to read more bytes than there are available.
  // we will read all that's available and complete the rest with zeroes
  //                                |                      |                    |
  //                                cursor                 usedSize             newCursor

  memset(str, 0, len);

  if( buf->cursor    > buf->usedSize)   {                                   return FAILURE; }
  if(   newCursor    > buf->usedSize)   { avail = buf->usedSize - buf->cursor;  ret  = FAILURE; }
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
  if (buffer_expand(buf, len) == FAILURE)   { /* printf("Beep\n"); */ return FAILURE;} //pExit("buffer overflow!");
  memcpy(buf->data + buf->cursor, str, len);
  buf->cursor += len;

  return SUCCESS;
}

UC buffer_writeStr(buffer_t *buf, void *str, U4 len)   {                  return buffer_write(buf, str, len); }
UC buffer_writeU4 (buffer_t *buf, U4 v             )   { v = swapU4(v);   return buffer_write(buf, &v ,   4); }
UC buffer_writeU2 (buffer_t *buf, U2 v             )   { v = swapU2(v);   return buffer_write(buf, &v ,   2); }
UC buffer_writeUC (buffer_t *buf, UC v             )   {                  return buffer_write(buf, &v ,   1); }
/*{
  if (checkBufferSize(buf, 1) == FAILURE)   {printf("Beep\n"); return FAILURE;} //pExit("buffer overflow!");
  *(buf->data + buf->cursor) = v; //memcpy(buf->data + buf->cursor, &v, 1);
  buf->cursor += 1;

  return SUCCESS;
  }
*/
UC buffer_seek(buffer_t *buf, S4 offset, int origin) // <== handle better
{ // seeking is always possible. Reading/wrting out of file bounds isn't!
  switch(origin)
    {
    case SEEK_SET : buf->cursor = 0;           break;
    case SEEK_END : buf->cursor = buf->usedSize;   break;
    default       :                            break;
    }
  buf->cursor += offset;
  return SUCCESS;
}

U4 buffer_tell (buffer_t *buf)   { return buf->cursor; }


// do not know when we would need this
// it seems to basically cleanup the trailing zeroes from the buffer
// this function has no place here anyway as it is not generic enough for buffers
/*
UC buffer_clean(buffer_t *buf)
{ U2 bytes;
  buffer_t newBuf;
  U4 lastNonZero;
  UC z;

  if(buf->usedSize<2)   return SUCCESS; //nothing to be done

  buffer_init_setSize(&newBuf, buf->usedSize);

  buffer_seek(buf, -1, SEEK_END);

  while(buffer_tell(buf) > 0) // was "!="
    { // seeks the last non-zero byte;  we'll be positionned right after
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
      buffer_seek(buf, lastNonZero, SEEK_SET); // why not: buffer_seek(buf, -1, SEEK_CUR);
      buffer_readUC(buf, &z); //      printU4(lastNonZero, SP);  printf(" ---> "); printUC(z, NL);       // exit(4);


      buffer_seek(buf, 0, SEEK_SET);

      while(buffer_tell(buf)<=lastNonZero)
	{ buffer_readU2(buf, &bytes);//	  printf("++++++++++++++++++++++++++++ "); printU2(bytes,NL);
	  // replace 0000 with 00
	  if     (bytes == 0x0000)   { //buffer_writeUC(&newBuf,           0x00);
      buffer_seek(buf, -1, SEEK_CUR); }
	  else		         { buffer_writeUC(&newBuf, (UC)(bytes>>8));   buffer_seek(buf, -1, SEEK_CUR);  }
	}//      buffer_writeUC(&newBuf, 0x00);
    }

  buffer_writeUC(&newBuf, 0x00);
  buffer_swapData(buf, &newBuf);//  printU4(buf->usedSize, NL);   buffer_print(buf);

  return SUCCESS;
}
*/
/*
UC buffer_changeSize(buffer_t *buf, U4 newSize) // States that we only consider the first "newSize" bytes of the buffer
{ if (newSize > buf->dataSize)   return FAILURE;
  buf->usedSize = newSize;             return SUCCESS; }
*/
UC buffer_close(buffer_t *buf)
{ // write a zero at end of buffer; used for strings????
  buffer_seek(buf,0,SEEK_SET);

  if(buf->toSync == true)
    { buffer_seek(buf, 0, SEEK_END);  buffer_writeUC(buf, 0); buffer_seek(buf,-1,SEEK_CUR);  buf->usedSize--; }

  return SUCCESS;
}

UC buffer_free(buffer_t *buf)
{
  free(buf->data);
  return SUCCESS;
}
