#ifndef STR_
#define STR_

#include "includes.h"

UC scopy   (char **, const char *);
//U4 text_len(char * , U4          );

U4 text_len       (char *, U4, UC type);
U4 text_len_1Byte (char *, U4);
U4 text_len_2Bytes(char *, U4);


#endif // STR_
