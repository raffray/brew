#include "includes.h"
#include "extern.h"
#include "mp3Stream.h"
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

#include "length_map.h"

#include <math.h>
#include <stdio.h>

#define MIN_CHAIN_LEN 7

// to compute the avg bitrate, sum all bitrates and divide by #frames
// wouldn't work in case like: #frames = 0xFFFFFFFF
// USELESS THEN --> U4 bitRate_map[3][3][15]; // counts the number of frames of each bitrate available
// use memset to set it to 0s

// hdr->length  = (FL) samplesPerFrame_idx [ hdr->versionId -1] [ hdr->layer - 1 ] / hdr->samplingRate;
// to remedy the accumulation of errors in frame_length, we can "simply" count the number of each frame type
// and make only o<ne division for each type
//U4 length_map[3][3][3];
/*
float length = 0;
for(i=0;i<3;i++) // version       --- hdr->versionId -1
for(j=0;j<3;j++) // layer         --- hdr->layer -1
for(k=0;k<3;k++) // samplingRate  --- getSamplingRate(hdr->hdr32)
{
 sr = samplingRate_idx[i][k] ;
 l  = (FL) samplesPerFrame_idx [i][j] / sr;
length += length_map[i,j,k]) * l;
}
 */

// knowing the segment's length and byteCount, we can deduce the bitrate (unless we already determined it was constant)
// (average bitrate /8) = byteCount/length

// therefore, each marker_list should have it's length_map
// each chain too. The length_map of a chain is simply the sum of it's markers' maps
//                 The length_map of the stream is     the sum of it's chains'  maps

// OR track_pos = byte_size/ (average bitrate /8)
//              = byte_size/ ((sum_bitRates>>4)/frameCount)
// the only problem is sum_bitRates; in extrem cases, on a 32bit system, we can have an overflow: 0xFFFFFFFF frames at Xkbps... BOOM

U4 vbr_type;

U4    get_stream_chainCount  (mp3File_t *file               )   { return (file->chain_list).count;            }
void  set_stream_start_offset(mp3File_t *file, U4 offset    )   {         file->stream_start_offset = offset; }
U4    get_stream_start_offset(mp3File_t *file               )   { return  file->stream_start_offset;          }
void  set_stream_end_offset  (mp3File_t *file, U4 offset    )   {         file->stream_end_offset   = offset; }
U4    get_stream_end_offset  (mp3File_t *file               )   { return  file->stream_end_offset;            }
//void  set_stream_frameCount  (mp3File_t *file, U4 frameCount)   {         file->frameCount = frameCount;      }
U4    get_stream_frameCount  (mp3File_t *file               )   { return  file->map.frameCount;                   }
void  set_stream_byteCount   (mp3File_t *file, U4  byteCount)   {         file-> byteCount =  byteCount;      } // <==  if data is corrupt: DIFFERENT FROM end_offest - start_offset
U4    get_stream_byteCount   (mp3File_t *file               )   { return  file-> byteCount;                   }
void  set_stream_isVbr       (mp3File_t *file, UC isVbr) {        file->isVbr = isVbr;  }
UC    get_stream_isVbr       (mp3File_t *file)           { return file->isVbr;          }
void  set_stream_bitRate     (mp3File_t *file, U2 bitRate    ) {  file->bitRate    = bitRate; }
U2    get_stream_bitRate     (mp3File_t *file)          { return file->bitRate;    }
void  set_stream_isVlength   (mp3File_t *file, UC isVlength) {    file->isVlength = isVlength;  }
UC    get_stream_isVlength   (mp3File_t *file)           { return file->isVlength;          }
//void  set_stream_length      (mp3File_t *file, FL length    ) {  file->length     = length; }
FL    get_stream_length      (mp3File_t *file)          { return file->map.length ;    }


UC findHeader(mp3File_t *file)
{ U4 header;

  while ( true )
    { TRY(mreadU4(file,   &header) , {})
      mseek(file, -4,   SEEK_CUR);
      if(isMp3Header(header))   return SUCCESS;
      mseek(file,  1,   SEEK_CUR); }
}

UC findChain(mp3File_t *file, U4 *offset) // returns the offset to the first chain (with frameCount > MIN_CHAIN_LEN)
{ U4   frameCount = 0;
  U4     fileSize = get_fileSize(file);
  U4       header;
  mp3Header_t hdr;
  U4 minimum_chain_length = MIN_CHAIN_LEN;

  if(get_vbrFrame_flag(file))   minimum_chain_length--;

  *offset = mtell(file);

  getMp3Header(file, &hdr, &header);

  while( true )
    { if ( frameCount > minimum_chain_length)           return SUCCESS;
      if ( mtell(file) + hdr.frameSize <= fileSize  )   frameCount++;
      if ( mtell(file) + hdr.frameSize >  fileSize-4)   return FAILURE;
      mseek(     file,   hdr.frameSize, SEEK_CUR);
      TRY( getMp3Header(file,  &hdr, &header) , {}) }
}

UC addChain(mp3File_t *file)
{ U4    start_offset;

  TRY (findHeader(file) , {})
   if (findChain (file, &start_offset) ==                    FAILURE)
     { mseek     (file,  start_offset+1, SEEK_SET);   return FAILURE;  }
  mseek(file, start_offset, SEEK_SET);
  retrieveChain(file);                                return SUCCESS;
}

// <== code is cleaner than before but, eventhough we only pass through chains once, the prog is slower.
UC retrieveChain(mp3File_t *file) // only called after foundChain was successful, SO we will NOT allocate structures for nothing
{ U4            fileSize = get_fileSize(file);
  U4       currentHeader, previousHeader;
  mp3Header_t currentHdr;
  U4        previousAddr;
  U4         currentAddr = 0;
  U4            nextAddr = 0;
  UC           isVbr;
  U2   first_bitRate;

  UC    isVlength; // is there a change in length? if so, no vbr frame can be created.
  FL first_length; // if isVlen == false, this is the length of every frame in the chain

  UC   exitType = 0;

  U4 end_offset;
  U4 first_header;

  chain_list_t  *chain_list = &file->chain_list;
  chain_t       *chain;
  marker_t      *marker;

  chain = push_chain(chain_list); // <== do not call directly but on "file" only
  set_chain_start_offset(chain, mtell(file));

  getMp3Header(file, &currentHdr, &currentHeader);
  first_header = currentHeader;

  isVbr     = false;   first_bitRate  = currentHdr.bitRate;
  isVlength = false;   first_length   = currentHdr.length;
  set_chain_first_bitRate(chain, first_bitRate);
  set_chain_first_length (chain, first_length);

  marker = push_marker(&chain->markers, mtell(file), currentHeader);
  add_value_to_map(&marker->map, &currentHdr);

  while( true )
    {
      previousAddr = currentAddr;   currentAddr      = mtell(file);
      nextAddr     = currentAddr  + currentHdr.frameSize;
      previousHeader = currentHeader;

      if (nextAddr > fileSize)	                                   { exitType = 1; break; }
      if (nextAddr > fileSize-4)                                                   break;
      mseek(file, currentHdr.frameSize, SEEK_CUR);

      if( getMp3Header(file, &currentHdr, &currentHeader) ==  FAILURE)             break; //printU4(mtell(file), NL);//printMp3Header(&currentHdr); printf("\n");

      if( !equivHeaders_vlsc(&currentHdr, previousHeader) )
	{ marker = push_marker(&chain->markers, mtell(file), currentHeader); }
      if( mtell(file)       + currentHdr.frameSize <= fileSize)
	{  add_value_to_map(&marker->map, &currentHdr);
	  if(currentHdr.bitRate     != first_bitRate)   isVbr     = true;
	  if(currentHdr.length      != first_length )   isVlength = true;  /* printf("--> %d %.10f %.10f\n", i++, currentHdr.length, chain_length);*/}
    }

  if (exitType == 1)  { set_chain_lastFrame_offset(chain, previousAddr);   end_offset = currentAddr; set_chain_end_offset(chain, currentAddr); }
  else                { set_chain_lastFrame_offset(chain,  currentAddr);   end_offset =    nextAddr; set_chain_end_offset(chain,    nextAddr); }

  marker = push_marker(&chain->markers, end_offset, currentHeader); // <=== is currentHeader valid
  set_chain_info(chain, first_header, isVbr, isVlength);

  create_marker_ptrList(&chain->markers); // <=== _lastChain_markerList(file)

  return SUCCESS;
}

// SPLIT mp3Header.c into ... mp3Header.c, mp3Data.c and maybe printReports.c??


// might want to collect data before printing it; only go through chains once. After they have been gathered?
// except maybe in case we only want the "Estimated playable track time"?
//
// Build a function that checks that everything is alright and only prints info
// when something is wrong, including:
//                                                 * id3 frame reaches beyond dataStart_offset??? <=== not in a first step
// * cbr has a vbr header
// * vbr has bad or no vbr header
// * there is more than one chain
// * frame spec changes (even in same chain)
// * data @end of mp3 data? (id3v1, lyrics, junk?)

/*
Algo
compute bitrate + cbr or vbr
 --> go through every frame.
       no vbr-frame <--> CBR, use first frame bitrate
          vbr-frame <--> VBR, use   VBR frame info to compute bitrate
        |_______________>   8*datasize/(frameCount*vbrFrameDuration)

Created new file by concatenating 2 with different specs as follow:
*
First file : Mpeg 1, layer 3 -- 48000 Hz -- CBR  64 kbps -- Single Channel --  24.000ms -- 216 Frames -- dataSize =  41472
2nd   file : Mpeg 1, layer 3 -- 44100 Hz -- CBR 128 kbps -- Joint Stereo   --  26.122ms -- 317 Frames -- dataSize = 132493
Combined   :                                VBR 102 kbps                                   533 Frames -- dataSize = 173965  as I report it
vbrfix-ed
VBR Frame  :                                                                                 1 Frame  -- dataSize =    417
TOTAL      :                                VBR 102 kbps                                   534 Frames -- dataSize = 174382
(  /mnt/EximpTags/test/Xing-diff-count/All files/j/samples/variableDur/compl_sin1k0db-out.mp3  )

8*174382 / (534*26.122) = 100 --> with vbrFrame created by vbrFix (vbrHeader is 0xFF FB 90 60)

It is calculated wrong; as it should.
For a file with frames of variable duration ---> We cannot create a vbr frame with a duration corresponding to what the real vbr bitrate would be.
In this example: duration = 8*174382/(102*534) = 25.06 ms
A stream with variation of frame specs (and particularly frame Durations) is a suspicious file for which a vbrFrame makes no sense!!!

We do not estimate anything though.
We then go through every single frame.
DOUBLE-CHECK --> do we create a new chain every time a change in spec is found? (case where 2 such changes follow each other with no gap)
if specs change, length does not necessarily change. (for example, maybe only the channelMode changes)

So, we determine if file is CBR or VBR, and calculate the bitrate

if CBR but length isn't? (we can create a file where, for example, frames alternate in mpeg version/layer/sample-rate.)


WARNING, in such a case, equivHeaders would fail finding chains eventhough file is composed of well positionned frames!!!!
         I KNOW it's a stupid example we would never find because no proper software would create such a file.
         But I and mp3Brew are both paranoid :D

Yet, it wouldn't change anything, vbrFrame is not concerned here!!



<=== !!!! What do we do for a chain composed of mixed-spec headers?
* For example, mp3val reports a valid chain, doesn't look into header specs.

!!!!!!!!!!!!!!!!
OPTION 1: chains are broken up into blocks of lenght > min_length with same frame specs.

OPTION 2: we get chains of following headers, regardless of specs
          THEN we break them up into "sub-chains" of  equal specs
	  This solution would allow us to detect changes of specs for "sub-chains" of small length ( < MIN)
!!!!!!!!!!!!!!!!

Chain #1... zero data loss, only spec changes
Start offset   -     End offset        #frames   track position   block length    corrupt data
00 00 .. ..    -    00 00 04 F5     VBR frame             0"000          0"026                      Mpeg 1, layer 2 -- 44100 Hz --  128 kbps -- Stereo
Chain#0
00 00 04 F5    -    00 03 66 44    :      9152            0"026         13"844                      Mpeg 1, layer 2 -- 44100 Hz --  128 kbps -- Stereo
data loss
00 03 66 44    -    00 03 67 E5    :                     13"870                       417 bytes
Chain#1
00 03 67 E5    -    00 04 14 F5    :        46         02"768         0 bytes        16"613         Mpeg 1, layer 3 -- 44100 Hz --  128 kbps -- Joint Stereo
00 04 14 F5    -    00 37 87 92    :       897       3'30"708         0 bytes      3'47"321         Mpeg 1, layer 2 -- 44100 Hz --  128 kbps -- Stereo

startChain1... changeSpec1     -- block length time -- block length frames --  previousOffset --> mtell()    printSpec(previousHeader)
changeSpec1... changeSpec2
changeSpec2... endChain1

endChain1  ... startChain2                       corrupt data
...
...

OR...
Deal with chain1... break into chain specs
*
print mtell()
set previousOffset, set previousHeader (as input to equiv)
set current = previous
while equiv( previousHeader, currentHedear)
{ move to next header }
print mtell (position of first non-equiv header)
print sub-chain frameCount
print sub-chain length
print corrupdata == 0
print accumulated Length
print_line(first header in sub-chain
*
print enAddr(previous chain)
print startAddr(current chain)
print corrupt_data
*
process next chain
.
.
.

NOTE: I think the vbrFrame should be included in frameCount and dataSize




 */



UC processMp3Data(mp3File_t *file, U4 offset)
{ UC ch_idx, m_idx;
  U4 chainCount;
  U4 frameCount = 0;
  U4  byteCount = 0;
  UC isVbr = false;
  UC isVlength = false;
  U2 firstChain_bitRate;
  U2 bitRate;

  FL firstChain_length;
  FL length = 0;

  chain_t  *chain;
  marker_t *marker;

//  U4 i;

//  setMp3_offsets(file, offset);
  retrieve_all_chains  (file);
  chainCount = get_stream_chainCount(file);

  firstChain_bitRate = get_chain_first_bitRate(file, 0);
  firstChain_length  = get_chain_first_length (file, 0);

  for(ch_idx = 0; ch_idx<chainCount; ch_idx++)
    {
      chain = get_chain(file, ch_idx);
      chain->start_offset = get_chain_marker_start_offset(file, ch_idx, 0);
 //     chain->  end_offset =   get_chain_marker_end_offset(file, ch_idx,   get_chain_marker_count(file, ch_idx)-1  );

      for(m_idx = 0; m_idx < get_chain_marker_count(file, ch_idx)-1; m_idx++)
	{
	  marker = get_chain_marker(file, ch_idx, m_idx);
	  process_map_info(&marker->map);
//	  frameCount += marker->map.frameCount;
//	  length     += marker->map.length;
	  add_maps_v2(&chain->map, &marker->map);
	}


      /*
mseek(file, chain->start_offset, SEEK_SET);
  getMp3Header(file, &currentHdr, &currentHeader);
  first_header = currentHeader;

  isVbr     = false;   first_bitRate  = currentHdr.bitRate;
  isVlength = false;   first_length   = currentHdr.length;
//  set_chain_first_bitRate(chain, first_bitRate);
//  set_chain_first_length (chain, first_length);

//run through the chain to check for bitrate and length variations in frames
for(i=0; i<chain->map.frameCount; i++)
    {
      previousAddr   = currentAddr;     currentAddr      = mtell(file);
      offset  = currentHdr.frameSize;   nextAddr  = currentAddr + offset;
      previousHeader = currentHeader;

      mseek(file, currentHdr.frameSize, SEEK_CUR);

      getMp3Header(file, &currentHdr, &currentHeader);

	  if(currentHdr.bitRate     != first_bitRate)   isVbr     = true;
	  if(currentHdr.length      != first_length )   isVlength = true;   chain_length += currentHdr.length;
    }
*/



      set_chain_byteCount(chain, get_chain_marker(file, ch_idx, get_chain_marker_count(file, ch_idx)-1)->start_offset
			  /**/ - get_chain_marker(file, ch_idx, 0                                     )->start_offset);
      process_map_info(&chain->map);
//      frameCount += chain->map.frameCount;
//      length     += chain->map.length;
      add_maps_v2(&file->map, &chain->map);
    }
  process_map_info(&file->map);
  frameCount = file->map.frameCount;
  length     = file->map.length;

  for(ch_idx=0; ch_idx< chainCount; ch_idx++)
    { byteCount  += get_chain_byteCount                     (file, ch_idx);

      if ( get_chain_isVbr                                  (file, ch_idx))   isVbr     = true; // case 1: one chain  is  VBR
      else if (firstChain_bitRate != get_chain_first_bitRate(file, ch_idx))   isVbr     = true; // case 2: all chains are CBR but one bitRate differs from others

      if ( get_chain_isVlength                              (file, ch_idx))   isVlength = true;
      else if (firstChain_length  != get_chain_first_length (file, ch_idx))   isVlength = true;

    }

  // <== handle the vbrFrame
  if(get_vbrFrame_flag(file))
    { frameCount++;
      byteCount += get_vbrFrame_frameSize(file);
      length    += get_vbrFrame_length(file);
    }
  bitRate = round(((byteCount/length)/125));

  //set_stream_start_offset(file, get_chain_start_offset(file, 0)); // <== already set in "isMp3File"
//  set_stream_frameCount  (file,frameCount);
  set_stream_byteCount   (file, byteCount);
  set_stream_isVbr       (file,     isVbr);
  set_stream_bitRate     (file,   bitRate);
  set_stream_isVlength   (file, isVlength);
//  set_stream_length      (file,    length);
  set_stream_end_offset  (file, get_chain_end_offset(file, chainCount-1));

  if(isVlength)   fprintf(ostream, "VLEN\n");
  return SUCCESS;
}

VD retrieve_all_chains(mp3File_t *file)
{ U4 filesize = get_fileSize(file);

  /**/  mseek(file, get_stream_start_offset(file), SEEK_SET);
  while(mtell(file) < filesize) {  addChain(file);   /*printU4(mtell(file), NL);*/ }
  create_chain_ptrList(&file->chain_list); // <=== do not call directly. -- create_chain_ptrList(mp3File_t *) will call ...
}

UC isMp3File(mp3File_t *file)
{ U4 start_offset = 0;

  mseek(file, 0,  SEEK_SET);
  while(true)
    { if (findHeader                  (file               ) == FAILURE)   return false;
      if (findChain                   (file, &start_offset) == FAILURE)
	mseek                         (file,  start_offset+1, SEEK_SET);
      else   { set_stream_start_offset(file,  start_offset);              return  true; } }
  /**/                                                                    return false;
}

UC setMp3_offsets(mp3File_t *file, U4 offset)
{ //                                offset : start of first mp3 chain, computed when generating the list of files to be processed
  // good when reopening a file we know is mp3.
  //  bad when   opening a new mp3 file
  // <===
  //      can run at start of tag retrieval.
  //      check by b/r/e/w/... option if it needs to run on input file.
  //
  return SUCCESS; // <== do nothing
  mseek(file, offset, SEEK_SET);
//  printf("---------> "); printU4(offset,NL);
  setVbrHeader(file);

  return SUCCESS;
}

 /*

1) scanSpecChanges -->

2> silentScan -->

3> isVbr, get br-type(B/C) and vbr-value
for each chain
  for each frame
    process bitRate comparison and calculate average
if used in 1> and 2> it makes one more full frame-scan
since they do all their processing in one pass it would mean running 2 scans... BAD
SOOOOOO we don't run this in function 1> and 2>

Is there a way to only use one function with a flag SCAN-TYPE
that would use the flag to perform one duty or another??

That seems a bit heavy.
BUT
using 3 functions that perform some actions in common isn't perfect either

IF only used for xingHeader-generation
all we need to know is byteCount, frameCount and isVbr to know if we create the header

We'd also need to make sure there's only one chain
We technically don't REALLY care if the channel mode changes
eventhough it would be a file "corruption"
if we ignore this... the chain creation would have made sure all
  */
