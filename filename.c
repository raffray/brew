#include "includes.h"
#include "extern.h"
#include "filename.h"
#include "mio.h"
#include "str.h"
#include "buffer.h"

VD buffer_append(buffer_t *buf, char *str, UC slash)
{ buffer_append_(buf, (UC *)str, slash); }

VD buffer_append_(buffer_t *buf, UC *str, UC slash)
{
  if(str!=NULL)
    { buffer_writeStr(buf, str, strlen_(str));
      if(slash)   buffer_writeUC(buf, '/');
      buffer_close(buf); }
}

UC filename_check(buffer_t *buf, char *extention)
{ // if filename already exists, find the first "filename (xx).mp3" available
  UC i;
  char str[11];
  UC nbLen = 1;
  UC extLen = strlen(extention); // <== typically 4 for ".mp3" or ".jpg"

  buffer_append(buf, extention, false);  // <== maybe it's an mp1 or mp2
  if(!nodeExists(buf->data))   return SUCCESS;

  buffer_seek(buf, -extLen, SEEK_CUR);
  buffer_append(buf, " (1)", false);
  buffer_append(buf, extention, false);
  if(!nodeExists(buf->data))   return SUCCESS;

  for(i=2; i<0xFF; i++)
    {
      if(i== 11)   nbLen++;
      if(i==101)   nbLen++;
      buffer_seek(buf, -(nbLen+3+extLen), SEEK_CUR);
      sprintf(str, " (%d)%s", i, extention);
      buffer_append(buf, str, false);

      if(!nodeExists(buf->data))   return SUCCESS;
    }

  return FAILURE;
}
