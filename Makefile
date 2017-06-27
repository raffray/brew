srcdir = .
SHELL= /bin/sh
CC= gcc
LN= $(CC)
RM= rm -f

#CFLAGS = -g -Wall -I$(srcdir) -pg
CFLAGS = -g -Wall -I$(srcdir)
#CFLAGS = -I$(srcdir)
#CFLAGS = -O5 -I$(srcdir)
#BYTE_FLAG =
LIBS = -lm
TARGET = mp3Brew

OBJS =  mp3Brew.o tests.o \
	mp3File.o mp3Frame_chain_list.o mp3Frame_marker_list.o mp3Frame.o mp3Stream.o mp3StreamAnalysis.o \
	byte.o buffer.o bstream.o fstream.o prints.o \
	cleanTag.o examine.o extractPics.o renameFile.o \
	err.o fileList.o filename.o frameID.o mio.o \
	id3v1Tag.o id3v1_xtdTag.o id3v2Tag_frame.o id3v2Tag_frame_v3.o id3v2Tag_frame_v4.o  id3v2Tag_frameList.o  id3v2Tag_header.o \
	id3v2Tag.o length_map.o mfile.o str.o sync_buf.o wipeFile.o vbrFrame.o \
	text-encoding/ascii.o text-encoding/iso-8859-1.o text-encoding/utf-16.o text-encoding/utf-8.o \
	text-encoding/utf-8_prints.o text-encoding/unicode.o \
	id3v2_frames/APIC.o id3v2_frames/COMM.o id3v2_frames/POPM.o id3v2_frames/TCON.o id3v2_frames/TXXX.o id3v2_frames/Txyz.o





$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS)  -o $(TARGET) $(OBJS) $(LIBS)

byte.o:
	${CC} ${CFLAGS} -o is_big_endian is_big_endian.c
	@ ./is_big_endian; if [ $$? -eq 0 ] ; \
	then $(CC) -c $(CFLAGS) -DLITTLE_ENDIAN byte.c -o byte.o; echo $(CC) -c $(CFLAGS) -DL_ENDIAN byte.c -o byte.o; \
	else $(CC) -c $(CFLAGS)            byte.c -o byte.o; echo $(CC) -c $(CFLAGS)            byte.c -o byte.o; fi
	@rm ./is_big_endian

.c.o :
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) *.o */*.o core.* *~ */*~ $(TARGET) logfile


all:  $(TARGET)
