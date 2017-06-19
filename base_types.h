#ifndef BASE_TYPES_
#define BASE_TYPES_

#include <inttypes.h>

#define false    0
#define true     1

#define SUCCESS  0
#define FAILURE  1
// for printing:  printf...  %"PRIu64"  and %"PRIu32"     or %"PRId32"...
//                           %lu            %zu              %l
#define U8 uint64_t
#define S8  int64_t
#define U4 uint32_t
#define S4  int32_t
#define U2 uint16_t
#define UC uint8_t //unsigned char
#define SC  int8_t //char

#define VD void
#define FL double

#define CS char *

#define MAX32 0x7FFFFFFF
#define MAX64 0x7FFFFFFFFFFFFFFF

#endif // BASE_TYPES_

