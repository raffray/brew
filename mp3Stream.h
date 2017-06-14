#ifndef MP3STREAM_
#define MP3STREAM_

#include "includes.h"
#include "brew_types.h"

U4   get_stream_chainCount  (mp3File_t *    );
void set_stream_start_offset(mp3File_t *, U4);
U4   get_stream_start_offset(mp3File_t *    );
void set_stream_end_offset  (mp3File_t *, U4);
U4   get_stream_end_offset  (mp3File_t *    );
//void set_stream_frameCount  (mp3File_t *, U4);
U4   get_stream_frameCount  (mp3File_t *    );
void set_stream_byteCount   (mp3File_t *, U4);
U4   get_stream_byteCount   (mp3File_t *    );
void set_stream_isVbr       (mp3File_t *, UC);
UC   get_stream_isVbr       (mp3File_t *    );
void set_stream_bitRate     (mp3File_t *, U2);
U2   get_stream_bitRate     (mp3File_t *    );
void set_stream_isVlength   (mp3File_t *, UC);
UC   get_stream_isVlength   (mp3File_t *    );
//void set_stream_length      (mp3File_t *, FL);
FL   get_stream_length      (mp3File_t *    );

UC findHeader         (mp3File_t *                     );
UC findChain          (mp3File_t *,                U4 *);

UC addChain           (mp3File_t *);
UC retrieveChain      (mp3File_t *);

UC getMp3Header       (mp3File_t *, mp3Header_t *, U4 *);

UC processMp3Data     (mp3File_t *,                  U4);
VD retrieve_all_chains(mp3File_t *                     );
VD retrieve_chainCount(mp3File_t *                     );
UC isMp3File          (mp3File_t *                     );
UC setMp3_offsets     (mp3File_t *                 , U4);




#endif // MP3STREAM_
