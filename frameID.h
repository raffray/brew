#ifndef FRAME_ID_
#define FRAME_ID_
#include "includes.h"
/*
#define TIT2 0
#define TPE1 1
#define TPE2 2
#define TALB 3
#define TRCK 4
#define TYER 5
*/
UC is_tag_char_valid(UC);
UC is_tag_valid (U4);
UC is_COMM_tag  (U4);
UC is_POPM_tag  (U4);
UC is_Text_tag  (U4);
UC is_TXXX_tag  (U4);
UC is_TCON_tag  (U4);
UC is_APIC_tag  (U4);
/*
UC is_PCNT_tag  (UC,UC,UC,UC);
UC is_OWNE_tag  (UC,UC,UC,UC);
*/

#endif //FRAME_ID_
