#include "length_map.h"
#include "extern.h"

/*
 Purpose of this map
 in the calculation of segments' length and average bitrates.

#1 avoid compoundind errors
  if we compute the length for each frame and then sum them all together
  we accumulate errors (due to limited float precisions)
#2 to prevent #1 we could use doubles instead of floats
#3 It is much more efficient in terms of number of divisions
   3*3*3 in this case versus thousands for segments of thousands of frames
#4 avoid overflow errors in calculation of average bitrate
   if we sum bitrates over a great number of frames, we could go
   over 0xFFFFFFFF (theoretically possible). A problem when strictly 32bit 
 */


VD init_map(length_map_t *map)
{
  memset(map, 0, sizeof(length_map_t));
}

VD process_map_info(length_map_t *map)
{
  float tmp;
  U2 samplingRate;
  U4 i, j, k;

  for(i=0;i<3;i++) // version       --- hdr->versionId -1
    for(j=0;j<3;j++) // layer         --- hdr->layer -1
      for(k=0;k<3;k++) // samplingRate  --- getSamplingRate(hdr->hdr32)
	if (map->map[i][j][k]!=0)
	  { samplingRate = samplingRate_idx[i][k] ;
	    tmp  = (FL) samplesPerFrame_idx [i][j] / samplingRate;
	    map->length += map->map[i][j][k] * tmp;  
	    map->frameCount+=map->map[i][j][k];
	  }
}

FL segment_length(length_map_t *map)
{
  FL length = 0;
  FL tmp;
  U2 samplingRate;
  U4 i, j, k;
//U4 framecount=0;
  for(i=0;i<3;i++) // version       --- hdr->versionId -1
    for(j=0;j<3;j++) // layer         --- hdr->layer -1
      for(k=0;k<3;k++) // samplingRate  --- getSamplingRate(hdr->hdr32)
	if (map->map[i][j][k]!=0)
	  { samplingRate = samplingRate_idx[i][k] ;
	    tmp  = (FL) samplesPerFrame_idx [i][j] / samplingRate;
	    length += map->map[i][j][k] * tmp;  
//	    framecount+=map->map[i][j][k];
	  }
//printf("%d\n", framecount);
  return length;
}

U4 get_map_frameCount(length_map_t *map)
{
  U4 i, j, k;
  U4 framecount=0;
  for(i=0;i<3;i++) // version       --- hdr->versionId -1
    for(j=0;j<3;j++) // layer         --- hdr->layer -1
      for(k=0;k<3;k++) // samplingRate  --- getSamplingRate(hdr->hdr32)
	if (map->map[i][j][k]!=0)
	    framecount+=map->map[i][j][k];

  return framecount;
}

VD add_maps_v1(length_map_t *res, length_map_t *map1, length_map_t *map2)
{
  U4 i, j, k;

  for(i=0;i<3;i++) // version       --- hdr->versionId -1
    for(j=0;j<3;j++) // layer         --- hdr->layer -1
      for(k=0;k<3;k++) // samplingRate  --- getSamplingRate(hdr->hdr32)
	res->map[i][j][k] = map1->map[i][j][k] + map2->map[i][j][k];
}

VD add_maps_v2(length_map_t *res, length_map_t *map1)
{
  U4 i, j, k;

  for(i=0;i<3;i++) // version       --- hdr->versionId -1
    for(j=0;j<3;j++) // layer         --- hdr->layer -1
      for(k=0;k<3;k++) // samplingRate  --- getSamplingRate(hdr->hdr32)
	res->map[i][j][k] += map1->map[i][j][k];
//  printf("+\n");
}

VD add_value_to_map(length_map_t *map, mp3Header_t *hdr)
{
  map->map[hdr->versionId-1][hdr->layer-1][getSamplingRate(hdr->hdr32)]++;
//  printf(".");
}

VD print_map(length_map_t *map)
{
  U4 i, j, k;

  for(i=0;i<3;i++) // version       --- hdr->versionId -1
    for(j=0;j<3;j++) // layer         --- hdr->layer -1
      for(k=0;k<3;k++) // samplingRate  --- getSamplingRate(hdr->hdr32)
	printf("%5d ", map->map[i][j][k]);
  printf("\n");
}
