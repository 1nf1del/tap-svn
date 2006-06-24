#  TAP.MAK - include this in your TAP makefiles 
#  V1.0  JDA  30 June 2004

# where the tap include files are like libtap.a, tap.h, etc
TAPDIR=c:\work\tap
# where cygwin is installed
CYGWINDIR=c:\cygwin
# include directories for mips compiler
TAPINCLUDES=-I$(TAPDIR) -I$(TAPDIR)\include -I $(CYGWINDIR)\usr\include -I $(CYGWINDIR)\usr\local\include -I ..

# update PATH with the cygwin directories
PATH=$(PATH);$(CYGWINDIR)\usr\local\bin;$(CYGWINDIR)\bin

# 
# -c = don't run linker
# -m* calls relate to the MIPS options
# -O2 turns on most optional optimisations
#
.c.o:	#TAPs source to object
	mips-gcc.exe -O2 -c -mtap -mlong-calls -msoft-float -D_TAP -DNDEBUG $(TAPINCLUDES) $*.c


