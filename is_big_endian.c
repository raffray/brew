#include <inttypes.h>

int main(int argc, char **argv)
{ int8_t end[2] = { 0x80, 0x01}; // 0x8001 if CPU is big endian. or 0x0180 if little endian
  return ( *(int16_t*)end == 0x8001 ); } // returns 1 (true) if Big Endian, 0 (false) if Little Endian

