#include "includes.h"
#include "byte.h"
#include "extern.h"
//#include "err.h"

#ifdef LITTLE_ENDIAN
U8 swapU8(U8 val) { return ( (((val) >> 56) & 0x00000000000000FF) | (((val) >> 40) & 0x000000000000FF00) |
			     (((val) >> 24) & 0x0000000000FF0000) | (((val) >>  8) & 0x00000000FF000000) |
			     (((val) <<  8) & 0x000000FF00000000) | (((val) << 24) & 0x0000FF0000000000) |
			     (((val) << 40) & 0x00FF000000000000) | (((val) << 56) & 0xFF00000000000000) ); }
U4 swapU4(U4 val) { return ( (((val) >> 24) & 0x000000FF)         | (((val) >>  8) & 0x0000FF00)         |
			     (((val) <<  8) & 0x00FF0000)         | (((val) << 24) & 0xFF000000)         ); }
U2 swapU2(U2 val) { return ( (((val) >>  8) & 0x00FF)             | (((val  <<  8) & 0xFF00))            ); }
#else
U8 swapU8(U8 val) { return val; }
U4 swapU4(U4 val) { return val; }
U2 swapU2(U2 val) { return val; }
#endif

void printU8str(U8 l, UC fc) { fprintf(ostream, "%c%c%c%c%c%c%c%c%c"   , (UC)(l>>56), (UC)(l>>48), (UC)(l>>40), (UC)(l>>32)
				       /**/                            , (UC)(l>>24), (UC)(l>>16), (UC)(l>> 8), (UC)l, fc); }
void printU4str(U4 l, UC fc) { fprintf(ostream, "%c%c%c%c%c"           , (UC)(l>>24), (UC)(l>>16), (UC)(l>> 8), (UC)l, fc); }
void printU2str(U2 s, UC fc) { fprintf(ostream, "%c%c%c"                                         , (UC)(s>> 8), (UC)s, fc); }
void printUCstr(UC c, UC fc) { fprintf(ostream, "%c%c"                                                        ,     c, fc); }

void printU8   (U8 l, UC fc) { fprintf(ostream, "%02X %02X %02X %02X %02X %02X %02X %02X   %c",
				       (UC)(l>>56), (UC)(l>>48), (UC)(l>>40), (UC)(l>>32), (UC)(l>>24), (UC)(l>>16), (UC)(l>> 8), (UC)(l>> 0), fc); }
void printU4   (U4 l, UC fc) { fprintf(ostream, "%02X %02X %02X %02X%c", (UC)(l>>24), (UC)(l>>16), (UC)(l>> 8), (UC)l, fc); }
void printU2   (U2 s, UC fc) { fprintf(ostream, "%02X %02X%c"                                    , (UC)(s>> 8), (UC)s, fc); }
void printUC   (UC c, UC fc) { fprintf(ostream, "%02X%c"                                                      ,     c, fc); }

void printBits (UC c, UC fc) { int i;   for(i=0; i<4; i++)   printf("%d", (c&(1<<i))>>i);   printf(" ");
  /**/                                  for(i=4; i<8; i++)   printf("%d", (c&(1<<i))>>i);   printf(" %c", fc); }

U4 decodeU4(U4 val) { return ((val & 0x0000007F) | ((val  & 0x00007F00)>>1) | ((val  & 0x007F0000)>>2) | ((val  & 0x7F000000)>>3)); }
U4 encodeU4(U4 val) { return ((val & 0x0000007F) | ((val <<1) & 0x00007F00) | ((val <<2) & 0x007F0000) | ((val <<3) & 0x7F000000)); }
