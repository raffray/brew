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

UC mreadStr (mp3File_t *, UC *, U4);
UC mreadU4  (mp3File_t *, U4 *);
UC mreadU2  (mp3File_t *, U2 *);
UC mreadUC  (mp3File_t *, UC *);

UC mwriteStr(mp3File_t *, UC *, U4);
UC mwriteU4 (mp3File_t *, U4);
UC mwriteU2 (mp3File_t *, U2);
UC mwriteUC (mp3File_t *, UC);

UC mopen    (mp3File_t *, char *);
U4 mtell    (mp3File_t *         );
UC mseek    (mp3File_t *, S4, int);

#endif // MFILE_
