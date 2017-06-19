#ifndef STR_
#define STR_

#include "includes.h"

UC scopy   (char **, const char *);
//U4 text_len(char * , U4          );

U4 text_len       (UC *, U4, UC type);
U4 text_len_1Byte (UC *, U4);
U4 text_len_2Bytes(UC *, U4);

int strcmp_(UC *s1, UC *);
size_t strlen_ (UC *);

#endif // STR_
