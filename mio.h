#ifndef MIO_
#define MIO_

#include "includes.h"

// option 0 for doing Nothing
//#define B  1
//#define R  2
#define P  1
#define E  3
#define W  4
#define N  5
#define X  6
#define T  7
#define C  8
#define OPTION_COUNT 9 // <== don't forget to update when adding an option

#define A  20
#define S  21

#define V1 22
#define V2 23

VD  resetFile(char *);
UC isNumber  (char *);
UC isRegFile (char *);
UC isDir     (char *);
UC nodeExists(UC *);
UC isFlag    (char *);

#endif // MIO_
