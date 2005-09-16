#  Include technology makefiles first
#
!include <tap.mak>

TAPDIR = c:\cygwin\usr\local\tap
TARGETS = DumpAPI.tap

all: $(TARGETS)

DumpAPI.tap:	DumpAPI.o
	mips-ld --cref -o $*.elf -L $(TAPDIR)\lib -T TAP.LD $*.o -l tap -l c -l gcc -l m -Map $*.map
	mips-objcopy -O binary $*.elf $*.tap
	tfcopy $*.tap "#:\ProgramFiles\"

DumpAPI.o:	DumpAPI.c

clean:
	-del $(TARGETS)
	-del *.o *.elf *.map
