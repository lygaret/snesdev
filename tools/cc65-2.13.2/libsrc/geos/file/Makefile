#
# Makefile for GEOS lib
# for cc65
#

%.o:	%.s
	@$(AS) -o $@ $(AFLAGS) $<


S_OBJS	= get1stdirentry.o getnxtdirentry.o\
	  openrecordfile.o closerecordfile.o nextrecord.o previousrecord.o pointrecord.o\
	  deleterecord.o insertrecord.o appendrecord.o readrecord.o writerecord.o\
	  updaterecordfile.o\
	  findfile.o followchain.o getfhdrinfo.o readfile.o savefile.o freefile.o\
	  deletefile.o renamefile.o findftypes.o readbyte.o getfile.o\
	  sysremove.o sysrename.o

all: $(S_OBJS)

clean:
	@$(RM) *.~ $(S_OBJS) core
