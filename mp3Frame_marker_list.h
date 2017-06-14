#ifndef MARKER_LIST_
#define MARKER_LIST_

#include "includes.h"
#include "brew_types.h"
#include "length_map.h"

typedef struct marker
{
  U4        start_offset;
//  U4      frame_position;
//  U4       byte_position; // <== seems unused
//  FL      track_position;
  U4              header;
  mp3Header_t        hdr;
  length_map_t    accMap; // the accumulation of infos up to this marker
  length_map_t       map; // the info about this marker
  marker_t    *next;
} marker_t;

typedef struct marker_list
{
  U4           count;
  marker_t    *first;
  marker_t    * last;
  marker_t **ptrList;
} marker_list_t;

marker_t  *create_marker   (U4, U4); //mp3Header_t *);
marker_t * push_marker(marker_list_t *, U4, U4); //mp3Header_t *);

void              create_marker_ptrList(marker_list_t *);
void               clean_marker_list   (marker_list_t *);
int               cmp_marker        (const void *, const void *);
void                print_marker_t(marker_t *);

//marker_list_t *get_last_chain_marker_list     (mp3File_t *);
marker_list_t *get_chain_marker_list          (mp3File_t *, U4 );
marker_t *get_chain_marker     (mp3File_t *, U4 , U4 );

U4             get_chain_marker_count         (mp3File_t *, U4 );
U4             get_chain_marker_start_offset  (mp3File_t *, U4 , U4 );
U4             get_chain_marker_frame_position(mp3File_t *, U4 , U4 );
//U4             get_chain_marker_byte_position (mp3File_t *, U4 , U4 );
FL               get_chain_marker_track_position(mp3File_t *, U4 , U4 );
U4               get_chain_marker_header        (mp3File_t *, U4 , U4 );

#endif // MARKER_LIST_
