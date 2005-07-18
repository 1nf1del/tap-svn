#  Include technology makefiles first
#
!include <tap.mak>

RELEASEDIR = C:\work\tap
TAPOBJECTS = ini.o  keymap.o  file.o  Utils.o vsprintf.o
#TAPHEADERS = ini.h  keymap.h  file.h  Utils.h
TARGETS = libUtils.a

all: $(TARGETS)

#
#

libUtils.a:   $(TAPOBJECTS) $(TAPHEADERS)
	mips-ar -cr $@ $**
	copy $*.a $(RELEASEDIR)
	copy *.h $(RELEASEDIR)

clean:
	-del $(TARGETS)
	-del *.o *.map

