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
#include "base_types.h"

#define ID3V1_TAG_LENGTH 128
#define XTD_TAG_LENGTH 227

#define XING 0x58696E67
#define INFO 0x496E666F
#define VBRI 0x56425249

#define ERR_CHAR 0xFFFD

/*
#define VERBOSE1 if((verbose1==1)                 ) {
#define VERBOSE2 if((verbose1==1) && (verbose2==1)) {
#define END_BLOCK }
*/

#define NL  0x0A
#define SP  0x20

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

#define TRY(func, action) if(func == FAILURE ) { action   return FAILURE; }

#endif // INCLUDES_
