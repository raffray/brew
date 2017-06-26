#include "id3v2Tag.h"
#include "id3v2Tag_header.h"
#include "id3v2Tag_frameList.h"
#include "id3v2Tag_frame.h"
#include "mp3File.h"
#include "mp3Stream.h"
#include "extern.h"
#include "frameID.h"
#include "byte.h"
#include "mfile.h"
#include "buffer.h"
#include "err.h"
#include "fileList.h"
#include "id3v2_frames/APIC.h"

#define TIT2 0x54495432
#define TPE1 0x54504531
#define TALB 0x54414C42
//#define TYER 0x
#define TDRC 0x54445243
#define COMM 0x434F4D4D
#define TRCK 0x5452434B
#define TCON 0x54434F4E

void set_id3v2Tag_size(mp3File_t *file, U4 size)   {        (file->id3v2Tag).size  = size; }
U4   get_id3v2Tag_size(mp3File_t *file         )   { return (file->id3v2Tag).size;         }

UC retrieve_id3v2Tag(mp3File_t *file)
{ U4     fileSize = get_fileSize(file);
  id3v2Tag_t *tag = &(file->id3v2Tag);

  memset(tag, 0, sizeof(id3v2Tag_t));

  TRY(check_id3v2Tag_presence(file) , {})
  if (!get_id3v2Tag_presenceFlag(file))   return FAILURE;

  if((HEADER_SIZE + get_id3v2Tag_xtdHdr_size(file)) > fileSize)  return pReturn(file, "Extended header present with improper size");
  retrieve_id3v2Tag_frameList(file);  // cannot fail

  return SUCCESS;
}

/*

get offset and size of buffer to resync in file

compute upSize (next multiple of 4)
malloc str of upSize bytes
memset it with 0s
readStr "size"bytes at "offset"

malloc buf2 of "size" bytes and memset it
open 2 buffer_t, for both strings
perform unsync
set new size for buf2

return buf2 (need string and new_size)
       or just string and size.

keep original data?
 * case 1: id3v2.3 --> original data is all the data between end_of_header and start_of_mp3-data (end of last frame would be preferable but can't be determined at this point)

 */

void clean_id3v2Tag(id3v2Tag_t *tag)
{
  // clean tag->frameList
  clean_id3v2Tag_frameList(&(tag->frameList));
}

void print_id3v2Tag(mp3File_t *file)
{ U4 i;
  U4 frameID;

  fprintf(ostream, "Id3v2.%d.%d Tag\n", get_id3v2Tag_version(file), get_id3v2Tag_revision(file));
  for(i=0; i<get_id3v2Tag_frameCount(file); i++)
    { frameID = get_id3v2Tag_frame_frameID  (file, i);
      printU4str(frameID, SP); // <==
  //      if      (get_id3v2Tag_frame_compression_flag(file, i))   { fprintf(ostream, "\bX -- compressed frame. Display not yet implemented\n"); }
  //      else if ( get_id3v2Tag_frame_encryption_flag(file, i))   { fprintf(ostream, "\bX -- encrypted  frame. Display not implemented\n"    ); }
  //      else
	{ if (isSelectFrame(frameID) == true)   fprintf(ostream, "\b: ");
	  else                                  fprintf(ostream, "\b* ");
	  print_id3v2Tag_frame(file, i); } }
  }

U4 dump_id3v2Tag(mp3File_t *file)
{ U4 frameCount = get_id3v2Tag_frameCount(file);
  U4 i;
  U4 tagSize = 0;
  buffer_t frame_buf;
  U4 dataSize = get_firstFrame_offset(file);
  UC *data = malloc(dataSize);
  UC version = 4; //  get_id3v2Tag_version(file)

  if (get_id3v2Tag_presenceFlag(file))
    {
      // Prepare an empty buffer, the size of the original Tag
      memset(data, 0, dataSize);
      buffer_open(&frame_buf, data, dataSize);
      buffer_writeU4(&frame_buf, 0x49443300 | version);

      buffer_seek(&frame_buf, 6, SEEK_CUR);
      for(i=0; i<frameCount; i++)
	tagSize += HEADER_SIZE + dump_id3v2Tag_frame(file, &frame_buf, i);

      mseek(file, 0, SEEK_SET);
      mwriteStr(file, data, dataSize);

      // The ID3v2 tag size is the size of the complete tag after unsychronisation, including padding, excluding the header but not excluding the extended header
      mseek(file, 6, SEEK_SET);
      mwriteU4(file, encodeU4(dataSize-10));
    }

  free(frame_buf.data);
  return dataSize; //tagSize; // <== only using datSize now sice we overwrite the tag and don't change the tag's size
}

void extractPictures(mp3File_t *file)
{ U4 i;
  //  UC picFound = false;

  for(i=0;     i<get_id3v2Tag_frameCount(file); i++)
    if (is_APIC_tag(get_id3v2Tag_frame_frameID(file, i)))
      {
  //	if(picFound==false)
	//{ picFound = true;   fprintf(ostream, "Extracting pictures\n"); }
	extract_APIC(file, i); }
}

VD addFrametoBuf(buffer_t *buf, U4 id, UC *str, U4 len)
{
  buffer_writeU4(buf, id);
  if(id==COMM)
    {
      buffer_writeU4(buf, 1+4+len);
      buffer_writeU2(buf, 0); // flags
      buffer_writeUC(buf, 0); // encoding
      buffer_writeU4(buf, 0x656E6700); // "eng", for English
    }
  else
    {
      buffer_writeU4(buf, 1+len); // 1 byte f0r encoding, then string
      buffer_writeU2(buf, 0); // flags
      // might want to encode in utf-8 which is equivalent for "standard" iso chars
      buffer_writeUC(buf, 0); //encoding = ISO
    }
  buffer_writeStr(buf, str, len);
}

VD addFrametoBuf_UC(buffer_t *buf, U4 id, UC val)
{
  char str1[3];
  //  char str2[5];

  sprintf(str1, "%d", val);
  /*  if(id == TCON)
    { sprintf(str2, "(%s)", str1);
      addFrametoBuf(buf, id, str2, strlen(str2));}
      else*/ addFrametoBuf(buf, id, (UC *)str1, strlen(str1));
}

buffer_t *makeV2fromV1Tag(mp3File_t *file)
{
  buffer_t *buf;
  id3v1Tag_t *v1;
  id3v1_xtdTag_t *xtd_v1;

  buf = malloc(sizeof(buffer_t));
  buffer_init_default(buf);
  buffer_writeU4(buf, 0x49443304);
  buffer_writeU2(buf, 0x0000);
  buffer_writeU4(buf, 0);

  if (get_id3v1Tag_presenceFlag(file) == false)   return NULL;
  v1 = &(file->id3v1Tag);
  if (get_id3v1_xtdTag_presenceFlag(file))
    xtd_v1 = &(file->id3v1_xtdTag);

  if(get_id3v1_xtdTag_presenceFlag(file))
    {
      addFrametoBuf(buf, TIT2, xtd_v1->title, 60);
      addFrametoBuf(buf, TPE1, xtd_v1->artist, 60);
      addFrametoBuf(buf, TALB, xtd_v1->album, 60);
    }
  else
    {
      addFrametoBuf(buf, TIT2, v1->title, 30);
      addFrametoBuf(buf, TPE1, v1->artist, 30);
      addFrametoBuf(buf, TALB, v1->album, 30);
      }
  //  char year   [ 5];  --> TYER
  //  char comment[31];  --> COMM
  //  UC track;          --> TRCK
  //  UC genre;          --> TCON

  addFrametoBuf(buf, TDRC, v1->year, 4);
  if(v1->flag==0)
    addFrametoBuf(buf, COMM, v1->comment, 28);
  else
    addFrametoBuf(buf, COMM, v1->comment, 30);

  if(v1->flag==0)
    addFrametoBuf_UC(buf, TRCK, v1->track);
  addFrametoBuf_UC(buf, TCON, v1->genre);

  buffer_seek(buf, 6, SEEK_SET);
  buffer_writeU4(buf, encodeU4(buf->usedSize-10));

  return buf;
}

id3v2Tag_t *makeV2fromV1Tag_(mp3File_t *file)
{
/*
Fields from id3v1Tag
  char title  [31];  --> TIT2
  char artist [31];  --> TPE1
  char album  [31];  --> TALB
  char year   [ 5];  --> TYER
  char comment[31];  --> COMM
  UC track;          --> TRCK
  UC genre;          --> TCON

Fields from id3v1Tag_xtd
  char title  [61];  --> TIT2
  char artist [61];  --> TPE1
  char album  [61];  --> TALB
  Speed
  Start time
  End   time
 */

/*
  mp3File_t testFile;
  id3v2Tag_t *v2;
  id3v1Tag_t *v1;
  id3v1_xtdTag_t *xtd_v1;
  id3v2Tag_header_t    *header;
  id3v2Tag_frameList_t *frameList;
  char *title;

  if (get_id3v1Tag_presenceFlag(file) == false)   return NULL;
  v1 = &(file->id3v1Tag);
  if (get_id3v1_xtdTag_presenceFlag(file))
    xtd_v1 = &(file->id3v1_xtdTag);

  v2 = &(testFile.id3v2Tag);

  memset(v2, 0, sizeof(id3v2Tag_t));
  header = &(v2->header);
  header->version  = 4;

  frameList = &(v2->frameList);

  if(get_id3v1_xtdTag_presenceFlag(file))
    { push_id3v2Tag_frameLink(frameList, TIT2, xtd_v1->title , 60, NULL);
      push_id3v2Tag_frameLink(frameList, TPE1, xtd_v1->artist, 60, NULL);
      push_id3v2Tag_frameLink(frameList, TALB, xtd_v1->album , 60, NULL); }
  else
    { title = malloc(30);
      strcpy(v1->title, title);
      push_id3v2Tag_frameLink(frameList, TIT2, title , 30, NULL);
      push_id3v2Tag_frameLink(frameList, TPE1, v1->artist, 30, NULL);
      push_id3v2Tag_frameLink(frameList, TALB, v1->album , 30, NULL);     }

      push_id3v2Tag_frameLink(frameList, TDRC, v1->year   ,  4, NULL);
      push_id3v2Tag_frameLink(frameList, COMM, v1->comment, 30, NULL);

      create_id3v2Tag_frame_ptrList(frameList);

      print_id3v2Tag(&testFile);
  return v2;
*/
return NULL;
}



/*
void printFrameIDs(mp3File_t *file, UC select, U4 tagSize)
{ U4 i; // dismissed optimisation (for the few frames we'll print...) and went for simplicity of code

  fprintf(ostream, "Id3v2.%d tag\n", get_tag_version(file      ));
  fprintf(ostream, " Tag size       : ");   printU4(tagSize, SP);
  fprintf(ostream, "= %7"PRId32" bytes\n",          tagSize    );
  fprintf(ostream, " Frames         : "                        );

  if(        get_tag_frameCount(file)==0)   fprintf(ostream, "\n");
  for(i=0; i<get_tag_frameCount(file); i++)
    if( ((select==SELECT) && (isSelectFrame(get_tag_frame_id(file, i)) == true)) || (select==ALL))
      { if (i==get_tag_frameCount(file)-1) printU4str(get_tag_frame_id(file, i), NL);
	else                               printU4str(get_tag_frame_id(file, i), SP); }

}
*/
