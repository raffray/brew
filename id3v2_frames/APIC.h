#ifndef APIC_T
#define APIC_T

#include "includes.h"
#include "brew_types.h"

typedef struct apic_data
{
  UC    enc;
  UC *mime_string_start;
  U4    mime_string_len;
  UC    pic_type;
  UC *desc_string_start;
  U4    desc_string_len;
  UC *picture_start;
  U4    pic_dataSize;
} apic_data_;

UC     APIC_valid(UC *, U4, apic_data_ *);
void   print_APIC(mp3File_t *, U4);
void extract_APIC(mp3File_t *, U4);

#endif // APIC_T
