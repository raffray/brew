#ifndef ID3_DISPLAY_
#define ID3_DISPLAY_

#include "includes.h"
#include "brew_types.h"

int tests(int, char **);

UC         doNothing(fileLink_t *);

VD       preDispatch(            );
UC          dispatch(int, char **);
VD      postDispatch(            );

UC test_file(fileLink_t *);

#endif //  ID3_DISPLAY_
