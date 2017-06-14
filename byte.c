#include "includes.h"
#include "byte.h"
#include "extern.h"
#include "err.h"

U4 Swap4Bytes(U4 val) { return ( (((val) >> 24) & 0x000000FF)         | (((val) >>  8) & 0x0000FF00)         |
			         (((val) <<  8) & 0x00FF0000)         | (((val) << 24) & 0xFF000000)         ); }
U2 Swap2Bytes(U2 val) { return ( (((val) >>  8) & 0x00FF)             | (((val  <<  8) & 0xFF00))            ); }

void printU4str(U4 l, UC fc) { fprintf(ostream, "%c%c%c%c%c"           , (UC)(l>>24), (UC)(l>>16), (UC)(l>> 8), (UC)l, fc); } 
void printU2str(U2 s, UC fc) { fprintf(ostream, "%c%c%c"                                         , (UC)(s>> 8), (UC)s, fc); } 
void printUCstr(UC c, UC fc) { fprintf(ostream, "%c%c"                                                        ,     c, fc); }
void printU4   (U4 l, UC fc) { fprintf(ostream, "%02X %02X %02X %02X%c", (UC)(l>>24), (UC)(l>>16), (UC)(l>> 8), (UC)l, fc); }
void printU2   (U2 s, UC fc) { fprintf(ostream, "%02X %02X%c"                                    , (UC)(s>> 8), (UC)s, fc); } 
void printUC   (UC c, UC fc) { fprintf(ostream, "%02X%c"                                                      ,     c, fc); }

U4 decodeU4(U4 encSize) { return ((encSize & 0x0000007F) | ((encSize  & 0x00007F00)>>1) | ((encSize  & 0x007F0000)>>2) | ((encSize  & 0x7F000000)>>3)); }
U4 encodeU4(U4 decSize) { return ((decSize & 0x0000007F) | ((decSize <<1) & 0x00007F00) | ((decSize <<2) & 0x007F0000) | ((decSize <<3) & 0x7F000000)); }
