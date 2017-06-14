#ifndef BSTREAM_
#define BSTREAM_
/* Mimicks a file stream using a buffer
//
// Testing, maybe we can do with FILE only... will see later.
// reproducing the behaviour of a FILE
// advantage... have the whole portion we need in memory, no need to worry
// about what happens to the file.
//
// supposed to fit our own purposes more closely
// will allow processing the backup version of a tag too. one buffer for every purpose
******************************************* */

#include "includes.h"
#include "brew_types.h"
#include "buffer.h"

typedef struct bstream
{
//  char *filename;
  UC      toSync;
  buffer_t   buf;
} bstream_t;


UC   b_open (mp3File_t *, char *);

U4 get_bstreamSize(bstream_t *);
VD set_bstreamSize(bstream_t *, U4);

UC b_readStr(bstream_t *, void *, U4);
UC b_readU4 (bstream_t *, U4 *);
UC b_readU2 (bstream_t *, U2 *);
UC b_readUC (bstream_t *, UC *);

UC b_writeStr(bstream_t *, void *, U4);
UC b_writeU4 (bstream_t *, U4);
UC b_writeU2 (bstream_t *, U2);
UC b_writeUC (bstream_t *, UC);

UC b_seek(bstream_t *, S4, int);
U4 b_tell(bstream_t *);

UC b_close(bstream_t *, char *);

#endif // BSTREAM_

