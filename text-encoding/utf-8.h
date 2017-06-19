#ifndef UTF8_
#define UTF8_

#include "buffer.h"
#include "extern.h"

UC          print_string_str(UC *,   U4, UC, UC);
UC          print_string_buf(buffer_t *, UC, UC);
UC          print_string    (UC *, U4, UC, UC);

buffer_t *string_to_utf8_str(UC *,   U4, UC, UC);
buffer_t *string_to_utf8_buf(buffer_t *, UC, UC);

UC nextUTF8               (buffer_t *, U4 *);
UC write_codepoint_to_utf8(buffer_t *, U4  );
UC count_UTF8Bytes(U4);
UC is_nonPrintable(U4);


char *get_Txyz_string(char *str, U4 len, UC type_known, UC type_given);
UC replace_badChar(buffer_t *buf);

#endif // UTF8_
