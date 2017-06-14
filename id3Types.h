// ID3V1_
/*
header 	        3 	"TAG"
title 	       30 	30 characters of the title
artist 	       30 	30 characters of the artist name
album 	       30 	30 characters of the album name
year 	        4 	A four-digit year
comment 	28   or 30 	The comment.
zero-byte    	1 	If a track number is stored, this byte contains a binary 0.
track    	1 	The number of the track on the album, or 0. Invalid, if previous byte is not a binary 0.
genre 	1 	Index in a list of genres, or 255
*/
typedef struct id3v1Tag
{ UC presenceFlag;
  char title  [31];
  char artist [31];
  char album  [31];
  char year   [ 5];
  char comment[31];
  UC  flag;
  UC track;
  UC genre;
} id3v1Tag_t;

// ID3V1_XTD_
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
  char title     [61];
  char artist    [61];
  char album     [61];
  char speed     [ 1];
  char start_time[ 7];
  char   end_time[ 7];

} id3v1_xtdTag_t;

// ID3V2TAG_FRAME_
/*
  All the frames consist of a frame header followed by one or more fields containing the actual information. The layout of the frame header:

  Frame ID       $xx xx xx xx (four characters)
  Size           $xx xx xx xx
  Flags          $xx xx
*/
/*
4> frame flags
    v2.3			|  v2.4
    %abc00000 %ijk00000		|  %0abc0000 %0h00kmnp
				|
a - Tag alter preservation	|  a - Tag alter preservation
b - File alter preservation	|  b - File alter preservation
c - Read only			|  c - Read only
				|
i - Compression			|
j - Encryption			|  h - Grouping identity
k - Grouping identity		|
				|  k - Compression
				|  m - Encryption
  				|  n - Unsynchronisation
				|  p - Data length indicator
////////////
newflags = flags & 0x00E0        | ?? &0x004D ??  -- &0x004C --

we disregard flags a, b anc
we keep compression, encryption and grouping
///////////
*/
typedef struct frame_info
{
  UC compression_flag;   U4 decompressedSize;
  UC encryption_flag;    UC encryptionMethod;
  UC grouping_flag;      UC groupID;
  UC unsync_flag;                       // id3v2.4 only  // <== unused since we store frames re-synched
  UC dataLength_flag;    U4 dataLength; // id3v2.4 only
  UC bytesSkipped;
} frame_info_t;

typedef struct id3v2Tag_frameLink // <=== We will usually refer to a frameLink as a frame ???
{
  U4   frameID;       
  char   *data;
  U4  dataSize; // the size of the content; does not include the header
  frame_info_t *info;

  id3v2Tag_frameLink_t  *next;
} id3v2Tag_frameLink_t;

// ID3V2TAG_FRAMELIST_
typedef struct id3v2Tag_frameList
{
  U4                       count;
  id3v2Tag_frameLink_t    *first;
  id3v2Tag_frameLink_t    * last;
  id3v2Tag_frameLink_t **ptrList;
} id3v2Tag_frameList_t;
// ID3V2TAG_FRAME_V3_
typedef struct id3v2Tag_frame_select
{
  U4 frameID;
  char *frameID_str;
  char *description;
} id3v2Tag_frame_select_;

// ID3V2TAG_
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

// ID3V2TAG_HEADER_
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
