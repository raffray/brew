#ifndef ID3V1TAG_
#define ID3V1TAG_

#include "includes.h"
#include "brew_types.h"

/*
header 	        3 	"TAG"
title 	       30 	30 characters of the title
artist 	       30 	30 characters of the artist name
album 	       30 	30 characters of the album name
year 	        4 	A four-digit year
comment 	28[4] or 30 	The comment.
zero-byte[4] 	1 	If a track number is stored, this byte contains a binary 0.
track[4] 	1 	The number of the track on the album, or 0. Invalid, if previous byte is not a binary 0.
genre 	1 	Index in a list of genres, or 255
*/

typedef struct id3v1Tag
{ UC presenceFlag;
  UC title  [31];
  UC artist [31];
  UC album  [31];
  UC year   [ 5];
  UC comment[31];
  UC  flag;
  UC track;
  UC genre;
} id3v1Tag_t;

UC get_id3v1Tag_presenceFlag(mp3File_t *);
VD check_id3v1Tag_presence  (mp3File_t *);
UC retrieve_id3v1Tag        (mp3File_t *);
UC print_id3v1Tag           (mp3File_t *);

#endif // ID3V1TAG_
