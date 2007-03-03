#
#  Copyright (c) 2006-2007 Simon Capewell. All Rights Reserved.
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

include ../../tools/build/tap.mk

TAPNAME=RemoteExtender

ifdef TOPPY2
CFLAGS+=-DTOPPY2
CXXFLAGS+=-DTOPPY2
TAPNAME:=$(TAPNAME)Toppy2
endif

all:
	$(MAKE) single
	$(MAKE) single TOPPY2=1

single: $(TAPNAME).tap ../../libs/libutils/libutils.a ../../libs/Firmware/libfirmware.a
	tfcopy.cmd $(TAPNAME).tap '#:\ProgramFiles\'


# A target for removing generated files.
clean:
	-rm -f *.tap *.o *.elf *.map

$(TAPNAME).tap: RemoteExtender.elf
	$(OBJCOPY) -O binary $< $@
	@mips-objdump -D RemoteExtender.elf > RemoteExtender.asm
	-rm -f *.o *.elf *.map

RemoteExtender.elf::	Main.o RemoteExtender.o MHEGState.o QuickAspectBlocker.o Settings.o ../../libs/libutils/libutils.a ../../libs/Firmware/libfirmware.a
Main.o:					Main.c Settings.h RemoteExtender.h MHEGState.h QuickAspectBlocker.h
Settings.o:				Settings.c Settings.h RemoteExtender.h MHEGState.h QuickAspectBlocker.h
RemoteExtender.o:		RemoteExtender.c RemoteExtender.h
MHEGState.o:			MHEGState.c MHEGState.h
QuickAspectBlocker.o:	QuickAspectBlocker.c QuickAspectBlocker.h

CFLAGS+=-I../../libs/libutils -I../../libs/Firmware -I../../libs/TSRCommander

../../libs/libutils/libutils.a::
	$(MAKE) -s -C ../../libs/libutils

../../libs/Firmware/libfirmware.a::
	$(MAKE) -s -C ../../libs/Firmware
