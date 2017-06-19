#ifndef BUFFER_
#define BUFFER_

#include "base_types.h"

typedef struct buffer
{
  UC  *data;
  U4 dataSize; // allocated size
  U4 usedSize; //  <--> SEEK_END
  U4   cursor;
  UC   toSync;
} buffer_t;

//U4 buffer_get_size(buffer_t *);
VD buffer_open(buffer_t *, UC *, U4);
VD buffer_init_default(buffer_t *            );

VD buffer_init_setSize(buffer_t *, U4);
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

UC buffer_expand(buffer_t *, U4);
//UC buffer_clean(buffer_t *);
//UC buffer_changeSize(buffer_t *, U4);
UC buffer_close(buffer_t *);
UC buffer_free(buffer_t *);

void buffer_print(buffer_t *);

#endif // BUFFER_
