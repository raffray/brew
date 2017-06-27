#include <ctype.h>
#include "includes.h"
#include "APIC.h"
#include "id3v2Tag_frame.h"
#include "mp3File.h"
#include "extern.h"
#include "byte.h"
#include "text-encoding/utf-8.h"
#include "text-encoding/utf-8_prints.h"
#include "err.h"
#include "str.h"

char picType[21][22] = { "Other", "32x32 png", "Other file icon", "cover", "back", "Leaflet page", "Media", "Lead artist", "Artist", "Conductor",
			 "Band", "Composer", "Lyricist", "Recording Location", "During recording", "During performance", "Movie", "Other", /*"A bright coloured fish",*/
			 "Illustration", "Artist logotype", "Publisher logotype"};

/*
<Header for 'Attached picture', ID: "APIC">
Text encoding   $xx
MIME type       <text string> $00                              <-- this should be an ascii string? iso? encoding is not related here.
Picture type    $xx
Description     <text string according to encoding> $00 (00)   <-- the encoding byte refers to this string
Picture data    <binary data>
*/

#define REMAINING_BYTES(a)  (data + dataSize - (a))

UC APIC_valid(UC *data, U4 dataSize, apic_data_ *apic)
{ S4 len;

  apic->enc = data[0];

  apic->mime_string_start = data+1;
  len = REMAINING_BYTES             (apic->mime_string_start           );   if(len<1) return false;
  apic->mime_string_len   = text_len(apic->mime_string_start, len, 0xFF);

  len = REMAINING_BYTES(apic->mime_string_start + apic->mime_string_len);   if(len<2) return false;
  apic->pic_type = data[apic->mime_string_len+1];

  apic->desc_string_start =          apic->mime_string_start+ apic->mime_string_len +1;
  len = REMAINING_BYTES             (apic->desc_string_start           );   if(len<1) return false;
  apic->desc_string_len   = text_len(apic->desc_string_start, len,  apic->enc);

  apic->picture_start     =          apic->desc_string_start+ apic->desc_string_len;
  len = REMAINING_BYTES             (apic->picture_start               );   if(len<1) return false;
  apic->pic_dataSize      = len;

  return true;
}

void print_APIC(mp3File_t *file, U4 frameNb)
{ UC   *data     = get_id3v2Tag_frame_data    (file, frameNb);
  U4    dataSize = get_id3v2Tag_frame_dataSize(file, frameNb);
//  buffer_t *buf  = get_id3v2Tag_frame_buffer(file, frameNb);
  apic_data_ apic;
  //U4 i;

  if(APIC_valid(data, dataSize, &apic))
    { fprintf(ostream, "(");
			print_firstString_str(apic.desc_string_start, apic.desc_string_len-1, true, apic.enc );
			////for(i=0; i<length; i++)   { fprintf(ostream, "%c", data[i]); }
      fprintf(ostream, "), [%s, %d]: ", picType[apic.pic_type], apic.pic_type);
			print_firstString_str(apic.mime_string_start, apic.mime_string_len-1, true, 0xFF);
			////for(i=0; i<length; i++)   { fprintf(ostream, "%c", data[i]); }
      fprintf(ostream, " , %d bytes", apic.pic_dataSize); }

	//		fprintf(ostream, "working on it");
}


void extract_APIC(mp3File_t *file, U4 frameNb)
{ UC *data     = get_id3v2Tag_frame_data    (file, frameNb);
  U4    dataSize = get_id3v2Tag_frame_dataSize(file, frameNb);
  char *filename = get_filename(file);
  apic_data_ apic;
  char *pic;
  char*pic_name;
  FILE *fp;

  if(APIC_valid(data, dataSize, &apic))
    {
      pic_name = malloc(strlen(filename) + 3 + strlen(picType[apic.pic_type]) +4 +1);
      sprintf(pic_name, "%s - %s.jpg", filename, picType[apic.pic_type]);

      fp = fopen(pic_name, "w");

      pic = malloc(apic.pic_dataSize);
      memcpy(pic, apic.picture_start, apic.pic_dataSize);
      fwrite(pic, 1, apic.pic_dataSize, fp);
      free(pic);
      fclose(fp);
    }
}

  /*
APIC * (Test Artist - Test Name), [cover, 3]: image/jpg, 4762 bytes
APIC * (Solarized), [back, 4]: image/jpg, 15976 bytes
APIC * (Solarized Twice), [Media, 6]: image/jpg, 20432 bytes
  */
