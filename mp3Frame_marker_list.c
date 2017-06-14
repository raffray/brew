#include "mp3Frame_marker_list.h"
#include "prints.h"
#include "byte.h"
#include "mp3File.h" // <== remove
#include "length_map.h"

marker_list_t *get_chain_marker_list(mp3File_t *file, U4 ch_idx)           { return &(get_chain    (file, ch_idx))->markers;                }
marker_t      *get_chain_marker     (mp3File_t *file, U4 ch_idx, U4 m_idx) { return get_chain_marker_list(file, ch_idx)->ptrList[m_idx];        }

// public
U4             get_chain_marker_count         (mp3File_t *file, U4 ch_idx)           { return get_chain_marker_list(file, ch_idx)->count;                 }
U4             get_chain_marker_start_offset  (mp3File_t *file, U4 ch_idx, U4 m_idx) { return get_chain_marker(file, ch_idx, m_idx)->  start_offset; }
U4             get_chain_marker_header        (mp3File_t *file, U4 ch_idx, U4 m_idx) { return get_chain_marker(file, ch_idx, m_idx)->        header; }
length_map_t  *get_chain_marker_map           (mp3File_t *file, U4 ch_idx, U4 m_idx) { return &(get_chain_marker(file, ch_idx, m_idx)->           map); }

marker_t *create_marker(U4 start_offset, U4 header)
{
  marker_t *newLink = malloc(sizeof(marker_t));
  memset(   newLink, 0,      sizeof(marker_t));
  /* assign values */
  newLink-> start_offset = start_offset;
  newLink->       header =       header;
  setMp3Header(&newLink->hdr,    header);

  newLink->next = NULL;

  return newLink;
}

marker_t * push_marker(marker_list_t *list, U4 start_offset, U4 header)
{ marker_t *newLink = create_marker( start_offset, header);

  list->count++;

  if(list->first == NULL)   list->first = list->last = newLink;
  else
    { (list->last)->next = newLink;      
      list->last         = newLink; }

  return newLink;
}

void create_marker_ptrList(marker_list_t *list)
{
  U4 i;
  marker_t *link = list->first;

  list->ptrList = malloc(list->count * sizeof(marker_t *));

  for(i=0; i< list->count; i++)
    { list->ptrList[i] = link;
      link = link->next; }
}

void clean_marker_list(marker_list_t *list)
{
  U4 i;

  for(i=0; i< list->count; i++)
    { /* free link elements that need freein */
      free(list->ptrList[i]); }

  free(list->ptrList);
}

int cmp_marker(const void * a, const void * b)
{ /*
 return 
 compare  -- (*(marker_t **)a)->element -- to -- (*(marker_t **)b)->element --
  */
  return 0;
 }

void print_marker_t(marker_t *link)
{
  printf("==> ");   printU4(link->start_offset, SP);
  printf("==> ");   printU4(link->header, NL);
}
