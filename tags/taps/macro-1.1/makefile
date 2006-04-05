#  Include technology makefiles first
#
!include <tap.mak>

RELEASEDIR = ..\..\TAPfiles
# INCLUDEDIR = C:\work\tap
INCLUDEDIR = C:\work\tapReal
EXTRAOBJECTS = menu.o  window.o keyHandler.o logging.o macromenu.o ini.o
TARGETS = macro.tap keymap

all: $(TARGETS)

#
#
macro.tap:	macro.o $(EXTRAOBJECTS) $(INCLUDEDIR)\libUtils.a
	mips-ld --cref -o $*.elf -T $(TAPDIR)\TAP.LD $*.o $(EXTRAOBJECTS) -l Utils -l tap -l c -l gcc -l m -Map $*.map
	mips-objcopy -O binary $*.elf $*.tap
	copy $*.tap $(RELEASEDIR)

keymap:		$(RELEASEDIR)\macro.ini

$(RELEASEDIR)\macro.ini:	macro.ini
	copy macro.ini $(RELEASEDIR)

macro.o:	macro.c $(INCLUDEDIR)\tap.h macro.h keyhandler.h $(INCLUDEDIR)\keymap.h

keyHandler.o:	keyHandler.c  $(INCLUDEDIR)\tap.h macro.h keyhandler.h $(INCLUDEDIR)\keymap.h

menu.o:		menu.c $(INCLUDEDIR)\tap.h window.h

window.o:	window.c  $(INCLUDEDIR)\tap.h window.h

logging.o:	logging.c  $(INCLUDEDIR)\tap.h logging.h

macromenu.o:	macromenu.c  $(INCLUDEDIR)\tap.h macromenu.h

ini.o:		ini.c  $(INCLUDEDIR)\tap.h ini.h


clean:
	-del $(TARGETS)
	-del *.o *.map
