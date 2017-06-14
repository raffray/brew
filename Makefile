srcdir = .
#SUB_DIRS = text-encoding 
#INCLUDES = -I. -Itext-encoding -Iid3v2_frames
SHELL= /bin/sh 
CC= gcc
LN= $(CC)
RM= rm -f

#CFLAGS = -g -Wall -I$(srcdir) -pg
CFLAGS = -g -Wall -I$(srcdir) 
#-I$(SUB_DIRS)
#CFLAGS = -O5  -I$(srcdir) -I$(SUB_DIRS)
LIBS = -lm
TARGET = mp3Brew
OBJS =  tests.o mp3Brew.o byte.o frameID.o \
	id3v2_frames/Txyz.o id3v2_frames/TXXX.o id3v2_frames/TCON.o \
	id3v2_frames/COMM.o id3v2_frames/APIC.o id3v2_frames/POPM.o \
	cpu_endian.o mio.o length_map.o \
	mp3Frame.o mp3Frame_chain_list.o mp3Frame_marker_list.o \
	mp3Stream.o mp3StreamAnalysis.o \
	mp3File.o str.o err.o mfile.o fstream.o bstream.o vbrFrame.o fileList.o prints.o \
	renameFile.o examine.o extractPics.o wipeFile.o cleanTag.o \
	id3v1Tag.o id3v1_xtdTag.o \
	id3v2Tag_header.o id3v2Tag.o id3v2Tag_frameList.o id3v2Tag_frame.o id3v2Tag_frame_v3.o id3v2Tag_frame_v4.o \
	buffer.o sync_buf.o filename.o \
	text-encoding/utf-8.o text-encoding/utf-16.o text-encoding/iso-8859-1.o text-encoding/ascii.o

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS)  -o $(TARGET) $(OBJS) $(LIBS)

.c.o :
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	$(RM) *.o */*.o core.* *~ */*~ $(TARGET)


all:  $(TARGET)
