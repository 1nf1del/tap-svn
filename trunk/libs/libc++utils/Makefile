# $Id$
#
# Copyright (c) 2004-2005 Peter Urbanec. All Rights Reserved.
#
#  This file is part of the TAPs for Topfield PVRs project.
#      http://developer.berlios.de/projects/tap/
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

# The next line pulls in the rule sets for building TAPs.
include ../../tools/build/tap.mk


# The first target is the default target.
all: libc++utils.a

CXXFLAGS+=-I../libutils -D_TAP

libc++utils.a: taparray.o tapstring.o vtable.o Logger.o PerformanceTimer.o IniFile.o
	$(AR) cr $@ $^

# A target for removing generated files.
clean:
	-rm -f *.a *.o *.elf
