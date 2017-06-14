#include "includes.h"
#include "extern.h"

UC    CPU_ENDIANNESS;

void getCpuEndianNess()
{ UC end[2] = { 0, 1}; // 0x00 01, aka 1 if CPU is big endian. or 256 if little endian

  if( *(U2*)end == 1)   CPU_ENDIANNESS = B_ENDIAN;
  else                  CPU_ENDIANNESS = L_ENDIAN;
  /*
  if( *(U2*)end == 1)   { CPU_ENDIANNESS = B_ENDIAN;   VERBOSE2 printf("Comp reads in Big endian\n"   ); END_BLOCK }
  else                  { CPU_ENDIANNESS = L_ENDIAN;   VERBOSE2 printf("Comp reads in Little endian\n"); END_BLOCK } */
}
