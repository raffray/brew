#include "utf-8_prints.h"
#include "unicode.h"
/*
#include "utf-16.h"
#include "iso-8859-1.h"
#include "ascii.h"
*/
#include "err.h"
#include "str.h"


void eatZeroes(buffer_t *buf)
{ UC c;
  while(buffer_tell(buf)<buf->usedSize)
  {
    buffer_readUC(buf, &c);
    if(c!=0)
    {
      buffer_seek(buf, -1, SEEK_CUR);
      break;
    }
  }
}

void findNextZero(buffer_t *buf)
{ UC c;
  while(buffer_tell(buf)<buf->usedSize)
  { buffer_readUC(buf, &c);
    if(c==0)   break; }
}

UC print_firstString_str(UC *data, U4 size, UC type_known, UC type_given)
{ buffer_t buf;

  buffer_open  (&buf, data, size);
  return print_firstString_buf(&buf, type_known, type_given);
}

UC print_firstString_buf(buffer_t *buffin, UC type_known, UC type_given)
{
  buffer_t *buf;

  if(!buffin->usedSize)   return SUCCESS; // Empty string, nothing to print

  buf = string_to_utf8_buf(buffin, type_known, type_given); // We are guaranteed we at least have one zero at the end

  buffer_seek(buf, 0, SEEK_SET);
  eatZeroes(buf);
  fprintf(ostream, "%s", buf->data+buffer_tell(buf));

  free(buf->data);
  free(buf);

  return SUCCESS;
}


UC print_multiString_str(UC *data, U4 size, UC type_known, UC type_given)
{ buffer_t buf;

  buffer_open  (&buf, data, size);
  return print_multiString_buf(&buf, type_known, type_given);
}

UC print_multiString_buf(buffer_t *buffin, UC type_known, UC type_given)
{
  buffer_t *buf;
  UC firstString_flag =0;

  if(!buffin->usedSize)   return SUCCESS; // Empty string, nothing to print

  buf = string_to_utf8_buf(buffin, type_known, type_given); // We are guaranteed we at least have one zero at the end

  buffer_seek(buf, 0, SEEK_SET);
  while(buffer_tell(buf)<buf->usedSize)
  {
    eatZeroes(buf);
    if(firstString_flag==0)   firstString_flag++;
    else
      if(buffer_tell(buf)<buf->usedSize) // if we haven't reached an end-of-string block of zeroes
         printf("\n       ");

    fprintf(ostream, "%s", buf->data+buffer_tell(buf));
    findNextZero(buf);
  }

  free(buf->data);
  free(buf);

  return SUCCESS;
}




UC print_string_obsolete(UC *str, U4 len, UC type_known, UC type_given)
{

    buffer_t *buf;
    U4 dataSize = 0;
    UC *data;

    if(len==0)   return SUCCESS;

    buf = string_to_utf8_str(str, len, type_known, type_given);
   //  buffer_print(buf);
    data = buf->data;
    dataSize = buf->usedSize;
    while(true)
      { while((*data) == '\0') data++; //strip zeroes
        if(data>=buf->data+dataSize)   return SUCCESS;
        { if (data == buf->data)   fprintf(ostream,          "%s", data);
  	else                     fprintf(ostream, "\n       %s", data); }
        data += strlen_(data);
      }

    free(buf->data);
    free(buf);

   //printf("?want to use print_string_str?\n");
    return SUCCESS;
}
