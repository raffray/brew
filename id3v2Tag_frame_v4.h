#ifndef ID3V2TAG_FRAME_V4_
#define ID3V2TAG_FRAME_V4_

#include "includes.h"
#include "brew_types.h"
#include "buffer.h"

UC process_id3v2Tag_frame_flags_v4(mp3File_t *, frame_info_t *, buffer_t *, U4);
U4 get_id3v2Tag_frameSize_v4(U4);
U4 set_id3v2Tag_frameSize_v4(U4);

#endif // ID3V2TAG_FRAME_V4_
