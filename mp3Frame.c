#include "includes.h"
#include "extern.h"
#include "mp3Frame.h"
#include "vbrFrame.h"
#include "mp3File.h"
#include "byte.h"
#include "mfile.h"
#include "err.h"
#include "prints.h"
#include "mp3Frame_marker_list.h"
#include "mp3Frame_chain_list.h"
#include "id3v1_xtdTag.h"
#include "id3v1Tag.h"

#include <math.h>
#include <stdio.h>


//U4 vbr_type;
UC version_idx[4] = { 3, 0, 2, 1}; // 3<->v2.5  version==0 --> mpg v2.5/ reserved / v2 / v1
UC   layer_idx[4] = { 0, 3, 2, 1}; // reserved/l3,l2,l1
//                                    as such, mpeg 1 "11",    -- layer 3 "01" is properly set
//                                         version_idx[3] = 1  -- layer_idx[1] = 3

U2 bitRate_idx[3][3][15] = { { {0,32,64,96,128,160,192,224,256,288,320,352,384,416,448},      // V,L = 1,1  idx = 3(x-1)+ (y-1) = 0
			       {0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384},      // V,L = 1,2        3(  0)+ (  1) = 1
			       {0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320} },    // V,L = 1,3                        2

			     { {0,32,48,56, 64, 80, 96, 112,128,144,160,176,192,224,256},     // V,L = 2,1                        3
			       {0, 8,16,24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160},      // V,L = 2,2                        4
			       {0, 8,16,24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160} },    // V,L = 2,3                        5

			     { {0,32,48,56, 64, 80, 96, 112,128,144,160,176,192,224,256},     // V,L = 3,1                        6
			       {0, 8,16,24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160},      // V,L = 3,2                        7
			       {0, 8,16,24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160} } };  // V,L = 3,3                        8
/*
U4 samplingRate_idx[3][4] = { {44100,48000,32000,0},  // mpeg_v1   or version_idx[3]
			      {22050,24000,16000,0},  // mpeg_v2   or version_idx[2]
			      {11025,12000, 8000,0} };// mpeg_v2.5 or version_idx[0]
*/
U2 samplingRate_idx[3][3] = { {44100,48000,32000},  // mpeg_v1   or version_idx[3]  not depending on layer but on smapling_idx 0/1/2
			      {22050,24000,16000},  // mpeg_v2   or version_idx[2]
			      {11025,12000, 8000} };// mpeg_v2.5 or version_idx[0]

U2 samplesPerFrame_idx[3][3] =  { { 384, 1152, 1152 },   // V1 l1/2/3
				  { 384, 1152, 576  },   // V2 l1/2/3
				  { 384, 1152, 576  } }; // V3 l1/2/3

UC getMp3Header(mp3File_t *file, mp3Header_t *hdr, U4 *header)
{ TRY(mreadU4(file, header) , {})
  mseek(file, -4, SEEK_CUR);
  return setMp3Header(hdr, *header); }
/*
#define getVersionId_(   header)  ((header & 0x00180000) >> 19) // .... .... ...1 1... .... .... .... ....
#define getLayer_(       header)  ((header & 0x00060000) >> 17) // .... .... .... .11. .... .... .... ....
#define getBitRate_(     header)  ((header & 0x0000F000) >> 12) // .... .... .... .... 1111 .... .... ....
#define getSamplingRate_(header)  ((header & 0x00000C00) >> 10) // .... .... .... .... .... 11.. .... ....
#define getPadding_(     header)  ((header & 0x00000200) >>  9) // .... .... .... .... .... ..1. .... ....
#define getChannelMode_( header)  ((header & 0x000000C0) >>  6) // .... .... .... .... .... .... 11.. ....
*/
UC setMp3Header(mp3Header_t *hdr, U4 val)
{
  hdr->hdr32 = val;

 if(!isMp3Header(val))  return FAILURE;
  hdr->versionId    = version_idx[ getVersionId(val) ];
  hdr->layer        = layer_idx  [ getLayer    (val) ];
  hdr->bitRate      = bitRate_idx[ hdr->versionId -1 ] [ hdr->layer -1 ] [ getBitRate(val) ];
  hdr->samplingRate = samplingRate_idx[ hdr->versionId -1] [ getSamplingRate(val) ] ;
  hdr->paddingBit   = getPadding(val);
  hdr->channelMode  = getChannelMode(val);

  if       (hdr->layer == 1)      hdr->frameSize = 4*floor(( 12000.0 * hdr->bitRate)/hdr->samplingRate + hdr->paddingBit);
  else if  (hdr->layer == 2)      hdr->frameSize =   floor((144000.0 * hdr->bitRate)/hdr->samplingRate + hdr->paddingBit);
  else if  (hdr->versionId == 1)  hdr->frameSize =   floor((144000.0 * hdr->bitRate)/hdr->samplingRate + hdr->paddingBit);
  else                            hdr->frameSize =   floor(( 72000.0 * hdr->bitRate)/hdr->samplingRate + hdr->paddingBit);

  hdr->length  = (FL) samplesPerFrame_idx [ hdr->versionId -1] [ hdr->layer - 1 ] / hdr->samplingRate;

  return SUCCESS;
}
/* // <==== (cf. Kahan summation algorithm for a solution)

Issue: hdr->length, no matter its representation introduces an error.
       floats are not acceptable
       doubles are ok.
When computing the total track length, adding each frame's length...
For example, mpeg 1, layer 3, 44100Hz : frame's length is 1152/44100 = 0.026122... no exact value
Theoretically, each frame can have any combination of version/layer/sampling-rate
if all frames have same specs, multiplying frame_count by frame_length is ok;
                               adding each frame_length, accumulating one by one isn't so great
BUT, in theory, we don't know

other idea, count the number of frames of each type/combo ( length_count(1152,44100)++ )
 */


UC isMp3Header(U4 header)
{ if ( (header & 0xFFE00000) != 0xFFE00000) return false;
  if ( (header & 0x00180000) == 0x00080000) return false;
  if ( (header & 0x00060000) == 0x00000000) return false;
  if ( (header & 0x0000F000) == 0x0000F000) return false;
  if ( (header & 0x0000F000) == 0x00000000) return false;
  if ( (header & 0x00000C00) == 0x00000C00) return false;

  return true; }

//U4 getFrameSync   (U4 header)   { return ( header & 0xFFE00000);       } // 1111 1111 111. .... .... .... .... ....
UC getVersionId   (U4 header)     { return ( header & 0x00180000) >> 19; } // .... .... ...1 1... .... .... .... ....
UC getLayer       (U4 header)     { return ( header & 0x00060000) >> 17; } // .... .... .... .11. .... .... .... ....
//U4 getProtectionBit(U4 header)  { return ( header & 0x00010000);       } // .... .... .... ...1 .... .... .... ....
U2 getBitRate     (U4 header)     { return ( header & 0x0000F000) >> 12; } // .... .... .... .... 1111 .... .... ....
UC getSamplingRate(U4 header)     { return ( header & 0x00000C00) >> 10; } // .... .... .... .... .... 11.. .... ....
UC getPadding     (U4 header)     { return ( header & 0x00000200) >>  9; } // .... .... .... .... .... ..1. .... ....
//U4 getPrivateBit(U4 header)     { return ( header & 0x00000100);       } // .... .... .... .... .... ...1 .... ....
UC getChannelMode (U4 header)     { return ( header & 0x000000C0) >>  6; } // .... .... .... .... .... .... 11.. ....
//UC getModeExtension(U4 header)  { return ( header & 0x00000030);       } // .... .... .... .... .... .... ..11 ....
//UC getCopyright (U4 header)     { return ( header & 0x00000008);       } // .... .... .... .... .... .... .... 1...
//UC getOriginal  (U4 header)     { return ( header & 0x00000004);       } // .... .... .... .... .... .... .... .1..
//UC getEmpasis   (U4 header)     { return ( header & 0x00000003);       } // .... .... .... .... .... .... .... ..11


VD printMp3Header(mp3Header_t *hdr)
{ //if(isMp3HeaderU4(hdr->hdr32)) // <=== ???
    { fprintf(ostream, "=>Header : ");   printU4(hdr->hdr32, NL    );
      if   (hdr->versionId == 3) fprintf(ostream, "  mpg  2.5\n");
      else                       fprintf(ostream, "  mpg   %d\n"          , hdr->versionId    );
      fprintf(ostream, "  layer %d\n"          , hdr->layer        );
      fprintf(ostream, "  bitrate       : %d\n", hdr->bitRate      );
      fprintf(ostream, "  sampling rate : %d\n", hdr->samplingRate );
      fprintf(ostream, "  padding bit   : %d\n", hdr->paddingBit   );
      switch( hdr->channelMode)
	{case 0 : fprintf(ostream, "  channel mode  : Stereo\n");       break;
	case  1 : fprintf(ostream, "  channel mode  : Joint Stereo\n"); break;
	case  2 : fprintf(ostream, "  channel mode  : Dual Channel\n"); break;
	case  3 : fprintf(ostream, "  channel mode  : Single Channel\n");         break;
	default :                                                       break; }
      fprintf(ostream, "  frameSize     : %"PRId32" = 0x",  hdr->frameSize); printU4(hdr->frameSize, NL);
      fprintf(ostream, "  frame Duration: %fms\n",   1000.0*hdr->length); }
}


VD printMp3Header_line(mp3Header_t *hdr)
{ //if(isMp3HeaderU4(hdr->hdr32)) {
  fprintf(ostream, " * ");
  fprintf(ostream, "Mpeg %d, layer %d -- %5d Hz -- %4d kbps -- ", hdr->versionId, hdr->layer, hdr->samplingRate, hdr->bitRate);
  switch( hdr->channelMode)
    {case 0 : fprintf(ostream,   "Stereo         -- "); break;
    case  1 : fprintf(ostream,   "Joint Stereo   -- "); break;
    case  2 : fprintf(ostream,   "Dual Channel   -- "); break;
    case  3 : fprintf(ostream,   "Single Channel -- "  ); break;
    default :                                           break; }
  fprintf(ostream, "%4.3fms -- frameSize = %4d = ",   1000.0*hdr->length, hdr->frameSize); printU2(hdr->frameSize, NL);
}

VD printMp3Header_line_short(mp3Header_t *hdr)
{ fprintf(ostream, " * ");
  fprintf(ostream, "Mpeg %d, layer %d -- %5d Hz -- ", hdr->versionId, hdr->layer, hdr->samplingRate);
  switch( hdr->channelMode)
    {case 0 : fprintf(ostream,             "Stereo\n"); break;
    case  1 : fprintf(ostream,       "Joint Stereo\n"); break;
    case  2 : fprintf(ostream,       "Dual Channel\n"); break;
    case  3 : fprintf(ostream,     "Single Channel\n"); break;
    default :                                           break; }
}


UC equivHeaders_vlsc_U4(U4 header1,        U4 header2)   { return ( (header1 & 0x001E0CC0) == (header2 & 0x001E0CC0)); }
UC equivHeaders_vlsc   (mp3Header_t* hdr1, U4 header2)   { return equivHeaders_vlsc_U4(hdr1->hdr32, header2); }
