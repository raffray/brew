#ifndef ID3V2TAG_HEADER_
#define ID3V2TAG_HEADER_

#include "includes.h"
#include "brew_types.h"

  /* *** 10-byte header ***

     ID3v2/file identifier      $49 44 33  | ID3
     ID3v2 version              $0x yy     | yy is not $FF 
     ID3v2 flags                %abcd0000  
     ID3v2 size             4 * %0xxxxxxx

   a - Unsynchronisation
   b - Extended header
   c - Experimental indicator
   d - Footer present         ---  ONLY in id3v2.4
  */

typedef struct id3v2Tag_header
{
  UC presenceFlag;

  UC version;
  UC revision;

  /*    FLAGS SECTION */
  UC unsync_presenceFlag;
  UC xtdHdr_presenceFlag; // eXtended Header
  U4 xtdHdr_size;
  UC exp_presenceFlag; // experimentalFlag <== do we handle and try and parse?
  //                                           or do we ignore the whole tag?
  SC footer_presenceFlag; // id3v2.4 only
  /* END FLAGS SECTION */
} id3v2Tag_header_t;

  /* Extended header
     Id3v2.3
     Extended header size   $xx xx xx xx
     Extended Flags         $xx xx
     Size of padding        $xx xx xx xx
     Where the 'Extended header size', currently 6 or 10 bytes, EXCLUDES ITSELF. (if flags indicate CRC, 4 bytes are appended to the extended header.

     Id3v2.4
     Extended header size   4 * %0xxxxxxx
     Number of flag bytes       $01
     Extended Flags             $xx
     Where the 'Extended header size' is the size of the whole extended
     header, stored as a 32 bit synchsafe integer. An extended header can
     thus never have a size of fewer than 6 bytes.             INCLUDES ITSELF
  */


/*
void set_id3v2Tag_presenceFlag(mp3File_t *file, UC flag);
void set_id3v2Tag_version     (mp3File_t *file, UC nb);
void set_id3v2Tag_subVersion  (mp3File_t *file, UC nb);
void set_id3v2Tag_unsync_presenceFlag  (mp3File_t *file, UC flag);
void set_id3v2Tag_xtdHdr_presenceFlag  (mp3File_t *file, UC flag);
void set_id3v2Tag_xtdHdr_size  (mp3File_t *file, U4 size);
void set_id3v2Tag_exp_presenceFlag  (mp3File_t *file, UC flag);
void set_id3v2Tag_footer_presenceFlag  (mp3File_t *file, UC flag);
*/

UC   get_id3v2Tag_presenceFlag(mp3File_t *file);
UC   get_id3v2Tag_version     (mp3File_t *file);
UC   get_id3v2Tag_revision  (mp3File_t *file);
UC   get_id3v2Tag_unsync_presenceFlag  (mp3File_t *file);
UC   get_id3v2Tag_xtdHdr_presenceFlag  (mp3File_t *file);
U4   get_id3v2Tag_xtdHdr_size  (mp3File_t *file);
//UC   get_id3v2Tag_exp_presenceFlag  (mp3File_t *file);
//UC   get_id3v2Tag_footer_presenceFlag  (mp3File_t *file);
/**/

void set_id3v2Tag_info(mp3File_t *file, UC version, UC subVer, UC unsync, UC xtdHdr, U4 xtdHdr_size, UC exp, UC footer);

UC check_id3v2Tag_presence(mp3File_t *);
UC retrieve_id3v2Tag_header(mp3File_t *);

#endif // ID3V2TAG_HEADER_

