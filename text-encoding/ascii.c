#include "ascii.h"

UC nextAscii(buffer_t *buf, U4 *w)
{ UC val;

  if(buffer_readUC(buf, &val) == FAILURE)   return FAILURE;
  if (val>0x7E)   *w = ERR_CHAR;
  else            *w = val;
  /**/                                      return SUCCESS;
}
