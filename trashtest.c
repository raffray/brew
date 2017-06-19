#include <stdio.h>
#include "buffer.h"
#include "byte.h"

FILE     *ostream;

void check(unsigned short a)
{
//  printf("%d %d\n", a, a|(unsigned short)0xFF00);
  if( (a>>8)!=0 && (a&0x00FF)==0) printf("ok\n"); else  printf("not ok\n");
}

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

void main()
{
UC firstString_flag=0;
buffer_t *buf = malloc(sizeof(buffer_t));
  // printf("%d\n", sizeof(short));
//  if((0x8000|0xFF00) == 0xFF00) printf("ok\n"); else  printf("not ok\n");
  check(0x8000);
  check(0x8001);
  check(0x0001);
  check(0x0000);

  // 00 00 33 33 33 00 33 00 00 33 00 33 00 00 00
  // Cleaned up string:
  //       33 33 33 00
  //                   33 00
  //                            33 00
  //                                  33 00
  //
  buffer_init_default(buf);
  buffer_writeU4(buf, 0x00003333);
  buffer_writeU4(buf, 0x33003300);
  buffer_writeU4(buf, 0x00330033);
  buffer_writeU4(buf, 0x00000000);



  while(buffer_tell(buf)<buf->usedSize)
  {
    eatZeroes(buf);
    if(firstString_flag==0)
    {
      printf("\n       ");
      firstString_flag++;
    }
    printf(         "%s", buf->data+buffer_tell(buf));
  }

  free(buf->data);
  free(buf);
}
