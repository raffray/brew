#ifndef BYTE_
#define BYTE_
#include "includes.h"

#define swapU8(s) (CPU_ENDIANNESS == L_ENDIAN) ? Swap8Bytes(s) : s
#define swapU4(s) (CPU_ENDIANNESS == L_ENDIAN) ? Swap4Bytes(s) : s
#define swapU2(s) (CPU_ENDIANNESS == L_ENDIAN) ? Swap2Bytes(s) : s

U8 Swap8Bytes(U8);
U4 Swap4Bytes(U4);
U2 Swap2Bytes(U2);

void printU4str(U4, UC);
void printU2str(U2, UC);
void printUCstr(UC, UC);

void printU8(U8, UC); // |xx xx xx xx xx xx xx xx .. xx xx xx  x
void printU4(U4, UC); // |xx xx xx xx .. xx xx xx xx ..
void printU2(U2, UC); // |xx xx .. xx xx .. xx xx .. xx xx        00 A4 37 FB 00 A4 37 FB
void printUC(UC, UC); // |xx .. xx .. xx .. xx .. xx              00A4   37FB    00A4   37FB
void printUC_NL (UC);
void printUC_SP (UC);
void printUC_3SP(UC);

U4 decodeU4(U4);
U4 encodeU4(U4);

U2 svalue(UC, UC, UC);
U4 lvalue(UC, UC, UC, UC, UC);

#endif // BYTE_
