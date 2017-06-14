#include "iso-8859-1.h"

UC nextIso(buffer_t *buf, U4 *w)
{ UC val;

  if(buffer_readUC(buf, &val) == FAILURE)   return FAILURE;
  *w = val;                                 return SUCCESS; }
