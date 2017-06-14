#ifndef ID3V2TAG_
#define ID3V2TAG_

#include "includes.h"
#include "brew_types.h"
#include "id3v2Tag_header.h"
#include "id3v2Tag_frameList.h"

/*
   Overall tag structure:

     +-----------------------------+  ---
     |      Header (10 bytes)      |    |
     +-----------------------------+    |
     |       Extended Header       |    | the value of the field "size" below
     | (variable length, OPTIONAL) |    |
     +-----------------------------+    |
     |   Frames (variable length)  |    |
     +-----------------------------+ ---
     |           Padding           |
     | (variable length, OPTIONAL) |
     +-----------------------------+
     | Footer (10 bytes, OPTIONAL) |
     +-----------------------------+
 */

typedef struct id3v2Tag
{ id3v2Tag_header_t    header;
  U4                   size;      // aka, offset at the end of the last frame.
  id3v2Tag_frameList_t frameList;
} id3v2Tag_t;


// Technically:
// The ID3v2 tag size is the sum of the byte length of the extended header,
// the padding and the frames after unsynchronisation. If a footer is present
// this equals to ('total size' - 20) bytes, otherwise    *(footer in v2.4 only)
// ('total size' - 10) bytes.
//
// different meaning in this program, for now, it is header + frames. padding is disregarded.
// footer and extended header remain to be implemented. They may never be.
// BUT in all cases, it will represent the size of the data block we backup.

void set_id3v2Tag_size        (mp3File_t *file, U4 size);
U4   get_id3v2Tag_size        (mp3File_t *file);

UC   retrieve_id3v2Tag          (mp3File_t *);
void    clean_id3v2Tag             (id3v2Tag_t *); // <== (mp3File_t *);

void print_id3v2Tag(mp3File_t *file);
U4    dump_id3v2Tag(mp3File_t *);

void extractPictures(mp3File_t *);

#endif // ID3V2TAG_
