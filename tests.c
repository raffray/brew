#include <unistd.h>
#include <math.h>

#include "includes.h"

#include "buffer.h"
#include "byte.h"
#include "mp3Brew.h"
#include "mp3Frame.h"
#include "mp3File.h"
#include "extern.h"
#include "cpu_endian.h"
#include "mio.h"
#include "err.h"
#include "fileList.h"
#include "buffer.h"
#include "text-encoding/utf-8.h"
#include "text-encoding/utf-16.h"
#include "text-encoding/iso-8859-1.h"
#include "text-encoding/ascii.h"
#include "bstream.h"

UC something(char *filename)
{
  struct stat fstat;

  fprintf(ostream, "%s\n",filename);
  if(lstat(filename, &fstat)!=0) { fprintf(ostream, "___\n\n"); return FAILURE; } // <=== should check for errors ??
  // in case of a link, lstat will not look at what it points to... it's a link, bye
	
  if(S_ISLNK(fstat.st_mode))   { fprintf(ostream, "is a link\n\n"); return SUCCESS; }   else fprintf(ostream, "is not a link\n");
  if(S_ISDIR(fstat.st_mode))     fprintf(ostream, "is a dir \n");   else fprintf(ostream, "is not a dir \n");
  if(S_ISREG(fstat.st_mode))     fprintf(ostream, "is a file\n");   else fprintf(ostream, "is not a file\n");
  fprintf(ostream, "\n");

  return SUCCESS;
}

/* U4 get_next_non_null_index(char *str, U4 len)
{ U4 idx=0;
  while((idx<len) && (*(str+(UC)idx)=='\0')) idx++;
  return idx; } */



// Genealogie
/*
VD print_section(bstream_t* stream, U4 count, U4 special)
{
  U2 nb;
  U4 i, j;
  UC c;
  U4 event;

  for(i=0; i<count; i++)
    {
      b_readU2(stream, &nb);
      nb = swapU2(nb);
      if(nb==0) printf("NILL");
      else
	{
	  if(i==special-1)
	    {
	      for(j=0; j<nb/4; j++)
		{
		  b_readU4(stream, &event);
		  event = swapU4(event);
		  printf("%d / ", event);
		}
//	      printf("Unprocessed");
	    }
	  else
	    for(j=0; j<nb; j++)
	      {
		b_readUC(stream, &c);
		printf("%c", c);
	      }
	}
      printf("%c", 0x09);
    }
      printf("\n");
}
*/



int tests(int argc, char **argv)
{
  /*
  buffer_t buf;
  U4 len = 0;

  buffer_init2(&buf,24);
  buffer_writeU4(&buf, 0x00310000);
  buffer_writeU4(&buf, 0x31003200);
  buffer_writeU4(&buf, 0x00003300);
  buffer_writeU4(&buf, 0x00343536);
  buffer_writeU4(&buf, 0x31323030);
  buffer_print(&buf);

  buffer_clean(&buf);
  buffer_print(&buf);

  buffer_seek(&buf, 0, SEEK_SET);
  while(len<buf.size)
    {
      printf("[%s]\n", buf.data+len);
      len += strlen(buf.data+len)+1;

    }

  free(buf.data);  exit(43);
*/
  /*
  char *data = NULL;
  char *data2 = NULL;
//  char *tmp = NULL;

  printU4(data,NL);//  printU4(data,SP); printf("  "); printU4(tmp,NL);
  data = malloc(3);
  data2=malloc(10);
  printU4(data,NL);//  printU4(data,SP); printf("  "); printU4(tmp,NL);

  data = realloc(data, 25360);
  printU4(data,NL);//  printU4(data,SP); printf("  "); printU4(tmp,NL);

  free(data);
  printU4(data,NL);//  printU4(data,SP); printf("  "); printU4(tmp,NL);

  free(data2);
  */
  /*  
  buffer_t buf;
  char *data;

  data = malloc(3);
  memset(data, 0, 3);
//  buffer_open2(&buf, &data, 3);
  buffer_open(&buf, data, 3);
//  printf("%d ", buf.size); buffer_print(&buf); 
  buffer_writeU4(&buf, 0x11223344);
  buffer_writeU4(&buf, 0x11223344);
//  printf("%d ", buf.size); buffer_print(&buf); 

  free(buf.data);
  exit(42);
  */

  /*
  buffer_t buf;
  char *data;
  UC size = 11;
  UC val;//  char *val;
  UC i;

  data = malloc(size);
//  val = malloc(4);

  buffer_open(&buf, data, size);
  buffer_seek(&buf, 0, SEEK_END);
  buffer_writeU4(&buf, 0x112233ff);

  for(i=0; i<250 ;i++)
    { buffer_seek(&buf, i, SEEK_CUR);
//      buffer_read(&buf, val, 5);
      buffer_readUC(&buf, &val);
 }

  buffer_close(&buf);
//  free(val);
  exit(42);
  */

  // Genealogie
  /*
  bstream_t stream;
  U4 i;

  b_open(&stream, "Modif.vig"); //"jp2.vig"); //"nicole.vig");

  printf("\t\t\tNb Personnes\tNb Liens\t\tNb Evenements\n");
  print_section(&stream, 11,0);   printf("\n");
  printf("Personnes\n#\t\tSex\tNom\tPrenom\tPere\tMere\t?\t?\tNote\t?\tCreation\tLast Modif\t?\tEvenements\t?\t?\t?\n"); // Sex: 0=Male, 1=Female
  for(i=0; i<254; i++)    { printf("#%d\t",i+1);  print_section(&stream, 17, 14); }   printf("\n"); //84
  printf("Liens\n#\t\tP1\tP2\tEvenements\n");
for(i=0; i<69 ; i++)    { printf("#%d\t",i+1);  print_section(&stream, 4, 4);  }   printf("\n");  // 25
  print_section(&stream, 1, 0);   printf("\n");
  printf("Evenements\n#\t\tEvenement\tDate\tHeure\t\tLieu\tInititule\n");
for(i=0; i<254; i++)    { printf("#%d\t",i+1);    print_section(&stream, 7, 0); }   printf("\n");// 59
*/

  /*
  // create a string made up of 1 character (utf-8?)
  // and try to create a file with that name

  // <========================  

   */


  /*
  for(i=start; i< stream.fileSize-3; i++)
    {
      b_readUC(&stream, &c1);
      b_readUC(&stream, &c2);
      b_readUC(&stream, &c3);
//      printUC(c1, NL);
//      printUC(c2, NL);
//      printUC(c3, NL);
//      printU4((c1<<16)+(c2<<8)+c3, NL);
//      exit(543);
      if(((c1<<16)+(c2<<8)+c3) == 0x01004F)   { printU4(b_tell(&stream), NL);   count++; }
      b_seek(&stream, -2, SEEK_CUR);
    }
  printf("==> [%d]\n", count);
  //  b_close(&stream);
  exit(42);
  */

/*{ // buffer resynchronisation tests
    char str1[10] = { 0x11, 0xFF, 0x00, 0xFE, 0xFF, 0x00, 0x00, 0x77 , 0x88 , 0x99 };
    U4 s2             = 1000000;
    U4 size           = 10000000;
    char *str2 = malloc(10000000);
    U4 i=0,j=10;
    buffer_t bufin;
    buffer_t *bufout;

    for(i=0;i<s2;i++)	memcpy((char*)(str2+(i*j)), str1, 10);
//    option 1
//    buffer_open(&bufin, str2, size);// printU4(size,NL);  printBuffer(&bufin);   
//    resyncBuf(str2, &size);         // printU4(size,NL);  printBuffer(&bufin);   buffer_close(&bufin);
//  option 2
    buffer_open(&bufin, str2, size);                         // printU4(size,NL); printBuffer(&bufin);
    getSynched_buffer_fromBuffer(&bufin,&bufout, size,true); // printU4(bufout->size,NL); printBuffer(bufout); //buffer_close...
    free(bufOut);

    free(str2);
    exit(42);
  }*/

/*  {
    U4 v;
    char mystr1[12] =  { 0xFF, 0xFE, 0x54, 0x00, 0x65, 0x00, 0x73, 0x00, 0x74, 0x00, 0x00, 0x00 };
    char mystr2[10] =  { 0xFF, 0xFE, 0x54, 0x00, 0x65, 0x00, 0x73, 0x00, 0x74, 0x00 };
    v = text_len_2(mystr1, 12, 1); printU4(v, NL);
    print_string(mystr1, 12, true, 1); printf("\n");
    v = text_len_2(mystr2, 10, 1); printU4(v, NL);
    print_string(mystr2, 10, true, 1); printf("\n");

    v = text_len_2(mystr2, -1, 1); printU4(v, NL);
    exit(32);
    }*/

/*
  //UTF8 et al. tests
  {
#define S1 4
    char name1 [S1  ] = {      0xEF, 0xBB, 0xA5, 0x00};                                          // UTF8 string : ﻥ
    char name1_[S1+1] = {0x03, 0xEF, 0xBB, 0xA5, 0x00};                                          // includes string encoding as first byte
    //                       | ﻥ               |
#define S2 12
    char name2 [S2  ] = {      0xFE, 0xFF, 0xFE, 0xE5, 0x00, 0xE9, 0x00, 0xe8, 0xd8, 0x01, 0xdc, 0x00 }; // UTF16 string: ﻥéè𐐀 
    char name2_[S2+1] = {0x01, 0xFE, 0xFF, 0xFE, 0xE5, 0x00, 0xE9, 0x00, 0xe8, 0xd8, 0x01, 0xdc, 0x00 };
    //                       | encoding  | ﻥ         | é         | è         | 𐐀                   |
    // as utf-8  :           0xEF,  0xBB, 0xA5, 0xC3, 0xA9, 0xC3, 0xA8, 0xF0, 0x90, 0x90, 0x80
    //                       | ﻥ                | é         | è         | 𐐀                   |
    char name3_[S2+1] = {0x01, 0xFE, 0xFF, 0xFE, 0xE5, 0x00, 0x00, 0x00, 0xe8, 0xd8, 0x01, 0xdc, 0x00 };
    //                       | encoding | ﻥ          | \0        | è         | 𐐀                   |
    char name4_[S2+1] = {0x00, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B };

    char name5_[0x37+1] = {0x01,    0xFF, 0xFE, 0x00, 0x00, 0xFF, 0xFE, 0x47, 0x00,   0x65, 0x00, 0x72, 0x00, 0x6D, 0x00, 0x61, 0x00, 
			            0x6E, 0x00, 0x79, 0x00, 0x2C, 0x00, 0x20, 0x00,   0x41, 0x00, 0x75, 0x00, 0x67, 0x00, 0x75, 0x00, 
			            0x73, 0x00, 0x74, 0x00, 0x20, 0x00, 0x32, 0x00,   0x35, 0x00, 0x2C, 0x00, 0x20, 0x00, 0x31, 0x00, 
			            0x39, 0x00, 0x38, 0x00, 0x34, 0x00, 0x00 };
    char iso[7] = { 0x00, 0x65, 0xB2, 0xFF, 0x0A, 0x0D, 0x33 };

    char snail[6] = {0x03, 0xf0, 0x9f, 0x90 ,0x8c, 0x00 };
    char test[10] = {0x03, 0x31, 0x32, 0x33 ,0x34, 0xEF, 0xBF, 0xBD, 0x0A, 0x33 };

    char ascii_str[257];
    char ascii_str2[3] = { 0x33, 0x1F, 0x33 };
    U4 i;

    for(i=0; i<128; i++)   ascii_str[i] = i;
    ascii_str[128]=0;
    for(i=129; i<257; i++)   ascii_str[i] = i-1;

    printf("[%s]\n", name1);
    printf("[%s]\n", name2);
    
    print_string(name1_, S1+1, false, 0);
    print_string(name2_, S2+1, false, 0);
    print_string(name3_, S2+1, false, 0);
    print_string(name4_, S2+1, false, 0);
    print_string(name5_, 0x38, false, 0);
    print_string(iso   , 7   , false, 0);
    print_string(snail , 6   , false, 0);

    print_string(test , 10   , false, 0);
    printf("ASCII\n");
    print_string(ascii_str, 257, true, 0xFF); 
    print_string(ascii_str2, 3, true, 0xFF); 
    print_string(ascii_str, 257, true, 0x00); 

    printf("Tests... done!\n\n");
//    exit(42);
  }
*/


/*
      buffer_open(&buffout, cp, CPS);
      buffer_seek(&buffout, 0, SEEK_SET);
      memset(cp, 0, CPS);
      for (i=     1; i<  0x20; i++)   { write_codepoint_to_utf8(&buffout,      i);   write_codepoint_to_utf8(&buffout, 0x20); }   write_codepoint_to_utf8(&buffout, 0x0A);
      for (i= 0x7F ; i<  0xA0; i++)   { write_codepoint_to_utf8(&buffout,      i);   write_codepoint_to_utf8(&buffout, 0x20); }   write_codepoint_to_utf8(&buffout, 0x0A);
                                  { write_codepoint_to_utf8(&buffout, 0x00AD);   write_codepoint_to_utf8(&buffout, 0x20); }   write_codepoint_to_utf8(&buffout, 0x0A);
      for (i= 0x600; i< 0x606; i++)   { write_codepoint_to_utf8(&buffout,      i);   write_codepoint_to_utf8(&buffout, 0x20); }   write_codepoint_to_utf8(&buffout, 0x0A);
                                  { write_codepoint_to_utf8(&buffout, 0x061C);   write_codepoint_to_utf8(&buffout, 0x20); }   write_codepoint_to_utf8(&buffout, 0x0A);
                                  { write_codepoint_to_utf8(&buffout, 0x06DD);   write_codepoint_to_utf8(&buffout, 0x20); }   write_codepoint_to_utf8(&buffout, 0x0A);
                                  { write_codepoint_to_utf8(&buffout, 0x180E);   write_codepoint_to_utf8(&buffout, 0x20); }   write_codepoint_to_utf8(&buffout, 0x0A);
      for (i=0x200B; i<0x2010; i++)   { write_codepoint_to_utf8(&buffout,      i);   write_codepoint_to_utf8(&buffout, 0x20); }   write_codepoint_to_utf8(&buffout, 0x0A);
      for (i=0x2028; i<0x202F; i++)   { write_codepoint_to_utf8(&buffout,      i);   write_codepoint_to_utf8(&buffout, 0x20); }   write_codepoint_to_utf8(&buffout, 0x0A);
      for (i=0x2060; i<0x2065; i++)   { write_codepoint_to_utf8(&buffout,      i);   write_codepoint_to_utf8(&buffout, 0x20); }   write_codepoint_to_utf8(&buffout, 0x0A);

      // 2065???

      for (i=0x2066; i<0x2070; i++)   { write_codepoint_to_utf8(&buffout,      i);   write_codepoint_to_utf8(&buffout, 0x20); }   write_codepoint_to_utf8(&buffout, 0x0A);
                       { write_codepoint_to_utf8(&buffout, 0xD800);   write_codepoint_to_utf8(&buffout, 0x20); }   write_codepoint_to_utf8(&buffout, 0x0A);
      for (i=0xDB7F; i<0xDB81; i++)   { write_codepoint_to_utf8(&buffout,      i);   write_codepoint_to_utf8(&buffout, 0x20); }   write_codepoint_to_utf8(&buffout, 0x0A);

      for (i=0x32BD; i<0x32D0; i++)   { write_codepoint_to_utf8(&buffout,      i);   write_codepoint_to_utf8(&buffout, 0x20); }   write_codepoint_to_utf8(&buffout, 0x0A);

      printf("%s\n", cp);
      buffer_close(&buffout);
      exit(42);
*/




  /*
{
  buffer_t buf;
  what(&buf);
//  printU4( *((U4*)(buf)), NL );
  printU4( *((U4*)(buf.buffer)), NL );
  exit(42);
}
  */

  /* Multi-byte printing tests. valid for unicode and uft8, at least

  // <=== BEWARE, a stream has its orientation set by the first print call to it
  //  fprintf (stdout, "Start1\n");
  //  fwprintf(stdout, L"Start2\n");
  //
  // will result in only "Start1" to be printed!

  U4 buffer [100];
  int cx1;
  int cx2;

  U4 string[3] = { 0x0032, 0x00F3, 0x0000};
  U4 uc1  = L'\x00c5';
  U4 utf1 = L'\uc080';

  // how to convert a string to utf8?
  int res;
  U4 utf8_[4];

  //#define LEN 3
  //char str[LEN] = { 0b11011100, 0b10011110, 0 }; // 110a bcde 10fg hijk   with [x xxxx  xx xxxx] = [0000 0xxx xxxx xxxx] = 0x07 1E = [0000 0111 0001 1110]
  //                                    0x07 1E ->  110
  //                                                   1 11
  //                                                       00 
  //                                                          10
  //                                                            01        
  //                                                               1110
  //                                                1101 1100 1001 1110 --> 0xDC 9E
  //                                                 
  #define LEN 4
  char str[LEN] = { 0b11011100, 0b10011110, 0x0032, 0 }; // first 2 bytes, = utf8 for --> ܞ <--   next is --> 2 <--, in ascii as well as utf8

  setlocale(LC_ALL, "");

  res = mbstowcs(utf8_, str, LEN);
  wprintf(L"-- [%d] -->  (%s)  [%ls]\n", res, str, utf8_);

  wprintf(L"[%lc %lc]\n", uc1, utf1);


  // ********************
  cx1 = swprintf(buffer    , 4, L"%lc", string[0]);
  cx2 = swprintf(buffer+cx1, 4, L"%lc", string[1]);
  fwprintf (stdout, L"%d [%ls]\n", cx1+cx2, buffer);

  exit(42); 
  */

  /*
  {
//Generate all rename test files
//
//is_TPE1_is_TPE2__is_TALB__is_TRACK__is_TIT2
//no      no       no       no        no        
//
// in the case is_TPE1__is_TPE2
// we will double files into TPE1==TEP2 and TPE1!=TPE2 (egal and diff versions)

  char   id_block[10] = { 0x49, 0x44, 0x33, 0x03, 0x00, 0x00, 0x00, 0x00, 0x09, 0x5D };
  char tpe1_block[18] = { 0x54, 0x50, 0x45, 0x31, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x41, 0x52, 0x54, 0x49, 0x53, 0x54, 0x31 }; 
  char tpe2_block[18] = { 0x54, 0x50, 0x45, 0x32, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x41, 0x52, 0x54, 0x49, 0x53, 0x54, 0x32 }; 
  char talb_block[16] = { 0x54, 0x41, 0x4C, 0x42, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x41, 0x4C, 0x42, 0x55, 0x4D }; 
  char trck_block[12] = { 0x54, 0x52, 0x43, 0x4B, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x31 }; 
  char tit2_block[16] = { 0x54, 0x49, 0x54, 0x32, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x54, 0x49, 0x54, 0x4C, 0x45 };

  char    frame[6][5] = { "TPE1", "TPE2", "TALB", "TRCK", "TIT2" };
  char presence[2][3] = { "is", "no" };
  int iter0, iter1, iter2, iter3, iter4;
  char filename[57];
  FILE *file;
  FILE *datafile = fopen("data", "r");
  U4 datafile_len;
  char *datafile_buffer;

  fseek(datafile, 0, SEEK_END);
  datafile_len = ftell(datafile);
  fseek(datafile, 0, SEEK_SET);
  datafile_buffer = malloc (datafile_len);
  fread(datafile_buffer, 1, datafile_len, datafile);
  fclose(datafile);

  for(iter0=0; iter0<2; iter0++)
  for(iter1=0; iter1<2; iter1++)
  for(iter2=0; iter2<2; iter2++)
  for(iter3=0; iter3<2; iter3++)
  for(iter4=0; iter4<2; iter4++)
    {
      sprintf(filename, "%s_%s__%s_%s__%s_%s__%s_%s__%s_%s.mp3", presence[iter0], frame[0]
	      /__/                                             , presence[iter1], frame[1]
	      /__/                                             , presence[iter2], frame[2]
	      /__/                                             , presence[iter3], frame[3]
	      /__-                                             , presence[iter4], frame[4]);

//      printf("[%s]\n", filename);

      file = fopen(filename, "w+");
      fwrite(id_block, 1, 10, file);
      if(iter0 == 0) fwrite(tpe1_block, 1, 18, file);
      if(iter1 == 0) fwrite(tpe2_block, 1, 18, file);
      if(iter2 == 0) fwrite(talb_block, 1, 16, file);
      if(iter3 == 0) fwrite(trck_block, 1, 12, file);
      if(iter4 == 0) fwrite(tit2_block, 1, 16, file);
      fwrite(datafile_buffer, 1, datafile_len, file);
      fclose(file);

      }
  free(datafile_buffer);
  exit(42);
  }
*/

  /*
  {
  U4 i;
  char c;
  FILE *fp_in  = fopen("0.mp3", "r");
  FILE *fp_out = fopen("1.mp3", "w+");


  for (i=0; i<10000; i++)
    {
      c = fgetc(fp_in);
      fputc(c, fp_out);
    }

  fclose(fp_in);
  fclose(fp_out);
  }
  */

  /*{ // One shot, split file into 2, removing the blank in the middle
      // 2.mp3 is Alannis Morissete track 4 on single "Oughta Know"
    mp3File_t mpfile;
    mp3File_t mpfile2_1;
    mp3File_t mpfile2_2;
    U4 header;
    mp3Header_t hdr;
    U4  startOffset_first;
    U4    endOffset_first;
    U4 startOffset_second;
    U4   endOffset_second;
    char *buffer;
    float accDuration = 0;

                            setMp3File(&mpfile   , "2.mp3"  , 0, FSTREAM);    mseek(&mpfile   , 0, SEEK_SET);
    resetFile("2_1.mp3");    setMp3File(&mpfile2_1, "2_1.mp3", 0, FSTREAM);    mseek(&mpfile2_1, 0, SEEK_SET);
    resetFile("2_2.mp3");    setMp3File(&mpfile2_2, "2_2.mp3", 0, FSTREAM);    mseek(&mpfile2_2, 0, SEEK_SET);

    while(1)
      {	mreadU4(&header, &mpfile);     mseek(&mpfile, -4, SEEK_CUR);
	setMp3Header(&hdr, header);
	accDuration += hdr.length;   if(accDuration>251) break;               // 4'11
	mseek(&mpfile, hdr.frameSize, SEEK_CUR); }
    startOffset_first = 0;
    endOffset_first   = mtell(&mpfile); printU4(endOffset_first,NL); // <== gives an approximation 
    endOffset_first   = 0x543E62;                                  // <== through an hex-editor, locate start of silent frames

    while(1)
      {	mreadU4(&header, &mpfile);     mseek(&mpfile, -4, SEEK_CUR);
	setMp3Header(&hdr, header);
	accDuration += hdr.length;   if(accDuration>309) break;               // 5'09
	mseek(&mpfile,  hdr.frameSize, SEEK_CUR); }
    startOffset_second = mtell(&mpfile); printU4(startOffset_second,NL); // Same as above
    startOffset_second = 0x57E7B2;
    endOffset_second   = get_fileSize(&mpfile);                        // Same as above
    endOffset_second   = 0x8A03DA;

    buffer = malloc(endOffset_first - startOffset_first);
    mseek    (&mpfile, startOffset_first, SEEK_SET);
    mreadStr (buffer ,   endOffset_first - startOffset_first,  &mpfile);
    mwriteStr(buffer ,   endOffset_first - startOffset_first,  &mpfile2_1);
    free     (buffer);

    buffer = malloc(endOffset_second - startOffset_second);
    mseek    (&mpfile, startOffset_second, SEEK_SET);
    mreadStr (buffer ,   endOffset_second - startOffset_second,  &mpfile);
    mwriteStr(buffer ,   endOffset_second - startOffset_second,  &mpfile2_2);
    free     (buffer);

    closeMp3File(&mpfile);
    closeMp3File(&mpfile2_1);
    closeMp3File(&mpfile2_2);
    exit(42);
  }*/


  /*
  {
    U4 i;
    mp3Header_t hdr;
    U4 val;


    for(i=0xE000; i<=0xFFFF; i++)
      {
	if( ((i&0xE000) == 0xE000) &&
	    ((i&0x000C) == 0x0000) &&
	    ((i&0x0003) == 0x0000) )
	  {
	    val = 0xFF000000 + ((i<<8) & 0x00FFFF00);
	    if(val == 0xFFFF1000) printf("HERE\n");
	    if(setMp3Header(&hdr, val) == SUCCESS)
	      { printU4(val, SP);  printMp3Header_line(&hdr); }
	  }
      }

    for(i=0xE000; i<=0xFFFF; i++)
      {
	if( ((i&0xE000) == 0xE000) &&
	    ((i&0x000C) == 0x0004) &&
	    ((i&0x0003) == 0x0000) )
	  {
	    val = 0xFF000000 + ((i<<8) & 0x00FFFF00);
	    
	    if(setMp3Header(&hdr, val) == SUCCESS)
	      { printU4(val, SP);  printMp3Header_line(&hdr); }
	  }
      }

    for(i=0xE000; i<=0xFFFF; i++)
      {
	if( ((i&0xE000) == 0xE000) &&
	    ((i&0x000C) == 0x0008) &&
	    ((i&0x0003) == 0x0000) )
	  {
	    val = 0xFF000000 + ((i<<8) & 0x00FFFF00);
	    
	    if(setMp3Header(&hdr, val) == SUCCESS)
	      { printU4(val, SP);  printMp3Header_line(&hdr); }
	  }
      }
   
    exit(2);
  }
  */
  /*
  { // test for gaps and final frame reaching beyond EOF
    mp3File_t file;
    mp3Header_t hdr;
    U4 f1 = 0xFFFB1000;   U4 size1;   char *buf1;   // 104  framesize
    U4 f2 = 0xFFFB1400;   U4 size2;   char *buf2;   //  96
    U4 f3 = 0xFFFB2800;   U4 size3;   char *buf3;   // 180
    U4 i;
    U4 k;

    resetFile(            "short-last-frame.mp3");   
    setMp3File(&file   , "short-last-frame.mp3"  , 0, FSTREAM);    mseek(&file, 0, SEEK_SET);
    setMp3Header(&hdr, f1);   size1 = hdr.frameSize-4;   buf1 = malloc(size1);
    setMp3Header(&hdr, f2);   size2 = hdr.frameSize-4;   buf2 = malloc(size2);
    setMp3Header(&hdr, f3);   size3 = hdr.frameSize-4;   buf3 = malloc(size3);

    for(i=0; i<3; i++)
    {                                                                                   //mwriteUC(&file, 0x35);
      for(k=0; k< 14; k++)   { mwriteU4(f1, &file);   mwriteStr(buf1, size1, &file); }	//mwriteUC(&file, 0x35);
      for(k=0; k< 15; k++)   { mwriteU4(f2, &file);   mwriteStr(buf2, size2, &file); }	//mwriteUC(&file, 0x35;
      for(k=0; k< 16; k++)   { mwriteU4(f3, &file);   mwriteStr(buf3, size3, &file); }
//      {                        mwriteU4(f1, &file);   mwriteStr(buf1, size1, &file); }	//mwriteU2(0x3543, &file)
      mwriteUC(&file, 0);
    }

    free(buf1);
    free(buf2);
    free(buf3);
    closeMp3File(&file);
  }
  */

  /*
    { // vbr-frame minimum size study
    UC version;
    UC layer;
    U2 bitrate;
    U2 samplingrate;
    UC channelmode;
    // since we only consider those 5 fields, FFFA appears but not FFFB. ans it is fine.
    mp3Header_t hdr;
    U4 val0, val;
    UC xing_offset;
UC xingOffset[4][2] = { {32, 17},  // Stereo       - V1/ V2&3
			{32, 17},  // Joint Stereo - V1/ V2&3
			{32, 17},  // Dual Channel - V1/ V2&3
			{17, 9} }; // Mono         - V1/ V2&3

    for(version=0; version<4; version++)                      // 0   2 3
      {
	if(version!=1)
	for(layer=1; layer<4; layer++)                          //   1 2 3
	  for(samplingrate=0; samplingrate<3; samplingrate++) // 0 1 2
	    for(bitrate=1; bitrate<15; bitrate++)                  //   1 to 14
	      for(channelmode=0; channelmode<4; channelmode++)  // 0 1 2 3
		{
		  val0 = 0xFFE00000 + (version <<19) + (layer << 17) + (bitrate << 12) + (samplingrate << 10) + (channelmode << 6);
		  if(setMp3Header(&hdr, val0) != SUCCESS) printf("ERROR  ");
		  xing_offset = xingOffset[hdr.channelMode][hdr.versionId-1];

		  printU4(val0, SP);  
		  //		  printMp3Header_line(&hdr); 
		  fprintf(ostream, "Mpeg %d,%d -- %5d Hz -- %4d kbps -- ", hdr.versionId, hdr.layer, hdr.samplingRate, hdr.bitRate);
		  switch( hdr.channelMode)
		    {case 0 : fprintf(ostream,   "Stereo         -- "); break;
		    case  1 : fprintf(ostream,   "Joint Stereo   -- "); break;
		    case  2 : fprintf(ostream,   "Dual Channel   -- "); break;
		    case  3 : fprintf(ostream,   "Single Channel -- "  ); break;
		    default :                                           break; }
		  fprintf(ostream, "%4.3fms -- frameSize = %4d   size needed= %4d",   1000.0*hdr.length, hdr.frameSize, 120+xing_offset);
		  if (hdr.frameSize>=120+xing_offset) printf("       OK\n");
		  else                                printf("   NOT OK\n");  // <-- increase bitrate until it works

		  val = (val0 & 0xFFFF0FFF) + 0x00006000;
		  if(setMp3Header(&hdr, val) != SUCCESS) printf("ERROR  ");
		  xing_offset = xingOffset[hdr.channelMode][hdr.versionId-1];

		  printU4(val, SP);  
		  //		  printMp3Header_line(&hdr); 
		  fprintf(ostream, "Mpeg %d,%d -- %5d Hz -- %4d kbps -- ", hdr.versionId, hdr.layer, hdr.samplingRate, hdr.bitRate);
		  switch( hdr.channelMode)
		    {case 0 : fprintf(ostream,   "Stereo         -- "); break;
		    case  1 : fprintf(ostream,   "Joint Stereo   -- "); break;
		    case  2 : fprintf(ostream,   "Dual Channel   -- "); break;
		    case  3 : fprintf(ostream,   "Single Channel -- "  ); break;
		    default :                                           break; }
		  fprintf(ostream, "%4.3fms -- frameSize = %4d   size needed= %4d",   1000.0*hdr.length, hdr.frameSize, 120+xing_offset);
		  if (hdr.frameSize>=120+xing_offset) printf("       OK\n\n");
		  else                                printf("   NOT OK\n\n");  // <-- increase bitrate until it works
		  
		}
      }

      exit(42);
  }
  */


  /*
  { // creating files containing 1 frame
    mp3File_t   frame_file; 
    mp3File_t   outfile; 
    mp3Header_t frame_hdr;
    U4          frame_hdrVal;
    char *name  = "frame_256kbps.mp3";
    char *name2 = "frame_256kbps";

    char *str;

    // **********************************************
    setMp3File(&frame_file, name, 0, FSTREAM);
    mreadU4(&frame_file, &frame_hdrVal);
    setMp3Header(&frame_hdr, frame_hdrVal);

    str = malloc(frame_hdr.frameSize);
    mseek(&frame_file, SEEK_SET, 0);    
    mreadStr(&frame_file, str, frame_hdr.frameSize);

    resetFile(name2); setMp3File(&outfile, name2, 0, FSTREAM);
    mseek   (&outfile, SEEK_SET, 0);    
    mwriteStr(&outfile, str, frame_hdr.frameSize);
    closeMp3File(&outfile);

    closeMp3File(&frame_file);
    exit(42);
  }
  */

  /*
  { // vbrframe size "study"
    U4  bytecount;
    U4 framecount;
    mp3Header_t vbr_hdr;
    U4          vbr_hdrVal;
    mp3File_t file;
    char name[16]; // "vbr_xyzkbps.mp3"
    U4 i, j, k;
    mp3File_t   frame_file1; 
    mp3Header_t frame_hdr1;
    U4          frame_hdrVal1;
    char *str1;
    mp3File_t   frame_file2; 
    mp3Header_t frame_hdr2;
    U4          frame_hdrVal2;
    char *str2;

    // **********************************************
    setMp3File  (&frame_file1, "frame_128kbps", 0, FSTREAM);
    mreadU4     (&frame_file1, &frame_hdrVal1);
    setMp3Header(&frame_hdr1  , frame_hdrVal1);

    str1 = malloc(frame_hdr1.frameSize);
    mseek       (&frame_file1, SEEK_SET, 0);    
    mreadStr    (&frame_file1, str1, frame_hdr1.frameSize);

    closeMp3File(&frame_file1);
    // **********************************************
    // **********************************************
    setMp3File  (&frame_file2, "frame_256kbps", 0, FSTREAM);
    mreadU4     (&frame_file2, &frame_hdrVal2);
    setMp3Header(&frame_hdr2  , frame_hdrVal2);

    str2 = malloc(frame_hdr2.frameSize);
    mseek       (&frame_file2, SEEK_SET, 0);    
    mreadStr    (&frame_file2, str2, frame_hdr2.frameSize);

    closeMp3File(&frame_file2);
    // **********************************************

    for(i=1; i<15; i++)
      {
	vbr_hdrVal = 0xFFFB0060 + (i<<12);
	setMp3Header(&vbr_hdr, vbr_hdrVal);

	//framecount = 400; //
 	framecount = 401;
	//bytecount = 200 * frame_hdr1.frameSize + 200 * frame_hdr2.frameSize;
	bytecount = vbr_hdr.frameSize + 200 * frame_hdr1.frameSize + 200 * frame_hdr2.frameSize;

	sprintf(name, "vbr_%03dkbps.mp3", vbr_hdr.bitRate);

	resetFile(name);    setMp3File(&file, name, 0, FSTREAM);    mseek(&file, 0, SEEK_SET);
	mwriteU4(&file, vbr_hdrVal);
	for(j=0; j<32;j++)   mwriteUC(&file, 0);
	mwriteU4(&file, XING);
	mwriteU4(&file, 0x00000003);
	mwriteU4(&file, framecount);
	mwriteU4(&file,  bytecount);
	for(j=0; j<vbr_hdr.frameSize-52;j++)   mwriteUC(&file, 0);

	for(k=0; k<200; k++)   mwriteStr(&file, str1, frame_hdr1.frameSize);
	for(k=0; k<200; k++)   mwriteStr(&file, str2, frame_hdr2.frameSize);

	closeMp3File(&file);
      }

      free(str1); free(str2);

    exit(42);
  }
  */
  return SUCCESS;
} 
