#include "includes.h"
#include "byte.h"
#include "extern.h"
#include "err.h"
#include "mp3File.h"
#include "fstream.h"
#include "bstream.h"

#include "fileList.h"

UC  mreadStr(mp3File_t *file, char *str,      U4 len)   { if (file->streamType == FSTREAM)   return f_readStr(&(file->fs), str,  len);     else   return b_readStr(&(file->bs), str, len);    }
UC  mreadU4 (mp3File_t *file  , U4             *v   )   { if (file->streamType == FSTREAM)   return f_readU4 (&(file->fs),         v);     else   return b_readU4(&(file->bs), v);            }
UC  mreadU2 (mp3File_t *file  , U2             *v   )   { if (file->streamType == FSTREAM)   return f_readU2 (&(file->fs),         v);     else   return b_readU2(&(file->bs), v);            }
UC  mreadUC (mp3File_t *file  , UC             *v   )   { if (file->streamType == FSTREAM)   return f_readUC (&(file->fs),         v);     else   return b_readUC(&(file->bs), v);            }

UC mwriteStr(mp3File_t *file, char *str,      U4 len)   { if (file->streamType == FSTREAM)   return f_writeStr(&(file->fs), str, len);     else   return b_writeStr(&(file->bs), str, len);   }
UC mwriteU4 (mp3File_t *file, U4                   v)   { if (file->streamType == FSTREAM)   return f_writeU4(&(file->fs),         v);     else   return b_writeU4(&(file->bs), v);           }
UC mwriteU2 (mp3File_t *file, U2                   v)   { if (file->streamType == FSTREAM)   return f_writeU2(&(file->fs),         v);     else   return b_writeU2(&(file->bs), v);           }
UC mwriteUC (mp3File_t *file, UC                   v)   { if (file->streamType == FSTREAM)   return f_writeUC(&(file->fs),         v);     else   return b_writeUC(&(file->bs), v);           }

UC mopen    (mp3File_t *file,             char *name)   { if (file->streamType == FSTREAM)   return f_open(file, name);                    else   return b_open(file, name);                  }
U4 mtell    (mp3File_t *file                        )   { if (file->streamType == FSTREAM)   return f_tell(&(file->fs));                   else   return b_tell(&(file->bs));                 }
UC mseek    (mp3File_t *file, S4 offset, int origin )   { if (file->streamType == FSTREAM)   return f_seek(&(file->fs), offset, origin);   else   return b_seek(&(file->bs), offset, origin); }
