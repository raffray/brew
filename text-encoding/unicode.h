#ifndef UNICODE_
#define UNICODE_

#include "buffer.h"
#include "extern.h"

buffer_t *string_to_utf8_str(UC *,   U4, UC, UC);
buffer_t *string_to_utf8_buf(buffer_t *, UC, UC);
/*
UC is_nonPrintable(U4);
UC write_codepoint_to_utf8_1byte(buffer_t *, U4);
UC write_codepoint_to_utf8_2byte(buffer_t *, U4);
UC write_codepoint_to_utf8_3byte(buffer_t *, U4);
UC write_codepoint_to_utf8_4byte(buffer_t *, U4);
*/
UC write_codepoint_to_utf8(buffer_t *, U4  );

#endif // UNICODE_
