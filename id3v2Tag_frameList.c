#include "id3v2Tag.h"
#include "id3v2Tag_header.h"
#include "id3v2Tag_frame.h"
#include "mp3File.h"
#include "mp3Stream.h"
#include "extern.h"
#include "frameID.h"
#include "byte.h"
#include "mfile.h"
#include "buffer.h"
#include "sync_buf.h"
#include "err.h"
#include "fileList.h"


id3v2Tag_frameList_t *get_id3v2Tag_frameList (mp3File_t *file)   { return &(file->id3v2Tag).frameList;       }
U4                    get_id3v2Tag_frameCount(mp3File_t *file)   { return  (file->id3v2Tag).frameList.count; }

id3v2Tag_frameLink_t *create_id3v2Tag_frameLink(U4 frameID, buffer_t *buf, frame_info_t *info) // <== ?? move to id3v2Tag_frame.c ??
{
  id3v2Tag_frameLink_t *newLink = malloc(sizeof(id3v2Tag_frameLink_t));
  memset(newLink, 0, sizeof(id3v2Tag_frameLink_t));

  /* assign values */
  newLink->frameID   =   frameID;
  newLink->buf       =      buf;
  newLink->info      =      info;
  newLink->next      =      NULL;

  return newLink;
}

void push_id3v2Tag_frameLink(id3v2Tag_frameList_t *list, U4 frameID, buffer_t *buf, frame_info_t *info)
{
  id3v2Tag_frameLink_t *newLink = create_id3v2Tag_frameLink(frameID, buf, info);

  list->count++;

  if (list->first == NULL)   list->first       = newLink;
  else                      (list->last)->next = newLink;
  /**/                       list->last        = newLink;
}

void create_id3v2Tag_frame_ptrList(id3v2Tag_frameList_t *list)
{
  U4 i;
  id3v2Tag_frameLink_t *link = list->first;

  list->ptrList = malloc(list->count * sizeof(id3v2Tag_frameLink_t *));

  for(i=0; i< list->count; i++)
    { list->ptrList[i] = link;
      link = link->next; }
}

void clean_id3v2Tag_frameList(id3v2Tag_frameList_t *list)
{
  U4 i;

  for(i=0; i< list->count; i++)
    { /* free link elements that need freeing */
//      printf("zzzzzzzzzz 1 ||||||||||\n");printU4((list->ptrList[i])->data,NL);
      free((list->ptrList[i])->buf->data);
      free((list->ptrList[i])->buf);
      free((list->ptrList[i])->info);
      free( list->ptrList[i]); }

  free(list->ptrList);
}


void retrieve_id3v2Tag_frameList(mp3File_t *file)
{ U4    tagSize = 0;
  id3v2Tag_frameList_t *list  = get_id3v2Tag_frameList(file);

  U4  frameSize = 0;
  UC *tagData;
  U4    tagDataSize;
  UC tagVersion = get_id3v2Tag_version(file);
  U4        xhs = get_id3v2Tag_xtdHdr_size(file);
  buffer_t tagBuffer;

  tagSize =   HEADER_SIZE + xhs; // init
  mseek(file, HEADER_SIZE + xhs, SEEK_SET); // Possible since already checked in retrieveTagHeader (id3Tag.c)

  tagDataSize = get_firstFrame_offset(file) - tagSize;
  tagData = malloc( tagDataSize );
  mreadStr(file, tagData, tagDataSize);
  buffer_open(&tagBuffer, tagData, tagDataSize);

  if ((tagVersion == 3) && get_id3v2Tag_unsync_presenceFlag(file))
    {
//      printf("----- YES\n");      exit(42);
    resyncBuf2(&tagBuffer);
}
//      printf("----- NO\n");      exit(42);

  while ((frameSize = retrieve_id3v2Tag_frame(file, &tagBuffer)) != 0)
    tagSize += HEADER_SIZE  + frameSize;

  buffer_close(&tagBuffer);
  set_id3v2Tag_size(file, tagSize);
  create_id3v2Tag_frame_ptrList(list);

  free(tagData);
}
