#include "str.h"


UC scopy(char **dest, const char *src)
{
  *dest = malloc(strlen(src)+1);
  strcpy(*dest, src);

  return SUCCESS;
}

U4 text_len(UC *text, U4 limit, UC type)
{  //printf("----> "); printU4(limit, NL);
  switch(type)
    {
    case 1  :
    case 2  : return   text_len_2Bytes(text, limit);   break;
    default : return   text_len_1Byte (text, limit);      break;
    }
}

U4 text_len_1Byte(UC *text, U4 limit) // returns the length of a string; if no 0x00 found up to limit, limit is the value returned
// for string terminated by ONE '\0'
{ U4 i=0;
  UC exit = false;
  /*
  "\31 \32 \33 \34"  limit==4
 i==0 --> i=1
 i==1 --> i=2
 i==2 --> i=3
 i==3 --> i=4   no \0 found; return 4+0

  "\31 \32 \00 \34"  limit==4
 i==0 --> i=1
 i==1 --> i=2
 i==2 --> i=2   \0 found; return 2+1

   */
  if(limit==0)   return 0;

  while((exit==false))
    {
      if(text[i]!=0)   i++;
      if(i==limit) exit=true;
    }
  if(i<limit)  i++; // includes the \0
  return i;
}

U4 text_len_2Bytes(UC *text, U4 limit)
{ U4 i=0;

//  printf("limit: %d || ", limit);

  if(limit<2)   return 0;
  if((limit%2)==1)   limit--;
  /*
  "\31 \00 \00 \34 \35"  limit==4
 i==0 --> i=2
 i==2 --> i=4
// testing text[4] is ok, not text[5]
   */

//  printf("limit: %d || ", limit);

  while(((text[i]+text[i+1])!=0) && (i<limit))   {

//    printf("[");  printUC(text[i], SP);
//                  printUC(text[i+1], SP);
//		  printf("] || i : %d || ", i); fflush(stdout);
i+=2;
  }

//    printf("[[[");  printUC(text[i], SP);
//                  printUC(text[i+1], SP);
//    printf("]]] || i : %d || ", i); fflush(stdout);

  if(i<limit)   i+=2;  // includes the 0x00 00
//  printf("i    : %d  ", i); fflush(stdout);

  return i;
}

int strcmp_(UC *s1, UC *s2)   { return strcmp((char*)s1, (char*)s2); }

size_t strlen_ (UC *s)   { return strlen((char*)s); }
