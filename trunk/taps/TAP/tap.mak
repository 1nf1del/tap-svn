#  TAP.MAK - include this in your TAP makefiles 
#  V1.0  JDA  30 June 2004

# where the tap include files are like libtap.a, tap.h, etc
TAPDIR=c:\cygwin\usr\local\tap

# where cygwin is installed
CYGWINDIR=c:\cygwin

RELEASEDIR = d:\

# include directories for mips compiler
TAPINCLUDES=-I$(TAPDIR)\include -I $(CYGWINDIR)\usr\include -I $(CYGWINDIR)\usr\local\include -I ..\common

# update PATH with the cygwin directories
PATH=$(PATH);$(CYGWINDIR)\usr\local\bin;$(CYGWINDIR)\bin

#
#redefine this to your extra object list in your makefile
EXTRAOBJECTS=

# 
# -c = don't run linker
# -m* calls relate to the MIPS options
# -O2 turns on most optional optimisations
#

.c.o:	#TAPs source to object
	mips-gcc.exe -O2 -c -mtap -mlong-calls -msoft-float -D_TAP -DDEBUG $(TAPINCLUDES) $*.c

#.c.tap:	#TAPs source to object
#	mips-gcc.exe -O2 -c -mtap -mlong-calls -msoft-float $(TAPINCLUDES) -D_TAP -DNDEBUG $*.c
#	mips-ld --cref -o $*.elf -T $(TAPDIR)\lib\TAP.LD $*.o $(EXTRAOBJECTS) -l tap -l c -l gcc -l m -Map $*.map
#	mips-objcopy -O binary $*.elf $*.tap
#	copy $*.tap $(RELEASEDIR)

#
# -T = link commands from this file
# -l = link with lib'file'.a
# --cref = output crossref table
# -map = create a link map file
#

.o.elf:	#
	mips-ld --cref -o $*.elf -T $(TAPDIR)\lib\TAP.LD $*.o -L $(TAPDIR)\lib $(EXTRAOBJECTS) -l tap -l c -l gcc -l m -Map $*.map

.elf.tap:	#TAP program from object
	mips-objcopy -O binary $*.elf $*.tap
	copy $*.tap $(RELEASEDIR)
