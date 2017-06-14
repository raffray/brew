#ifndef LENGTH_MAP_
#define LENGTH_MAP_
#include "includes.h"
#include "mp3Frame.h"

/*
#include "brew_types.h"
#include "prints.h"
*/

typedef struct length_map
{
  U4 map[3][3][3];
  float length;
  U4 frameCount;
} length_map_t;

VD init_map(length_map_t *);
VD process_map_info(length_map_t *);
FL segment_length(length_map_t *);
U4 get_map_frameCount(length_map_t *);
VD add_maps_v1(length_map_t *, length_map_t *, length_map_t *);
VD add_maps_v2(length_map_t *, length_map_t *);
VD add_value_to_map(length_map_t *, mp3Header_t *);
VD print_map(length_map_t *);

#endif // LENGTH_MAP_
