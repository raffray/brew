#ifndef INCLUDES_
#define INCLUDES_

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
//#include <locale.h>
//#include <wchar.h>

#include "base_types.h"

#define ID3V1_TAG_LENGTH 128
#define XTD_TAG_LENGTH 227

#define XING 0x58696E67
#define INFO 0x496E666F
#define VBRI 0x56425249

#define ERR_CHAR 0xFFFD

/*
#define TIT2 ....

 */


/*
#define VERBOSE1 if((verbose1==1)                 ) {
#define VERBOSE2 if((verbose1==1) && (verbose2==1)) {
#define END_BLOCK }
*/

#define NL  0x0A
#define SP  0x20


#define Fprintf(str     ) fprintf(ostream, str)
#define Fprintf2(str, arg) fprintf(ostream, str, arg)

#define ALL      0
#define SELECT   1
#define TRACK    2
#define SCAN     3

#define B_ENDIAN 1
#define L_ENDIAN 0

#define  FSTREAM 0
#define  BSTREAM 1

#define ver3     3
#define ver4     4

#define HEADER_SIZE 10 // All headers are 10 byte long (tag and tag-frames)


/*
#define false    0
#define true     1

#define SUCCESS  0
#define FAILURE  1
// for printing:  printf...  %"PRIu64"  and %"PRIu32"     or %"PRId32"...
//                           %lu            %zu              %l
#define U8 uint64_t
#define U4 uint32_t
#define S4  int32_t
#define U2 unsigned short
#define UC unsigned char
#define SC char

#define VD void
#define FL double

#define CS char *

#define MAX32 0x7FFFFFFF
#define MAX64 0x7FFFFFFFFFFFFFFF
*/

#define TRY(func, action) if(func == FAILURE ) { action   return FAILURE; }
/*
#define INFO 1
#define XING 2
#define VBRI 3
*/

#endif // INCLUDES_

