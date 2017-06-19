#ifndef FILENAME_
#define FILENAME_

#include "includes.h"
#include "brew_types.h"

VD buffer_append (buffer_t *, char *, UC);
VD buffer_append_(buffer_t *, UC *, UC);
UC filename_check(buffer_t *, char *);

#endif /* FILENAME_ */
