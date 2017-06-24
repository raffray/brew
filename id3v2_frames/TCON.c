#include <ctype.h>
#include "includes.h"
#include "Txyz.h"
#include "id3v2Tag_frame.h"
#include "mp3File.h"
#include "extern.h"
#include "byte.h"
#include "err.h"
#include "text-encoding/utf-8.h"
#include "str.h"

/* TCON : Content Type

    <Header for 'Text information frame', ID: "T000" - "TZZZ", excluding "TXXX"
    Text encoding    $xx
    Information    <text string according to encoding>

// id3v2.3 specs
   The 'Content type', which previously was stored as a one byte numeric value only, is now a numeric string.
   You may use one or several of the types as ID3v1.1 did or, since the category list would be impossible to
   maintain with accurate and up to date categories, define your own.

    References to the ID3v1 genres can be made by, as first byte, enter "(" followed by a number from the genres
    list (appendix A) and ended with a ")" character. This is optionally followed by a refinement, e.g. "(21)" or
    "(4)Eurodisco". Several references can be made in the same frame, e.g. "(51)(39)". If the refinement should
    begin with a "(" character it should be replaced with "((", e.g. "((I can figure out any genre)"
    or "(55)((I think...)". The following new content types is defined in ID3v2 and is implemented in the same way
    as the numeric content types, e.g. "(RX)".

    RX    Remix
    CR    Cover

  // the stored string will either be like: "Comedy"
  // in which case we look up the string in tconArray
  // and use it's index to print: "TCON: Comedy (57)"
  // OR
  // string is like "(57)" and we print the string at index 57, which is "Comedy"
  // OR
  // since v2.4 I think, like "57"... may take care of that later

*/

// 22 is the maximum length of strings. for: 140. Contemporary Christian
UC tconArray[256][22]={
					 "Blues"                 , "Classic Rock"         , "Country"          , "Dance"           , "Disco"          ,
		       "Funk"                  , "Grunge"               , "Hip-Hop"          , "Jazz"            , "Metal"          ,
		       "New Age"               , "Oldies"               , "Other"            , "Pop"             , "R&B"            ,
		       "Rap"                   , "Reggae"               , "Rock"             , "Techno"          , "Industrial"     ,
		       "Alternative"           , "Ska"                  , "Death Metal"      , "Pranks"          , "Soundtrack"     ,
		       "Euro-Techno"           , "Ambient"              , "Trip-Hop"         , "Vocal"           , "Jazz+Funk"      ,
		       "Fusion"                , "Trance"               , "Classical"        , "Instrumental"    , "Acid"           ,
		       "House"                 , "Game"                 , "Sound Clip"       , "Gospel"          , "Noise"          ,
		       "AlternRock"            , "Bass"                 , "Soul"             , "Punk"            , "Space"          ,
		       "Meditative"            , "Instrumental Pop"     , "Instrumental Rock", "Ethnic"          , "Gothic"         ,
		       "Darkwave"              , "Techno-Industrial"    , "Electronic"       , "Pop-Folk"        , "Eurodance"      ,
		       "Dream"                 , "Southern Rock"        , "Comedy"           , "Cult"            , "Gangsta"        ,
		       "Top 40"                , "Christian Rap"        , "Pop/Funk"         , "Jungle"          , "Native American",
		       "Cabaret"               , "New Wave"             , "Psychadelic"      , "Rave"            , "Showtunes"      ,
		       "Trailer"               , "Lo-Fi"                , "Tribal"           , "Acid Punk"       , "Acid Jazz"      ,
		       "Polka"                 , "Retro"                , "Musical"          , "Rock & Roll"     , "Hard Rock"      ,
		       "Folk"                  , "Folk-Rock"            , "National Folk"    , "Swing"           , "Fast Fusion"    ,
		       "Bebob"                 , "Latin"                , "Revival"          , "Celtic"          , "Bluegrass"      ,
		       "Avantgarde"            , "Gothic Rock"          , "Progressive Rock" , "Psychedelic Rock", "Symphonic Rock" ,
		       "Slow Rock"             , "Big Band"             , "Chorus"           , "Easy Listening"  , "Acoustic"       ,
		       "Humour"                , "Speech"               , "Chanson"          , "Opera"           , "Chamber Music"  ,
		       "Sonata"                , "Symphony"             , "Booty Bass"       , "Primus"          , "Porn Groove"    ,
		       "Satire"                , "Slow Jam"             , "Club"             , "Tango"           , "Samba"          ,
		       "Folklore"              , "Ballad"               , "Power Ballad"     , "Rhythmic Soul"   , "Freestyle"      ,
		       "Duet"                  , "Punk Rock"            , "Drum Solo"        , "A capella"       , "Euro-House"     ,
		       "Dance Hall"            , "Goa"                  , "Drum & Bass"      , "Club-House"      , "Hardcore"       ,
		       "Terror"                , "Indie"                , "BritPop"          , "Afro-Punk"       , "Polsk Punk"     ,
		       "Beat"                  , "Christian Gangsta Rap", "Heavy Metal"      , "Black Metal"     , "Crossover"      ,
		       "Contemporary Christian", "Christian Rock"       , "Merengue"         , "Salsa"           , "Thrash Metal"   ,
		       "Anime"                 , "JPop"                 , "Synthpop"         , "Abstract"        , "Art Rock"       ,
		       "Baroque"               , "Bhangra"              , "Big Beat"         , "Breakbeat"       , "Chillout"       ,
		       "Downtempo"             , "Dub"                  , "EBM"              , "Eclectic"        , "Electro"        ,
		       "Electroclash"          , "Emo"                  , "Experimental"     , "Garage"          , "Global"         ,
		       "IDM"                   , "Illbient"             , "Industro-Goth"    , "Jam Band"        , "Krautrock"      ,
		       "Leftfield"             , "Lounge"               , "Math Rock"        , "New Romantic"    , "Nu-Breakz"      ,
		       "Post-Punk"             , "Post-Rock"            , "Psytrance"        , "Shoegaze"        , "Space Rock"     ,
		       "Trop Rock"             , "World Music"          , "Neoclassical"     , "Audiobook"       , "Audio Theatre"  ,
		       "Neue Deutsche Welle"   , "Podcast"              , "Indie Rock"       , "G-Funk"          , "Dubstep"        ,
		       "Garage Rock"           , "Psybient"             , "None"             , "None"            , "None"           ,
		       "None"                  , "None"                 , "None"             , "None"            , "None"           ,
		       "None"                  , "None"                 , "None"             , "None"            , "None"           ,
		       "None"                  , "None"                 , "None"             , "None"            , "None"           ,
		       "None"                  , "None"                 , "None"             , "None"            , "None"           ,
		       "None"                  , "None"                 , "None"             , "None"            , "None"           ,
		       "None"                  , "None"                 , "None"             , "None"            , "None"           ,
		       "None"                  , "None"                 , "None"             , "None"            , "None"           ,
		       "None"                  , "None"                 , "None"             , "None"            , "None"           ,
		       "None"                  , "None"                 , "None"             , "None"            , "None"           ,
		       "None"                  , "None"                 , "None"             , "None"            , "None"           ,
		       "None"                  , "None"                 , "None"             , "None"            , "None"           ,
		       "None"                  , "None"                 , "None"             , "None"            , "None"           ,
		       "None"};

// 'CR' = Cover // not supported
// 'RX' = Remix // not supported

//buffer_t *string_to_utf8_str(char *str, U4 len, UC type_known, UC type_given); // <====

void print_TCON2(mp3File_t *file, U4 frameNb) // mseek already performed by print_Txxx_frame
{ buffer_t *buffer = get_id3v2Tag_frame_buffer    (file, frameNb);
//  char     *data = get_id3v2Tag_frame_data    (file, frameNb);
//  U4    dataSize = get_id3v2Tag_frame_dataSize(file, frameNb);

  print_string_buf(buffer, false, 0);
}

void print_TCON(mp3File_t *file, U4 frameNb) // mseek already performed by print_Txxx_frame
{ UC                  *data = get_id3v2Tag_frame_data    (file, frameNb);
  U4                 dataSize = get_id3v2Tag_frame_dataSize(file, frameNb);

//  buffer_t *buffer = string_to_utf8_str(data, dataSize, false, 0);

  /*
  // id3v2.3 examples
  (55)
  (31)(27)
  (31)My_refinement
  (23)My_refinement(11)Another_refinement
  (27)((My_refinement)(57)Another_refinement
  (19)((My_refinement)(12)(44)Another_refinement(37)((One_more_refinement)

  //idv2.4
  '00' separated strings
  21 \0 22
  21 \0 my genre \0
  */

  // string will be a sequence of
  print_string(data, dataSize, false, 0); //printf("%s\n", buffer->data);

  //free(buffer);

    /*
  frame_t *frame   = get_tag_frame(file, frameNb);
  U4 endFrame_offset = frame->start_offset + frame->size;
  U2 i;
  U2 v = 256;
  UC c;




  mseek(file, frame->start_offset, SEEK_SET);
  mreadUC(file, &c);
  getString(file, c, endFrame_offset); // We've grabbed 'Str'

  if( (idx>2) && (Str[0]=='(') && (Str[idx-1]==')') )
    {
      switch (idx)
	{
	case 3: if(isdigit(Str[1]))                                       v =      Str[1]-0x30;                                     break;
	case 4: if(isdigit(Str[1]) && isdigit(Str[2]))                    v =  10*(Str[1]-0x30) +    (Str[2]-0x30);                 break;
	case 5: if(isdigit(Str[1]) && isdigit(Str[2]) && isdigit(Str[3])) v = 100*(Str[1]-0x30) + 10*(Str[2]-0x30) + (Str[3]-0x30); break;
	default : break;
	}
      if (v<256) fprintf(ostream, "%s %s", tconArray[v], Str);
      else       fprintf(ostream, "%s (255)", Str); // fprintf(ostream, "Unknown %s\n",          Str);
    }
  else
    {
      for (i=0; i<256; i++)
	if ( strcmp((const char*)tconArray[i], (const char*)Str) == 0 )
	  { fprintf(ostream, "%s (%d)", Str, i); i=257; }

      if(i==256)
	fprintf(ostream, "%s (255)", Str);
    }

  free(Str);
    */
}
