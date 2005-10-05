#  Include technology makefiles first
#
!include <tap.mak>

APIDIR = D:\source\topfield\api
TARGETS = VirtualRemote.tap

all: $(TARGETS)

#
VirtualRemote.tap:	VirtualRemote.o
	mips-ld --cref -o $*.elf -T $(APIDIR)\TAP.LD $*.o -L $(APIDIR) -l c -l tap -l gcc -l m -Map $*.map
	mips-objcopy -O binary $*.elf $*.tap
	tfcopy $*.tap "#:\ProgramFiles\"

VirtualRemote.o:		VirtualRemote.c

clean:
	-del $(TARGETS)
	-del *.o *.elf *.map
