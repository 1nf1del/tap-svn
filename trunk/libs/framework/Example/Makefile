#
#  Copyright (c) 2005 Simon Capewell. All Rights Reserved.
#
#  This file is part of the TAPs for Topfield PVRs project.
#      http://tap.berlios.de/
#
#  This is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  The software is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this software; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
LIBRARIES=../libFramework.a ../../libc++utils/libc++utils.a ../../libutils/libutils.a 
OBJECTS=TestFramework.o YesNoBox.o BitmapScreen.o TextTools.o

all: $(LIBRARIES) TestFramework.tap 

include ../../../tools/build/tap.mk
include ../../libc++utils/tap++.mk

../../libutils/libutils.a::
	$(MAKE) -C ../../libutils

../../libc++utils/libc++utils.a::
	$(MAKE) -C ../../libc++utils
	
../libFramework.a::
	$(MAKE) -C ../



CXXFLAGS+= -I ../../libc++utils -I ../
LDFLAGS+= -L../../libc++utils -L../ -L../../libutils -lFramework -lc++utils -lutils -lc
# Copy files to the Toppy using tfcopy
copy: all
	tfcopy *.tap '#:\ProgramFiles\'
	@mips-objdump -d TestFramework.elf > TestFramework.asm

# A target for removing generated files.
clean:
	-rm -f *.tap *.o *.elf *.map VTableFix.cpp

TestFramework.tap:		TestFramework.elf
TestFramework.elf:		$(OBJECTS) $(VTABLEFIX) $(LIBRARIES)
TestFramework.o:		TestFramework.cpp graphics.c
YesNoBox.o:			YesNoBox.cpp YesNoBox.h graphics.c
TextTools.o:			TextTools.cpp TextTools.h
BitmapScreen.o:			BitmapScreen.cpp BitmapScreen.h
