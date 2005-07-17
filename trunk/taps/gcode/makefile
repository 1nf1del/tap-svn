#  Include technology makefiles first
#
!include <tap.mak>

RELEASEDIR = ..\..\TAPfiles
INCLUDEDIR = C:\work\tap
EXTRAOBJECTS = gui.o gcfuncs.o window.o
TARGETS = gcode.tap options keymap

all: $(TARGETS)

#
#
gcode.tap:	gcode.o $(EXTRAOBJECTS) $(INCLUDEDIR)\libUtilsv1.1.a
	mips-ld --cref -o $*.elf -T $(TAPDIR)\TAP.LD $*.o $(EXTRAOBJECTS) -l Utilsv1.1 -l tap -l c -l gcc -l m -Map $*.map
	mips-objcopy -O binary $*.elf $*.tap
	copy $*.tap $(RELEASEDIR)

gcode.o:	gcode.c $(INCLUDEDIR)\tap.h gcode.h gcfuncs.h window.h $(INCLUDEDIR)\keymap.h

gui.o:		gui.c $(INCLUDEDIR)\tap.h gcode.h window.h

gcfuncs.o:	gcfuncs.c $(INCLUDEDIR)\tap.h

window.o:	window.c $(INCLUDEDIR)\tap.h window.h

keymap:		$(RELEASEDIR)\keymap.ini

$(RELEASEDIR)\keymap.ini:	keymap.ini
	copy keymap.ini $(RELEASEDIR)

options:	$(RELEASEDIR)\gcode.ini

$(RELEASEDIR)\gcode.ini:	gcode.ini
	copy gcode.ini $(RELEASEDIR)

clean:
	-del $(TARGETS)
	-del *.o *.map
