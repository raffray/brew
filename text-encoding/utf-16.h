#ifndef UTF16_
#define UTF16_

#include "includes.h"
#include "brew_types.h"
#include "buffer.h"

UC get_UTF16_enc(buffer_t *, UC *);
UC nextUTF16    (buffer_t *, U4 *, UC);

#endif // UTF16_
