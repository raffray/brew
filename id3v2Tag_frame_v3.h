#ifndef ID3V2TAG_FRAME_V3_
#define ID3V2TAG_FRAME_V3_

#include "includes.h"
#include "brew_types.h"
#include "buffer.h"
/*
typedef struct id3v2Tag_frame_v3
{

} id3v2Tag_frame_v3_;
*/

#define FRAME_COUNT_V3 2 

typedef struct id3v2Tag_frame_select
{
  U4 frameID;
  char *frameID_str;
  char *description;
} id3v2Tag_frame_select_;

/*

id3v2Tag_frame_select_ frame_select[FRAME_COUNT_v3];




 */

UC process_id3v2Tag_frame_flags_v3(frame_info_t *, buffer_t *, U4);
U4 get_id3v2Tag_frameSize_v3(U4);
U4 set_id3v2Tag_frameSize_v3(U4);

#endif // ID3V2TAG_FRAME_V3_
