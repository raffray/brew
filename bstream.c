#include <inttypes.h>
#include "extern.h"

#include "bstream.h"
#include "mp3File.h"
#include "byte.h"

#include "str.h"
#include "err.h"

// either we f-open an existing file, read the part we want into a buffer and fclose it.
// or, we create an empty buffer, fill it as we please, and fwrite it all at closing
//
// f-open in "r" mode, read whole file in buffer, fclose
// if a write is done, set SYNC flag and rewrite file at closing (f-open "w+"...)
// if we write beyond bounds, use realloc with a size of 2 * position_at_end_of_write
// or FFFF FFFF and if we reach that position, throw an error
//
// limit#1 filesize
// limit#2 buffersize >=filesize   we realloc a bigger buffer when an attempt to write beyond "eof" is made
// the size of the file is only that of the the last byte written
//

// 0
//
// filesize
//
// buffersize
//
// MAX32 aka 7FFF FFFF

// Max file size on 32-bit is 2GB. So, fseek +/- 2GB, value range for a signed 32bit value, is fine
// Check, everywhere in program, for pure 32-bit use or if we can use 64-bit file functions
// on 32-bit systems.
// or limit the program to 2GB files even on 64-bit systems.
// Might even want to set the limit (MAX32) to 250MB for now (who has regular mp3 files larger than that?)

// every offset we read while parsing file has to be >0   if read as   signed
//                                                   <2GB if read as unsigned, which is equivalent


UC b_open(mp3File_t *file, char *filename)
{ FILE *pFile;
  struct stat st;
  bstream_t *bs  = &(file->bs);
  buffer_t  *buf = &(bs ->buf);
  UC *data = NULL;
  //  memset(bs, 0, sizeof(bstream_t));
  bs->toSync = false;
  if ( lstat(filename, &st) == 0)
    { pFile = fopen(filename, "r");
      if(!pFile)    return FAILURE; //pReturn(file, "fopen: could not open file!");
      rewind(pFile); // <=== useful?

      data = malloc(st.st_size);
      if ( fread(data, 1, st.st_size, pFile) != st.st_size)   return FAILURE; // pReturn(file, "b_open: could not load file");
      fclose(pFile);

      set_fileSize(file    , st.st_size);     // <== might want to limit filesize HERE
      buffer_open(buf, data, st.st_size); }
  else if(errno == ENOENT) // file needs to be created
    { buffer_init_default(buf);
      bs->toSync = true; }
  else   { fprintf(ostream, "b_open: could not stat file");   return FAILURE; }

//  scopy(&(bs->filename), filename); // why make a copy?

  return SUCCESS;
}

U4 get_bstreamSize(bstream_t *bs)            { return (&(bs->buf))->usedSize;        }
VD set_bstreamSize(bstream_t *bs, U4 size)   {        (&(bs->buf))->usedSize = size; }

UC  b_readStr(bstream_t *bs, void *str, U4 len)   { return buffer_readStr(&(bs ->buf), str, len); }
UC  b_readU4 (bstream_t *bs, U4   *v          )   { return buffer_readU4 (&(bs ->buf), v       ); }
UC  b_readU2 (bstream_t *bs, U2   *v          )   { return buffer_readU2 (&(bs ->buf), v       ); }
UC  b_readUC (bstream_t *bs, UC   *v          )   { return buffer_readUC (&(bs ->buf), v       ); }

UC b_writeStr(bstream_t *bs, void *str, U4 len)   { return buffer_writeStr(&(bs ->buf),  str, len); }
UC b_writeU4 (bstream_t *bs, U4 v             )   { return buffer_writeU4 (&(bs ->buf),  v       ); }
UC b_writeU2 (bstream_t *bs, U2 v             )   { return buffer_writeU2 (&(bs ->buf),  v       ); }
UC b_writeUC (bstream_t *bs, UC v             )   { return buffer_writeUC (&(bs ->buf),  v       ); }

UC b_seek(bstream_t *bs, S4 offset, int origin)   { return buffer_seek(&(bs ->buf), offset, origin); }
U4 b_tell(bstream_t *bs                       )   { return buffer_tell(&(bs ->buf)                ); }

UC b_close(bstream_t *bs, char *filename)
{ FILE *pFile;
  buffer_t  *buf = &(bs ->buf);

  if(bs == NULL)   return pExit("b_close failed, called on a NULL pointer");
  if((buf->toSync) || (bs->toSync))
    {    pFile = fopen(filename, "w+");
      if(pFile == NULL)                                         return pExit("b_close: could not sync file");

      if(fwrite(buf->data, 1, buf->usedSize, pFile) != buf->usedSize)   return pExit("b_close: could not sync file");
      fclose(pFile); }

//  free(bs->filename);
  free(buf->data);

  return SUCCESS;
}
