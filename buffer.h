#ifndef BUFFER_
#define BUFFER_

#include "base_types.h"
//#include "brew_types.h"

//typedef struct mp3File mp3File_t;

typedef struct buffer
{
  char *data;
  U4    size;    // used size  <--> SEEK_END
  U4 bufferSize; // allocated size
  U4  cursor;
  UC  toSync;
} buffer_t;


U4    get_buffer_size  (buffer_t *);


UC checkBufferSize(buffer_t *, U4);
VD     buffer_open(buffer_t *, char *, U4);
VD     buffer_init(buffer_t *            );

VD buffer_init2(buffer_t *, U4);
VD buffer_swapData(buffer_t *, buffer_t *);

UC buffer_read   (buffer_t *, void *, U4);
UC buffer_readStr(buffer_t *, void *, U4);
UC buffer_readU4 (buffer_t *, U4 *);
UC buffer_readU2 (buffer_t *, U2 *);
UC buffer_readUC (buffer_t *, UC *);

UC buffer_writeStr(buffer_t *, void *, U4);
UC buffer_writeU4 (buffer_t *, U4);
UC buffer_writeU2 (buffer_t *, U2);
UC buffer_writeUC (buffer_t *, UC);

UC buffer_seek(buffer_t *, S4, int);
U4 buffer_tell(buffer_t *);

UC buffer_clean(buffer_t *);
UC buffer_changeSize(buffer_t *, U4);
UC buffer_close(buffer_t *);

void buffer_print(buffer_t *);

UC buffer_readU4v2(buffer_t *, U4 *);

#endif // BUFFER_
