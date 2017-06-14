#ifndef MP3FRAME_
#define MP3FRAME_

#include "includes.h"
#include "brew_types.h"

typedef struct mp3Header // <== mp3Frame_header
{
  U4 hdr32;
  UC versionId;
  UC layer;
  U2 bitRate;
  U2 samplingRate;
  UC paddingBit;
  UC channelMode;
  U4 frameSize;  // in bytes
  FL    length;  // duration, in ms
} mp3Header_t;

UC isMp3Header    (U4);
UC setMp3Header   (mp3Header_t *, U4);
UC getMp3Header   (mp3File_t *, mp3Header_t *, U4*);

//U4 getFrameSync   (U4);
UC getVersionId   (U4);
UC getLayer       (U4);
U2 getBitRate     (U4);
UC getSamplingRate(U4);
UC getPadding     (U4);
UC getChannelMode (U4);

VD printMp3Header           (              mp3Header_t *    );
VD printMp3Header_line      (              mp3Header_t *    );
VD printMp3Header_line_short(              mp3Header_t *    );
UC equivHeaders_vlsc_U4     (              U4           , U4);
UC equivHeaders_vlsc        (              mp3Header_t *, U4);




#endif // MP3FRAME_
