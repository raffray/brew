#include "fstream.h"
#include "mp3File.h"
#include "extern.h"
#include "byte.h"
#include "err.h"

// <== NO== only when we need to set and keep the filesize
// <==   == which is when we open a file for reading
// <==   == the only time we open a file for writing, will be a "w+"; erase file content. no filesize needed
UC f_open0(fstream_t *fs, char *fname)
{ struct stat st;

  if(lstat(fname, &st) != 0) {printf("here 10\n"); return FAILURE;}
  fs->pFile = fopen(fname, "r+");
  if(!fs->pFile) {/*printf("here 11 ");*/ return FAILURE;} //pReturn(file, "f_open: could not open file!");
  fs->fileSize = st.st_size;
//printf("here 12 "); printU4(fs->fileSize, NL);
  rewind(fs->pFile);

  return SUCCESS;
}

UC f_open(mp3File_t *file, char *fname)
{ struct stat st;
  fstream_t *fs = &(file->fs);

  if(lstat(fname, &st) != 0) {printf("here 10\n"); return FAILURE;}
  fs->pFile = fopen(fname, "r+");
  if(!fs->pFile) {/*printf("here 11 ");*/ return FAILURE;} //pReturn(file, "f_open: could not open file!");
  set_fileSize(file, st.st_size);
//  file->fileSize = fs->fileSize;
//printf("here 12 "); printU4(fs->fileSize, NL);
  rewind(fs->pFile);

  return SUCCESS;
}

U4 get_fstreamSize(fstream_t *fs)            { return fs->fileSize;        }
VD set_fstreamSize(fstream_t *fs, U4 size)   {        fs->fileSize = size; }

UC   f_read(fstream_t *fs, void *ptr, size_t count)
{
  if(fread(ptr, 1, count, fs->pFile ) == count)                         return SUCCESS;
  if(feof (               fs->pFile ) != 0    ) { clearerr(fs->pFile ); return FAILURE; }
  /**/                                                                  return pExit(" f_read Error"); //  we would have: ferror(pFile)!=0
}
UC  f_readStr(fstream_t *fs, void *str, U4 len) { return   f_read(fs, str, len);                                  }
UC  f_readU4 (fstream_t *fs, U4 *v            ) { UC ret = f_read(fs, v  , 4  );   *v = swapU4(*v);   return ret; }
UC  f_readU2 (fstream_t *fs, U2 *v            ) { UC ret = f_read(fs, v  , 2  );   *v = swapU2(*v);   return ret; }
UC  f_readUC (fstream_t *fs, UC *v            ) { return   f_read(fs, v  , 1  );                                  }

UC f_writeStr(fstream_t *fs, void *str, U4 len) {                if(fwrite(str, 1, len, fs->pFile)!=len) pExit("in f_writeStr"); return SUCCESS; }
UC f_writeU4 (fstream_t *fs, U4  v            ) { v = swapU4(v); if(fwrite(&v , 1, 4  , fs->pFile)!=  4) pExit("in f_writeU4" ); return SUCCESS; }
UC f_writeU2 (fstream_t *fs, U2  v            ) { v = swapU2(v); if(fwrite(&v , 1, 2  , fs->pFile)!=  2) pExit("in f_writeU2" ); return SUCCESS; }
UC f_writeUC (fstream_t *fs, UC  v            ) {              ; if(fwrite(&v , 1, 1  , fs->pFile)!=  1) pExit("in f_writeUC" ); return SUCCESS; }

// serves as wrapper for the negative seeks too ( fseek(,V,); and V==-1   V is interpreted as FFFFFFFF...)
UC  f_seek(fstream_t *fs, S4 offset, int origin ) { if (fseek(fs->pFile, offset, origin) != 0) return FAILURE;   return SUCCESS; } 

U4  f_tell(fstream_t *fs)                         { return ftell(fs->pFile);                 }
//int  f_getc(fstream_t *fs)                         { return fgetc(fs->pFile);                 }

void f_close(fstream_t *fs) { if(fs->pFile!=NULL)   fclose(fs->pFile); }
