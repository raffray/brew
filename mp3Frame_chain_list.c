#include "mp3Frame_chain_list.h"
#include "mp3File.h"


//chain_t *get_last_chain(mp3File_t *file             )   { return (file->chain_list).last;                                 } // <== in mp3Stream ??
//chain_t *get_chain     (mp3File_t *file, U4 ch_idx  )   { return (file->chain_list).ptrList[ch_idx];                      }
chain_t *get_last_chain(mp3File_t *file             )   { return (file->chain_list).last;                                 } // <== in mp3Stream ??
chain_t *get_chain     (mp3File_t *file, U4 ch_idx  )   { return (file->chain_list).ptrList[ch_idx];                      }

VD set_chain_info                (chain_t  *chain, U4 first_header, UC isVbr, UC isVlength)
{ set_chain_first_header(chain,  first_header);
  set_chain_isVbr       (chain,        isVbr);
  set_chain_isVlength   (chain,    isVlength);
}

VD set_chain_first_header        (chain_t *chain,  U4 header)   {                              chain->first_header = header; }
U4 get_chain_first_header        (mp3File_t *file, U4 index )   { return get_chain     (file, index)->first_header;          }
VD set_chain_start_offset        (chain_t *chain,  U4 offset)   {                              chain->start_offset = offset; }
U4 get_chain_start_offset        (mp3File_t *file, U4 index )   { return get_chain     (file, index)->start_offset;          }

VD set_chain_lastFrame_offset    (chain_t *chain,  U4 offset)   {                              chain->lastFrame_offset = offset; }
U4 get_chain_lastFrame_offset    (mp3File_t *file, U4 index )   { return get_chain     (file, index)->lastFrame_offset;          }
VD set_chain_end_offset          (chain_t *chain,  U4 offset)   {                              chain->end_offset = offset; }
U4 get_chain_end_offset          (mp3File_t *file, U4 index )   { return get_chain     (file, index)->end_offset;          }

//VD set_chain_frameCount          (chain_t *chain, U4 count )   {                              chain->frameCount = count; }
//U4 get_chain_frameCount          (mp3File_t *file, U4 index )   { return get_chain     (file, index)->frameCount; }
U4 get_chain_frameCount          (mp3File_t *file, U4 index )   { return get_chain     (file, index)->map.frameCount; }
VD set_chain_byteCount           (chain_t *chain, U4 count )   {                              chain-> byteCount = count; }
U4 get_chain_byteCount           (mp3File_t *file, U4 index )   { return get_chain     (file, index)-> byteCount; }

VD set_chain_isVbr               (chain_t *chain, UC isVbr  )   {                              chain->isVbr = isVbr; }
UC get_chain_isVbr               (mp3File_t *file, U4 index  )   { return get_chain     (file, index)->isVbr;        }
VD set_chain_first_bitRate       (chain_t *chain, U2 bitRate)   {                              chain->first_bitRate = bitRate; }
U2 get_chain_first_bitRate       (mp3File_t *file, U4 index  )   { return get_chain     (file, index)->first_bitRate;        }

VD set_chain_isVlength           (chain_t *chain, UC isVlength) {                              chain->isVlength = isVlength; }
UC get_chain_isVlength           (mp3File_t *file, U4 index    ) { return get_chain     (file, index)->isVlength; }
VD set_chain_first_length        (chain_t *chain, FL length   ) {                              chain->first_length = length; }
FL get_chain_first_length        (mp3File_t *file, U4 index    ) { return get_chain     (file, index)->first_length; }
//VD set_chain_length              (chain_t *chain, FL length   ) {                              chain->length = length; }
//FL get_chain_length              (mp3File_t *file, U4 index    ) { return get_chain     (file, index)->length; }
FL get_chain_length              (mp3File_t *file, U4 index    ) { return get_chain     (file, index)->map.length; }



chain_t *create_chain()
{ chain_t *newLink = malloc(sizeof(chain_t));
  memset(       newLink, 0,      sizeof(chain_t));
  newLink->next = NULL;
  return newLink; }

chain_t * push_chain(chain_list_t *list)
{ chain_t *newLink = create_chain();

  list->count++;

  if(list->first == NULL)   list->first       = newLink;
  else                     (list->last)->next = newLink;      
  /**/                      list->last        = newLink; 

  return newLink;
}

VD create_chain_ptrList(chain_list_t *list)
{
  U4 i;
  chain_t *link = list->first;

  list->ptrList = malloc(list->count * sizeof(chain_t *));

  for(i=0; i< list->count; i++)
    { list->ptrList[i] = link;
      link = link->next; }
}

VD clean_chain_list(chain_list_t *list)
{
  U4 i;

  for(i=0; i< list->count; i++)
    { /* free link elements that need freein */
      // call clean_subChain_list on all subChains
      free(list->ptrList[i]); }

  free(list->ptrList);
}

int cmp_chain(const VD * a, const VD * b)
{ /*
 return 
 compare  -- (*(chain_t **)a)->element -- to -- (*(chain_t **)b)->element --
  */
  return 0;
 }

VD print_chain_t(chain_t *link)
{

}
