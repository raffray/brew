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
    case 2  : return   text_len_2Bytes(text, limit);   break; // UTF-16 and UTF-16BE
    default : return   text_len_1Byte (text, limit);   break;
    }
}

U4 text_len_1Byte(UC *text, U4 limit) // returns the length of a string; if no 0x00 found up to limit, limit is the value returned
// for string terminated by ONE '\0'
{
   U4 i=0;

  for(i=0; i<limit; i++)   { if(text[i]==0)   break; }

  if(i<limit)  i++; // includes the \0
  return i;
}

U4 text_len_2Bytes(UC *text, U4 limit)
{ U4 i=0;

  if(limit<2)   return 0;
  if((limit%2)==1)   limit--;

//  while(((text[i]+text[i+1])!=0) && (i<limit))  { i+=2; }
for(i=0; i<limit; i+=2)   { if((text[i]+text[i+1])==0)   break; }

  if(i<limit)   i+=2;  // includes the 0x00 00

  return i;
}

int strcmp_(UC *s1, UC *s2)   { return strcmp((char*)s1, (char*)s2); }

size_t strlen_ (UC *s)   { return strlen((char*)s); }
