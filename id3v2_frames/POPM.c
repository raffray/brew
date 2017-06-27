#include "includes.h"
#include "POPM.h"
#include "id3v2Tag_frame.h"
#include "mp3File.h"
#include "extern.h"
#include "byte.h"
#include "mio.h"
#include "err.h"
#include "str.h"
#include "text-encoding/utf-8.h"
#include "text-encoding/utf-8_prints.h"

/*
  <Header for 'Popularimeter', ID: "POPM">
  Email to user <text string> $00          <=== * ascii?
  Rating $xx
  Counter $xx xx xx xx (xx ...)            <=== * at least 32 bits long, adding one byte in front when counter bytes all reach 0xFF
  //                                            * optionnal
 */

void print_POPM(mp3File_t *file, U4 frameNb)
{

  UC      *data     = get_id3v2Tag_frame_data    (file, frameNb);
  U4       dataSize = get_id3v2Tag_frame_dataSize(file, frameNb);
  buffer_t data_buffer;

  UC rating = 0;
  U4 counter= 0;
  U4 counterByteSize;
  U4 length;
  UC val;

  length = text_len(data, dataSize, 0xFF);
  print_firstString_str(data, length,  true, 0xFF);
  counterByteSize = dataSize - (length+1);

  buffer_open  (&data_buffer, data, dataSize);
  buffer_seek  (&data_buffer, length+1, SEEK_SET);
  buffer_readUC(&data_buffer, &rating);
  counterByteSize--;
  if(   counterByteSize>4)   counterByteSize = 4;   // <==
  for(; counterByteSize>0;   counterByteSize--)
    { buffer_readUC(&data_buffer, &val);   counter = (counter<<8) + val; }
  buffer_close(&data_buffer);

  fprintf(ostream, ", counter=%d rating=%d", counter, rating);
}
