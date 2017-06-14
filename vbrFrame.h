#ifndef VBRFRAME_T
#define VBRFRAME_T

#include "includes.h"
#include "brew_types.h"

typedef struct vbrFrame
{
  U4 header;
  U4 offset;
  U4 type; // Xing/Info/VBRI
  U4 idOffset;
  U4 xingFlags; // XING ONLY
  UC frameCount_flag;
  U4 frameCount; //  stored value (what the stream should contain)
  UC  byteCount_flag;
  U4  byteCount; //  stored value (what the stream byte-count should be)
  UC tocFlag;
  U2 qInd; // quality Indicator;
  U4 frameSize;
  FL length;
  U2 version; // VBRI ONLY
  U2 delay;   // VBRI ONLY
} vbrFrame_t;

//UC   isVbrHeader      (mp3File_t *);
//VD processVbrHeader (mp3File_t */*, mp3Header_t **/);
VD setVbrHeader     (mp3File_t *);
VD processXingHeader(mp3File_t */*, mp3Header_t **/);
VD processVbriHeader(mp3File_t */*, mp3Header_t **/);
VD printVbrHeader   (mp3File_t */*, mp3Header_t **/);
VD printXingHeader  (mp3File_t */*, mp3Header_t **/);
VD printVbriHeader  (mp3File_t */*, mp3Header_t **/);
UC checkXingHeader  (mp3File_t *);
UC createXingFrame  (mp3File_t *, char **, U4 *);

void  set_vbrFrame_flag  (mp3File_t *, UC);
UC    get_vbrFrame_flag  (mp3File_t *);
void  set_vbrFrame_offset  (mp3File_t *, U4);
U4    get_vbrFrame_offset  (mp3File_t *);
void  set_vbrFrame_idOffset(mp3File_t *, U4);
U4    get_vbrFrame_idOffset(mp3File_t *);

void  set_vbrFrame_type(mp3File_t *, U4);
U4    get_vbrFrame_type(mp3File_t *);
void  set_vbrFrame_xingFlags(mp3File_t *, U4);
U4    get_vbrFrame_xingFlags(mp3File_t *);

void  set_vbrFrame_frameCount_flag(mp3File_t *, UC);
UC    get_vbrFrame_frameCount_flag(mp3File_t *);
void  set_vbrFrame_frameCount(mp3File_t *, U4);
U4    get_vbrFrame_frameCount(mp3File_t *);

void  set_vbrFrame_byteCount_flag(mp3File_t *, UC);
UC    get_vbrFrame_byteCount_flag(mp3File_t *);
void  set_vbrFrame_byteCount(mp3File_t *, U4);
U4    get_vbrFrame_byteCount(mp3File_t *);

void  set_vbrFrame_header   (mp3File_t *, U4);
U4    get_vbrFrame_header   (mp3File_t *);

void  set_vbrFrame_frameSize(mp3File_t *, U4);
U4    get_vbrFrame_frameSize(mp3File_t *);
void  set_vbrFrame_length   (mp3File_t *, float);
float get_vbrFrame_length   (mp3File_t *);

void set_vbrFrame_version(mp3File_t *, U2); // VBRI ONLY
U2   get_vbrFrame_version(mp3File_t *);     // VBRI ONLY
void set_vbrFrame_delay(mp3File_t *, U2); // VBRI ONLY
U2   get_vbrFrame_delay(mp3File_t *);     // VBRI ONLY
void set_vbrFrame_tocFlag(mp3File_t *, UC);
UC   get_vbrFrame_tocFlag(mp3File_t *);
void set_vbrFrame_qInd(mp3File_t *, U2);
U2   get_vbrFrame_qInd(mp3File_t *);

#endif // VBRFRAME_T
