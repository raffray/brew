#ifndef ID3V2TAG_FRAMELIST_
#define ID3V2TAG_FRAMELIST_

#include "includes.h"
#include "brew_types.h"
#include "id3v2Tag_frame.h"


typedef struct id3v2Tag_frameList
{
  U4                       count;
  id3v2Tag_frameLink_t    *first;
  id3v2Tag_frameLink_t    * last;
  id3v2Tag_frameLink_t **ptrList;
} id3v2Tag_frameList_t;


id3v2Tag_frameList_t *get_id3v2Tag_frameList (mp3File_t *file);
U4                    get_id3v2Tag_frameCount(mp3File_t *);
void                  set_id3v2Tag_frameCount(mp3File_t *file, U4 frameCount);

id3v2Tag_frameLink_t *create_id3v2Tag_frameLink(U4, buffer_t *, frame_info_t*);
void                    push_id3v2Tag_frameLink(id3v2Tag_frameList_t *, U4, buffer_t *, frame_info_t*);

void            create_id3v2Tag_frame_ptrList(id3v2Tag_frameList_t *);
void                clean_id3v2Tag_frameList(id3v2Tag_frameList_t *);

void retrieve_id3v2Tag_frameList(mp3File_t *);

#endif // ID3V2TAG_FRAMELIST_

