#include "prints.h"
#include "includes.h"
#include "mp3Frame.h"
#include "mp3File.h"
#include "byte.h"
#include "extern.h"
#include "length_map.h"

void printWarning(mp3File_t *file)
{ if ((printWarnings_flag == true) && (hasWarning == false))   { fprintf(ostream, "----> %s\n", get_filename(file));   hasWarning = true; }
  fprintf(ostream, "WARNING: "); }


void printNspace(int n)   { int i; for(i=0; i<n;i++) fprintf(ostream, " "); }

void printDuration (float d, UC endC)
{
  U4 mn;
  UC sec;
  U2 mil;

  if (d<0) fprintf(ostream,"+-+-+-+%c", endC);
  else
    { mn  =  d / 60;
      sec =  d - 60*mn;
      mil = (d - 60*mn - sec)*1000;
      if(mn>=1)	fprintf(ostream, "%3"PRId32"'%02d\"%03d%c", mn, sec, mil, endC);
      else	fprintf(ostream,        "    %02d\"%03d%c",     sec, mil, endC);
    }
}

void print_int_commas(U4 nb)
{
  U4 s1, s2, s3, s4; // nb = a,bbb,ccc,ddd  MAX... FF FF FF FF = 4,294,967,295

  s4 =  nb            %1000;
  s3 = (nb/1000      )%1000;
  s2 = (nb/1000000   )%1000;
  s1 = (nb/1000000000)%1000;

  if      (s1!=0) fprintf(ostream, "%1d,%03d,%03d,%03d",s1,s2,s3,s4); // s1!=0
  else if (s2!=0) fprintf(ostream,    "  %3d,%03d,%03d",   s2,s3,s4); // s1==0 s2!=0
  else if (s3!=0) fprintf(ostream,     "      %3d,%03d",      s3,s4); // s1==0 s2==0 s3!=0
  else            fprintf(ostream,      "          %3d",         s4); // s1==0 s2==0 s3==0
}
	   
void print_byteCount(const char *str, U4 nb, UC sp)
{ if ((nb==0) || (nb==1))   { fprintf(ostream, "%s", str);   print_int_commas(nb);   fprintf(ostream, " byte " ); }
  else                      { fprintf(ostream, "%s", str);   print_int_commas(nb);   fprintf(ostream, " bytes"); }
  if (sp == NL)               fprintf(ostream, "\n"); 
}

void printSectionTitle(const char *str)
{
  int maxLength = 22;
  int i;

  if   (strlen(str)> maxLength)         for(i=0;           i<maxLength; i++) fprintf(ostream, "%c", str[i]);
  else { fprintf(ostream, "%s", str);   for(i=strlen(str); i<maxLength; i++) fprintf(ostream, " "); }
}

void printSection_short(UC cond, const char *str, U4 start, U4 end)
{
  if(cond) 
    { printSectionTitle(str);   printU4(start, SP);   fprintf(ostream, "- ");   printU4(end, SP); 
      printNspace(42);          print_byteCount("", end-start, NL); }
}

void printSection_long(const char *str, U4 start, U4 end, U4 frameCount, mp3Header_t *hdr, float startTime, float endTime)
{ printSectionTitle(str);   printU4(start, SP);   fprintf(ostream, "- ");   printU4(end, SP); 
  printNspace(2);   print_int_commas(frameCount); 
  printNspace(2);   printDuration(startTime, SP); 
  printNspace(1);   printDuration(  endTime, SP); 
  print_byteCount("  ", end-start, SP);   printNspace(4);   printMp3Header_line_short(hdr);
}

void printSection_long2(const char *str, marker_t *m1, marker_t *m2)
{ printSectionTitle(str);   printU4(m1->start_offset, SP);   fprintf(ostream, "- ");   printU4(m2->start_offset, SP); 
  printNspace(2);   print_int_commas(get_map_frameCount(&m1->map)); 
  printNspace(2);   printDuration(segment_length(&m1->accMap) - segment_length(&m1->map), SP); 
  printNspace(1);   printDuration(segment_length(&m2->accMap) - segment_length(&m2->map), SP); 
  print_byteCount("  ", m2->start_offset - m1->start_offset, SP);   printNspace(4);   printMp3Header_line_short(&m1->hdr);
}
