#include "includes.h"
#include "extern.h"
#include "mp3StreamAnalysis.h"
#include "mp3Stream.h"
#include "mp3Frame.h"
#include "vbrFrame.h"
#include "mp3File.h"
#include "byte.h"
#include "err.h"
#include "prints.h"
#include "mp3Frame_marker_list.h"
#include "mp3Frame_chain_list.h"
#include "id3v1_xtdTag.h"
#include "id3v1Tag.h"

#include "length_map.h" 

#include <math.h>
#include <stdio.h>

VD printMp3Info(mp3File_t *file)
{ U4        ch_idx, m_idx;
  U4    beforeStream_size;
  U4            byteCount = get_stream_byteCount (file);
  U4           frameCount = get_stream_frameCount(file);
  U4           chainCount = get_stream_chainCount(file);
  U4      corruption_size = 0;
  U4     afterStream_size;
  U4               offset;
  mp3Header_t         hdr;

  length_map_t vbr_map;
  length_map_t *accMap;

  init_map(&vbr_map);

  /*
So far each marker contains the info about the frames between its location and that of the next one
The vbr has no marker

to print the section-info about a marker, we need:
 the offsets of it and the next         <--- start_offset for each marker
 the number of frames in it             <--- frameCount of its map
 the time location of it and the next   <--- the location at the start --- we need to sum every market segment length in a startMap
                                             the location at the end   --- we have the length between both markers

basically, we run to maps along the chains/markers

the first one is initialised with the info of the vbrFrame if present. 0 otherwise
THEN
    startmap will be the "vbr map" 
    endMap   is startMap + the first marker's map
maps do not contain the length up to their respective markers
   */

  if(get_vbrFrame_flag(file))   { printVbrHeader(file);   setMp3Header(&hdr, get_vbrFrame_header(file));   add_value_to_map(&vbr_map, &hdr); } // <== might want to store hdr in vbrFrame
  add_maps_v2(&get_chain_marker(file, 0, 0)->accMap,  &vbr_map);
  accMap  =   &get_chain_marker(file, 0, 0)->accMap;

  for(ch_idx = 0; ch_idx<chainCount; ch_idx++)
    for(m_idx = 0; m_idx <  get_chain_marker_count (file, ch_idx); m_idx++)
      {
	if((ch_idx==0) && (m_idx==0))   add_maps_v2(&get_chain_marker(file,      0,     0)->accMap, &get_chain_marker(file,      0,     0)->map);
	else                            add_maps_v1(&get_chain_marker(file, ch_idx, m_idx)->accMap, &get_chain_marker(file, ch_idx, m_idx)->map, accMap);
	accMap  =   &get_chain_marker(file, ch_idx, m_idx)->accMap;
      }

  fprintf(ostream, "\nSection               From Offset - To   Offset         #Frames   From Pos.     To Pos.            Block Size\n");
  /*TAG section */
  printSection_short(get_id3v2Tag_presenceFlag(file), "Id3v2 Tag", 0, get_id3v2Tag_size(file));

  beforeStream_size = get_firstFrame_offset(file);
  printSection_short(get_id3v2Tag_size(file) !=  beforeStream_size, "Padding+Junk?", get_id3v2Tag_size(file), beforeStream_size);

  if(get_vbrFrame_flag(file)) 
    { setMp3Header(&hdr, get_vbrFrame_header(file));
      printSection_long("VBR frame",  get_vbrFrame_offset(file), get_stream_start_offset(file), 1, &hdr, 0, hdr.length);
//      printSection_long2("Mpeg data",     , get_chain_marker(file, 0, 0)); // <--- no can do, the vbr frame has no marker
    }


  for(ch_idx = 0; ch_idx<chainCount; ch_idx++)
    { for(m_idx = 0; m_idx <  get_chain_marker_count (file, ch_idx)-1; m_idx++)    // <== is this function well named?
	{ setMp3Header(&hdr,  get_chain_marker_header(file, ch_idx,    m_idx));
	  printSection_long2("Mpeg data", get_chain_marker(file, ch_idx, m_idx  ), get_chain_marker(file, ch_idx, m_idx+1)); }

      if(ch_idx != chainCount-1)
	{ printSection_short(true, "Corruption", get_chain_end_offset(file, ch_idx), get_chain_start_offset(file, ch_idx+1));
	  corruption_size +=get_chain_start_offset(file, ch_idx+1) - get_chain_end_offset(file, ch_idx); }
    }

  offset = get_chain_end_offset(file, chainCount-1);

  /*
EOS
EOS ...
EOF - 128 - 227  --> id3v1_xtd
EOF - 128        --> id3v1
EOF

case 1: id3v1 AND xtd ... EOS >  EOF - 127 - 227 ...
                          EOS <= EOF - 127 - 227 ...
   */

  // need to check there is no clash between EOS and id3v1 and/or/ id3v1_xtd
  /*
example, only id3v1 TAG present
file size =    0x01000000 
TAG at         0x00FFFF80
stream ends at 0x00FFFFA0
the last frame overlaps the id3v1 TAG
   */

  if (!get_id3v1Tag_presenceFlag(file))
    { printSection_short( offset != get_fileSize(file), "Data after Stream", 
			  offset,   get_fileSize(file)); }
  else 
    { if (get_id3v1_xtdTag_presenceFlag(file))
	{ printSection_short( offset != get_fileSize(file) - ID3V1_TAG_LENGTH - XTD_TAG_LENGTH, "Junk???", 
			      offset,   get_fileSize(file) - ID3V1_TAG_LENGTH - XTD_TAG_LENGTH);
	  printSection_short( true, "Id3v1 Tag", 
			      get_fileSize(file)-128, get_fileSize(file)); }
      else
	{ // printSection_short( offset != get_fileSize(file), "Data after Stream", 
	//		       offset,   get_fileSize(file)); 
	  printSection_short( true, "Id3v1 Tag+", 
			      get_fileSize(file)-128, get_fileSize(file)); }
    }

  afterStream_size = get_fileSize(file) - offset;
  printSectionTitle("End of File");   printU4(get_fileSize(file), NL);
  
  fprintf(ostream, "-------------------------------------------------------------------------------------------------------------\n");
  fprintf(ostream, "Summary\n");
  
  fprintf(ostream,   " * Bitrate    :        ");
  if (get_stream_isVbr(file) == false)  fprintf(ostream,   "CBR ");
  else                 fprintf(ostream,   "VBR ");
  fprintf(ostream,      "%4d kbps\n", get_stream_bitRate(file));

  fprintf(ostream,"Track Time    :      "); printDuration(get_stream_length(file), NL);
  print_byteCount("Before Stream :   ", beforeStream_size, NL);

  fprintf(ostream, "Frame Count   :   ");  print_int_commas(frameCount);
  fprintf(ostream, "         ( "); printU4(frameCount, SP);   fprintf(ostream, ")"); 

  if(get_vbrFrame_flag(file))
//    {
      if(get_vbrFrame_frameCount_flag(file) == true )
//	{
	  if (frameCount != get_vbrFrame_frameCount(file))  
	    { fprintf(ostream, "\n * ");   printU4str(get_vbrFrame_type(file), SP); fprintf(ostream, "frame     ");  
	      print_int_commas(get_vbrFrame_frameCount(file));   fprintf(ostream, "         ( ");    
	      printU4         (get_vbrFrame_frameCount(file), SP);   fprintf(ostream, ")"); 
	      fprintf(ostream, "    WARNING! Differs from actual frame-count!!"); }
//	}
//    }
  fprintf(ostream, "\n"); 

  // HERE..... Apparently, if no vbr frame present (but file vbr???) we counted one frame too many

  print_byteCount("Audio Data    :   ", byteCount   , SP);
  fprintf(ostream, "   ( ");    printU4(byteCount, SP);    fprintf(ostream, ")");
  // vbr Audio stream byte-count ok?
  if(get_vbrFrame_flag(file))
    { if(get_vbrFrame_byteCount_flag(file) == true )
	{
	  if (byteCount != get_vbrFrame_byteCount (file))   
	    { fprintf(ostream, "\n * ");   printU4str(get_vbrFrame_type(file), SP); 
	      print_byteCount("frame     ", get_vbrFrame_byteCount(file), SP);   fprintf(ostream, "   ( ");    
	      printU4(get_vbrFrame_byteCount(file), SP);   fprintf(ostream, ")"); 
	      fprintf(ostream, "    WARNING! Differs from actual  byte-count!!"); }
	}
    }
  fprintf(ostream, "\n"); 

  if(corruption_size!=0)
  print_byteCount("Corruption    :   ", corruption_size , NL);
//  print_byteCount("After Stream  :   ", afterStream_size, NL);

  if (!get_id3v1Tag_presenceFlag(file))
    {
//      if(corruption_size!=0)
//      print_byteCount("Corruption    :   ", corruption_size , NL);
//      print_byteCount("Junk??  :   ", afterStream_size, NL);
      if(afterStream_size != 0)   print_byteCount("After Stream  :   ", afterStream_size, NL);
// printSection_short( offset != get_fileSize(file), "Data after Stream", 
//			  offset,   get_fileSize(file)); 
}
  else
    {
      if (afterStream_size != 128)
      print_byteCount("After Stream  :   ", afterStream_size - 128, NL);
      print_byteCount("Id3v1 tag     :   ",                    128, NL);
      /*
 printSection_short( offset != get_fileSize(file)-128, "Junk???", 
			  offset,   get_fileSize(file)-128);
      printSection_short( true, "Id3v1 Tag", 
			  get_fileSize(file)-128, get_fileSize(file)); 
      */
}




  if (beforeStream_size+byteCount+corruption_size+afterStream_size !=  get_fileSize(file) )  // would reveal a program flaw
    { fprintf(ostream, " *** WARNING!! BUG!! *** The following 2 values should be identical!!\n");
      print_byteCount("Total         :   ", beforeStream_size+byteCount+corruption_size+afterStream_size, NL);
      print_byteCount("Filesize      :   ", get_fileSize(file), NL); }
}


VD silentScan(mp3File_t *file)
{ U4       ch_idx, m_idx;
  U4           byteCount =  get_stream_byteCount(file);
  U4    total_frameCount = get_stream_frameCount(file);
  U4     corruption_size = 0;
  U4    afterStream_size;
  UC       vbrFrame_flag = get_vbrFrame_flag(file);
  UC               isVBR = get_stream_isVbr   (file);
  U4          chainCount = get_stream_chainCount(file);
  U4        start_offset;
  U4          end_offset;

//    if(get_id3Flag(file)==true) printf("S\n"); else printf("F\n");

  if ((!get_id3v2Tag_presenceFlag(file)) && ((    vbrFrame_flag &&  (    get_vbrFrame_offset(file)!=0))
			       || (!vbrFrame_flag &&  (get_stream_start_offset(file)!=0))))
    { printWarning(file);
      print_byteCount("Junk?? before audio stream                                              ", get_stream_start_offset(file), NL); }
  
  for(ch_idx=0; ch_idx<chainCount; ch_idx++)
    { 
      for(m_idx=1; m_idx<get_chain_marker_count(file, ch_idx)-1; m_idx++)
	{ printWarning(file); fprintf(ostream, "Frame Specifications change  at");   printNspace(3);  printU4(get_chain_marker_start_offset(file, ch_idx, m_idx), NL); }
      if(ch_idx!=chainCount-1)
	{ printWarning(file);
	  start_offset = get_chain_start_offset(file, ch_idx+1);
	  end_offset   =   get_chain_end_offset(file, ch_idx  );
	  fprintf(ostream, "Corruption in audio stream from   ");   printU4(end_offset, SP);   fprintf(ostream, "  to   ");   printU4(start_offset, SP);
	  fprintf(ostream,                        " -  size = ");   
	  print_byteCount("", start_offset - end_offset, NL);
	  corruption_size +=  start_offset - end_offset;

	  if( !equivHeaders_vlsc_U4( get_chain_marker_header(file, ch_idx, m_idx),                                get_chain_marker_header      (file, ch_idx+1, 0) ) )
	    { printWarning(file); fprintf(ostream, "Frame Specifications change  at");   printNspace(3);  printU4(get_chain_marker_start_offset(file, ch_idx+1, 0), NL); }
	}
    }
  end_offset       = get_chain_end_offset(file, chainCount-1);
  afterStream_size = get_fileSize(file) - end_offset;
  if (afterStream_size != 0)
    if(!get_id3v1Tag_presenceFlag(file))
    { printWarning(file);
      fprintf(ostream, "Data    after audio stream from   ");   printU4(end_offset, SP);
      fprintf(ostream,                            "  to   ");   printU4(get_fileSize(file), SP);
      fprintf(ostream,                         "    ..... ");   print_byteCount(" ", afterStream_size, NL); }

  // 3 mutually excusive conditions
  if(!vbrFrame_flag && (isVBR == true )) // FALSE && TRUE
    { printWarning(file);   fprintf(ostream,   "No VBR frame with VBR stream\n"); }
  else if
    ( vbrFrame_flag && (isVBR == false)) // TRUE && FALSE
    { printWarning(file);   printU4str(get_vbrFrame_type(file), SP);   fprintf(ostream,   "frame with CBR stream\n"); }
  else if
    ( vbrFrame_flag && (isVBR ==  true)) // TRUE AND TRUE
    { if (get_vbrFrame_type(file) == INFO)   {   printWarning(file);   fprintf(ostream,   "INFO frame Might want to change it to XING for better compatibility\n"); }
      
      if(get_vbrFrame_frameCount_flag(file) == true )
	if (total_frameCount != get_vbrFrame_frameCount(file))  
	  { printWarning(file);   printU4str(get_vbrFrame_type(file), SP);
	    fprintf(ostream, "frame ---> frame-count differs from actual frame-count!!\n"); }  // <====   not valid if vbr-frame has no byte info

      if(get_vbrFrame_byteCount_flag(file) == true )
	if (byteCount != get_vbrFrame_byteCount (file))   
	  { printWarning(file);   printU4str(get_vbrFrame_type(file), SP);
	    fprintf(ostream, "frame --->  byte-count differs from actual  byte-count!!\n"); }  // <====   not valid if vbr-frame has no frame info
      //                                 // FALSE AND FALSE --> CBR with no VBR-frame... all is good
      checkXingHeader(file);
    }
}

