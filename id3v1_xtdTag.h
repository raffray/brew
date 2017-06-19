#ifndef ID3V1_XTD_
#define ID3V1_XTD_

#include "includes.h"
#include "brew_types.h"

/*
header 	4 	"TAG+"
title 	60 	60 characters of the title
artist 	60 	60 characters of the artist name
album 	60 	60 characters of the album name
speed 	1 	0=unset, 1=slow, 2= medium, 3=fast, 4=hardcore
genre 	30 	A free-text field for the genre
start-time 	6 	the start of the music as mmm:ss
end-time 	6 	the end of the music as mmm:ss
*/

typedef struct id3v1_xtdTag
{ UC presenceFlag;
  UC title     [61];
  UC artist    [61];
  UC album     [61];
  UC speed     [ 1];
  UC start_time[ 7];
  UC   end_time[ 7];

} id3v1_xtdTag_t;

UC get_id3v1_xtdTag_presenceFlag(mp3File_t *);
VD check_id3v1_xtdTag_presence  (mp3File_t *);
UC retrieve_id3v1_xtdTag        (mp3File_t *);
UC print_id3v1_xtdTag           (mp3File_t *);



UC is_id3v1_xtd_present(mp3File_t *);
UC print_id3v1_xtd_info(mp3File_t *);


#endif // ID3V1_XTD_
