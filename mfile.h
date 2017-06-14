/*
 Meta-file structure
 We will handle files as either a "FILE *" or a buffer
 cf f_stream.* / b_stream.*
 */

#ifndef MFILE_
#define MFILE_
#include "includes.h"
//#include "brew_types.h"
//#include "mp3File.h"

typedef struct mp3File mp3File_t;


UC  mreadStr(mp3File_t *file, char *str, U4 len);
UC  mreadU4 (mp3File_t *file  , U4        *v   );
UC  mreadU2 (mp3File_t *file  , U2        *v   );
UC  mreadUC (mp3File_t *file  , UC        *v   );

UC mwriteStr(mp3File_t *file, char *str, U4 len);
UC mwriteU4 (mp3File_t *file, U4              v);
UC mwriteU2 (mp3File_t *file, U2              v);
UC mwriteUC (mp3File_t *file, UC              v);

UC mopen    (mp3File_t *file,        char *name);
U4 mtell    (mp3File_t *file                   );
UC mseek    (mp3File_t *file, S4 offset, int origin );


#endif // MFILE_
