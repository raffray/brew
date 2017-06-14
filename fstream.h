#ifndef FSTREAM_
#define FSTREAM_

#include "includes.h"
#include "brew_types.h"

typedef struct fstream
{
  FILE *pFile;
  U4 fileSize;
} fstream_t;

UC f_open0   (fstream_t *, char *);
UC f_open    (mp3File_t *, char *);

U4 get_fstreamSize(fstream_t *);
VD set_fstreamSize(fstream_t *, U4);
//UC  f_read   (fstream_t *, void *, size_t);
UC  f_readStr(fstream_t *, void *, U4    );
UC  f_readU4 (fstream_t *, U4 *          );
UC  f_readU2 (fstream_t *, U2 *          );
UC  f_readUC (fstream_t *, UC *          );


UC f_writeStr(fstream_t *, void *, U4);
UC f_writeU4 (fstream_t *, U4        );
UC f_writeU2 (fstream_t *, U2        );
UC f_writeUC (fstream_t *, UC        );

UC   f_seek (fstream_t *, S4, int);
U4   f_tell (fstream_t *);
//int  f_getc (fstream_t *);

void f_close(fstream_t *);

#endif // FSTREAM_
