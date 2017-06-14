#ifndef CHAIN_LIST_
#define CHAIN_LIST_

#include "includes.h"
#include "brew_types.h"
#include "mp3Frame_marker_list.h"
#include "length_map.h"

typedef struct chain
{
  U4       first_header;
  U4       start_offset;
  U4   lastFrame_offset; // offset of the start of the last header
  U4         end_offset;
  U4         frameCount;
  U4          byteCount; // <=== end_offset - start_offset
  //                     set it when setting end_offset
  UC              isVbr;
  U2      first_bitRate;

  UC          isVlength; // variable frame length; BAD for vbr frame...
  FL       first_length;
  length_map_t      map;
  FL             length;
  marker_list_t markers;

  chain_t    *next;
} chain_t;

typedef struct chain_list
{
  U4          count;
  chain_t    *first;
  chain_t    * last;
  chain_t **ptrList;
} chain_list_t;

// "private"
chain_t  *create_chain   (              );

// "public"
chain_t *push_chain   (chain_list_t *);
void           create_chain_ptrList(chain_list_t *);
void            clean_chain_list   (chain_list_t *);
int               cmp_chain        (const void *, const void *);
void             print_chain_t(chain_t *);

// private
// chain_t *get_last_chain(mp3File_t *    );
chain_t *get_chain     (mp3File_t *, U4);

VD set_chain_info                (chain_t *, U4, UC, UC);
void set_chain_start_offset      (chain_t *, U4);
U4   get_chain_start_offset      (mp3File_t *, U4);

void set_chain_lastFrame_offset  (chain_t *, U4);
U4   get_chain_lastFrame_offset  (mp3File_t *, U4);
void set_chain_end_offset        (chain_t *, U4);
U4   get_chain_end_offset        (mp3File_t *, U4);
void set_chain_first_header      (chain_t *, U4);
U4   get_chain_first_header      (mp3File_t *, U4);

void   set_chain_isVbr           (chain_t *, UC);
UC     get_chain_isVbr           (mp3File_t *, U4);
void   set_chain_first_bitRate   (chain_t *, U2);
U2     get_chain_first_bitRate   (mp3File_t *, U4);
void   set_chain_bitRate_sum     (chain_t *, U4);
U4     get_chain_bitRate_sum     (mp3File_t *, U4);

void   set_chain_isVlength       (chain_t *, UC);
UC     get_chain_isVlength       (mp3File_t *, U4);
void   set_chain_first_length    (chain_t *, FL);
FL     get_chain_first_length    (mp3File_t *, U4);
void   set_chain_length          (chain_t *, FL);
FL     get_chain_length          (mp3File_t *, U4);

void set_chain_frameCount        (chain_t *, U4);
U4   get_chain_frameCount        (mp3File_t *, U4);
void  set_chain_byteCount        (chain_t *, U4);
U4    get_chain_byteCount        (mp3File_t *, U4);

#endif // CHAIN_LIST_
