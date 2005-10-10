# $Id$
#
# Copyright (c) 2004-2005 Peter Urbanec. All Rights Reserved.
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

# The next line pulls in the rule sets for building TAPs.
include ../../tools/build/tap.mk

# The first target is the default target.
all: my.tap

# A target for removing generated files.
clean:
	-rm -f *.tap *.o *.elf

###################
#
# Add your modifications down here
#
###################

# If the TAP source code is contained in a single file
# you don't need anything else, as long as the name of
# the source file and the name of the TAP match.
#
# For example, if you are creating my.tap, you need my.c


# If your TAP is built from multiple files (highly recommended)
# you will need to specify the dependencies between the files.
# This is done by telling make which source files are required
# to create a particular target file. These dependecy rules are
# then combined with built in rules and make will figure out what
# files need to be built and when.
#
# For example, lets say we are building my.tap from my.c as well
# as main_menu.c config_menu.c and menu.h. Let's also assume that
# all of the .c files #include the menu.h file. Here are the
# dependencies:
#
#my.elf:: my.o main_menu.o config_menu.o
#my.o: my.c menu.h
#main_menu.o: main_menu.c menu.h
#config_menu.o: config_menu.c menu.h
#
#######
# N.B. - The dependencies must start at column 0.
#        No space allowed at the start of line.
#######
$(error Please modify your Makefile by following the comments contained inside and then delete this line)

