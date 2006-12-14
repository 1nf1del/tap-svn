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
OBJECTS=taparray.o tapstring.o vtable.o Logger.o PerformanceTimer.o IniFile.o
OBJECTS+=Channels.o EPGchannel.o EPGdata.o EPGevent.o Globals.o Timers.o Timer.o 
OBJECTS+=MEIReader.o EPGVisitor.o TimeSlot.o PackedDateTime.o ProgressNotification.o
OBJECTS+=DirectoryUtils.o Option.o Options.o KeyDef.o EPGReader.o EPGSourceOption.o
OBJECTS+=YesNoOption.o NumericOption.o KeyOption.o OptionUpdateNotifier.o
OBJECTS+=JagCSVReader.o stringarchive.o Archive.o ArchivedProgram.o ArchiveVisitor.o
OBJECTS+=LoggingOption.o
LIBRARIES=../libutils/libutils.a 
LIBDIRS=-L ../libutils

# The first target is the default target.
all: libc++utils.a

include tap++.mk

LDFLAGS:= -u TAP_Main $(LIBDIRS) -lutils $(LDFLAGS)


INCLUDEDIRS=-I ../libutils
CXXFLAGS+= $(INCLUDEDIRS)

libc++utils.a: $(OBJECTS) $(TEMPLATESBUILTFLAG)
	$(AR) cr $@ $(OBJECTS)

# A target for removing generated files.
clean:
	-rm -f *.a *.o *.elf *.rpo *.done
	
depends:
	makedepend -I $(TAP_HOME)/include $(INCLUDEDIRS) -Y/usr/include -Y/usr/local/include -I/usr/include *.cpp

# DO NOT DELETE

Archive.o: Archive.h /usr/include/stdlib.h /usr/include/features.h
Archive.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Archive.o: /usr/include/sys/types.h /usr/include/bits/types.h
Archive.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
Archive.o: /usr/include/time.h /usr/include/endian.h
Archive.o: /usr/include/bits/endian.h /usr/include/sys/select.h
Archive.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
Archive.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
Archive.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
Archive.o: /usr/include/alloca.h ArchivedProgram.h tapstring.h
Archive.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
Archive.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
Archive.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
Archive.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
Archive.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
Archive.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h timeslot.h
Archive.o: PackedDateTime.h taparray.h /usr/include/memory.h
Archive.o: /usr/include/string.h quicksort.h DirectoryUtils.h
Archive.o: ../libutils/file.h ../libutils/Utils.h ArchiveVisitor.h inifile.h
Archive.o: Logger.H
ArchivedProgram.o: archivedprogram.h /usr/include/stdlib.h
ArchivedProgram.o: /usr/include/features.h /usr/include/sys/cdefs.h
ArchivedProgram.o: /usr/include/gnu/stubs.h /usr/include/sys/types.h
ArchivedProgram.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
ArchivedProgram.o: /usr/include/bits/typesizes.h /usr/include/time.h
ArchivedProgram.o: /usr/include/endian.h /usr/include/bits/endian.h
ArchivedProgram.o: /usr/include/sys/select.h /usr/include/bits/select.h
ArchivedProgram.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
ArchivedProgram.o: /usr/include/sys/sysmacros.h
ArchivedProgram.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
ArchivedProgram.o: /usr/include/alloca.h tapstring.h
ArchivedProgram.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
ArchivedProgram.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
ArchivedProgram.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
ArchivedProgram.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
ArchivedProgram.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
ArchivedProgram.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
ArchivedProgram.o: timeslot.h PackedDateTime.h ../libutils/file.h
ArchivedProgram.o: ../libutils/Utils.h /usr/include/string.h RecHeader.h
ArchivedProgram.o: stringarchive.h logger.h
ArchiveVisitor.o: ArchiveVisitor.h /usr/include/stdlib.h
ArchiveVisitor.o: /usr/include/features.h /usr/include/sys/cdefs.h
ArchiveVisitor.o: /usr/include/gnu/stubs.h /usr/include/sys/types.h
ArchiveVisitor.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
ArchiveVisitor.o: /usr/include/bits/typesizes.h /usr/include/time.h
ArchiveVisitor.o: /usr/include/endian.h /usr/include/bits/endian.h
ArchiveVisitor.o: /usr/include/sys/select.h /usr/include/bits/select.h
ArchiveVisitor.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
ArchiveVisitor.o: /usr/include/sys/sysmacros.h
ArchiveVisitor.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
ArchiveVisitor.o: /usr/include/alloca.h
Channels.o: channels.h /usr/include/stdlib.h /usr/include/features.h
Channels.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Channels.o: /usr/include/sys/types.h /usr/include/bits/types.h
Channels.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
Channels.o: /usr/include/time.h /usr/include/endian.h
Channels.o: /usr/include/bits/endian.h /usr/include/sys/select.h
Channels.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
Channels.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
Channels.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
Channels.o: /usr/include/alloca.h
Channels.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
Channels.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
Channels.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
Channels.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
Channels.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
Channels.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h tapstring.h
Channels.o: taparray.h /usr/include/memory.h /usr/include/string.h
Channels.o: quicksort.h
DirectoryUtils.o: DirectoryUtils.h /usr/include/string.h
DirectoryUtils.o: /usr/include/features.h /usr/include/sys/cdefs.h
DirectoryUtils.o: /usr/include/gnu/stubs.h /usr/include/stdlib.h
DirectoryUtils.o: /usr/include/sys/types.h /usr/include/bits/types.h
DirectoryUtils.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
DirectoryUtils.o: /usr/include/time.h /usr/include/endian.h
DirectoryUtils.o: /usr/include/bits/endian.h /usr/include/sys/select.h
DirectoryUtils.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
DirectoryUtils.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
DirectoryUtils.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
DirectoryUtils.o: /usr/include/alloca.h
DirectoryUtils.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
DirectoryUtils.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
DirectoryUtils.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
DirectoryUtils.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
DirectoryUtils.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
DirectoryUtils.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
DirectoryUtils.o: tapstring.h ../libutils/file.h ../libutils/Utils.h
DirectoryUtils.o: taparray.h /usr/include/memory.h quicksort.h Logger.h
EPGchannel.o: epgchannel.h /usr/include/stdlib.h /usr/include/features.h
EPGchannel.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
EPGchannel.o: /usr/include/sys/types.h /usr/include/bits/types.h
EPGchannel.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
EPGchannel.o: /usr/include/time.h /usr/include/endian.h
EPGchannel.o: /usr/include/bits/endian.h /usr/include/sys/select.h
EPGchannel.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
EPGchannel.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
EPGchannel.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
EPGchannel.o: /usr/include/alloca.h
EPGchannel.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
EPGchannel.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
EPGchannel.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
EPGchannel.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
EPGchannel.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
EPGchannel.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h taparray.h
EPGchannel.o: /usr/include/memory.h /usr/include/string.h quicksort.h
EPGchannel.o: epgevent.h tapstring.h TimeSlot.h PackedDateTime.h EPGvisitor.h
EPGdata.o: epgdata.h /usr/include/stdlib.h /usr/include/features.h
EPGdata.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
EPGdata.o: /usr/include/sys/types.h /usr/include/bits/types.h
EPGdata.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
EPGdata.o: /usr/include/time.h /usr/include/endian.h
EPGdata.o: /usr/include/bits/endian.h /usr/include/sys/select.h
EPGdata.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
EPGdata.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
EPGdata.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
EPGdata.o: /usr/include/alloca.h
EPGdata.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
EPGdata.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
EPGdata.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
EPGdata.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
EPGdata.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
EPGdata.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h taparray.h
EPGdata.o: /usr/include/memory.h /usr/include/string.h quicksort.h
EPGdata.o: tapstring.h epgchannel.h epgevent.h TimeSlot.h PackedDateTime.h
EPGdata.o: Globals.h EPGdata.h Channels.h MEIReader.h ../libutils/file.h
EPGdata.o: ../libutils/Utils.h EPGReader.h ProgressNotification.h Logger.h
EPGdata.o: JagCSVReader.h
EPGevent.o: epgevent.h /usr/include/stdlib.h /usr/include/features.h
EPGevent.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
EPGevent.o: /usr/include/sys/types.h /usr/include/bits/types.h
EPGevent.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
EPGevent.o: /usr/include/time.h /usr/include/endian.h
EPGevent.o: /usr/include/bits/endian.h /usr/include/sys/select.h
EPGevent.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
EPGevent.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
EPGevent.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
EPGevent.o: /usr/include/alloca.h
EPGevent.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
EPGevent.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
EPGevent.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
EPGevent.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
EPGevent.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
EPGevent.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h tapstring.h
EPGevent.o: TimeSlot.h PackedDateTime.h globals.h EPGdata.h Channels.h
EPGevent.o: taparray.h /usr/include/memory.h /usr/include/string.h
EPGevent.o: quicksort.h timers.h /usr/include/ctype.h DirectoryUtils.h
EPGevent.o: ../libutils/file.h ../libutils/Utils.h
EPGReader.o: EPGreader.h /usr/include/stdlib.h /usr/include/features.h
EPGReader.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
EPGReader.o: /usr/include/sys/types.h /usr/include/bits/types.h
EPGReader.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
EPGReader.o: /usr/include/time.h /usr/include/endian.h
EPGReader.o: /usr/include/bits/endian.h /usr/include/sys/select.h
EPGReader.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
EPGReader.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
EPGReader.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
EPGReader.o: /usr/include/alloca.h
EPGReader.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
EPGReader.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
EPGReader.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
EPGReader.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
EPGReader.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
EPGReader.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
EPGReader.o: PackedDateTime.h EPGevent.h tapstring.h TimeSlot.h taparray.h
EPGReader.o: /usr/include/memory.h /usr/include/string.h quicksort.h
EPGReader.o: EPGdata.h globals.h channels.h
EPGSourceOption.o: epgsourceoption.h option.h /usr/include/stdlib.h
EPGSourceOption.o: /usr/include/features.h /usr/include/sys/cdefs.h
EPGSourceOption.o: /usr/include/gnu/stubs.h /usr/include/sys/types.h
EPGSourceOption.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
EPGSourceOption.o: /usr/include/bits/typesizes.h /usr/include/time.h
EPGSourceOption.o: /usr/include/endian.h /usr/include/bits/endian.h
EPGSourceOption.o: /usr/include/sys/select.h /usr/include/bits/select.h
EPGSourceOption.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
EPGSourceOption.o: /usr/include/sys/sysmacros.h
EPGSourceOption.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
EPGSourceOption.o: /usr/include/alloca.h
EPGSourceOption.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
EPGSourceOption.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
EPGSourceOption.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
EPGSourceOption.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
EPGSourceOption.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
EPGSourceOption.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
EPGSourceOption.o: tapstring.h taparray.h /usr/include/memory.h
EPGSourceOption.o: /usr/include/string.h quicksort.h EPGdata.h
EPGVisitor.o: epgvisitor.h /usr/include/stdlib.h /usr/include/features.h
EPGVisitor.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
EPGVisitor.o: /usr/include/sys/types.h /usr/include/bits/types.h
EPGVisitor.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
EPGVisitor.o: /usr/include/time.h /usr/include/endian.h
EPGVisitor.o: /usr/include/bits/endian.h /usr/include/sys/select.h
EPGVisitor.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
EPGVisitor.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
EPGVisitor.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
EPGVisitor.o: /usr/include/alloca.h
Globals.o: globals.h EPGdata.h Timers.h /usr/include/stdlib.h
Globals.o: /usr/include/features.h /usr/include/sys/cdefs.h
Globals.o: /usr/include/gnu/stubs.h /usr/include/sys/types.h
Globals.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
Globals.o: /usr/include/bits/typesizes.h /usr/include/time.h
Globals.o: /usr/include/endian.h /usr/include/bits/endian.h
Globals.o: /usr/include/sys/select.h /usr/include/bits/select.h
Globals.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
Globals.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
Globals.o: /usr/include/bits/sched.h /usr/include/alloca.h
Globals.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
Globals.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
Globals.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
Globals.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
Globals.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
Globals.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h taparray.h
Globals.o: /usr/include/memory.h /usr/include/string.h quicksort.h EPGData.h
Globals.o: tapstring.h Channels.h Archive.h ArchivedProgram.h timeslot.h
Globals.o: PackedDateTime.h Logger.H
IniFile.o: /usr/include/stdlib.h /usr/include/features.h
IniFile.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
IniFile.o: /usr/include/sys/types.h /usr/include/bits/types.h
IniFile.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
IniFile.o: /usr/include/time.h /usr/include/endian.h
IniFile.o: /usr/include/bits/endian.h /usr/include/sys/select.h
IniFile.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
IniFile.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
IniFile.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
IniFile.o: /usr/include/alloca.h /usr/include/string.h
IniFile.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
IniFile.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
IniFile.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
IniFile.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
IniFile.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
IniFile.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
IniFile.o: ../libutils/tapapifix.h inifile.h taparray.h /usr/include/memory.h
IniFile.o: quicksort.h tapstring.h ../libutils/file.h ../libutils/Utils.h
IniFile.o: DirectoryUtils.h logger.h
JagCSVReader.o: JagCSVreader.h MEIReader.h /usr/include/stdlib.h
JagCSVReader.o: /usr/include/features.h /usr/include/sys/cdefs.h
JagCSVReader.o: /usr/include/gnu/stubs.h /usr/include/sys/types.h
JagCSVReader.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
JagCSVReader.o: /usr/include/bits/typesizes.h /usr/include/time.h
JagCSVReader.o: /usr/include/endian.h /usr/include/bits/endian.h
JagCSVReader.o: /usr/include/sys/select.h /usr/include/bits/select.h
JagCSVReader.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
JagCSVReader.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
JagCSVReader.o: /usr/include/bits/sched.h /usr/include/alloca.h
JagCSVReader.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
JagCSVReader.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
JagCSVReader.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
JagCSVReader.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
JagCSVReader.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
JagCSVReader.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
JagCSVReader.o: ../libutils/file.h ../libutils/Utils.h EPGReader.h
JagCSVReader.o: PackedDateTime.h tapstring.h EPGevent.h TimeSlot.h
KeyDef.o: keydef.h /usr/local/topfield-gcc/topfield-sdk/include/tap.h
KeyDef.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
KeyDef.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
KeyDef.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
KeyDef.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
KeyDef.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
KeyOption.o: keyoption.h option.h /usr/include/stdlib.h
KeyOption.o: /usr/include/features.h /usr/include/sys/cdefs.h
KeyOption.o: /usr/include/gnu/stubs.h /usr/include/sys/types.h
KeyOption.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
KeyOption.o: /usr/include/bits/typesizes.h /usr/include/time.h
KeyOption.o: /usr/include/endian.h /usr/include/bits/endian.h
KeyOption.o: /usr/include/sys/select.h /usr/include/bits/select.h
KeyOption.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
KeyOption.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
KeyOption.o: /usr/include/bits/sched.h /usr/include/alloca.h
KeyOption.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
KeyOption.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
KeyOption.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
KeyOption.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
KeyOption.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
KeyOption.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h tapstring.h
KeyOption.o: taparray.h /usr/include/memory.h /usr/include/string.h
KeyOption.o: quicksort.h KeyDef.h ../libutils/morekeys.h
Logger.o: logger.h /usr/local/topfield-gcc/topfield-sdk/include/tap.h
Logger.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
Logger.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
Logger.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
Logger.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
Logger.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
Logger.o: ../libutils/file.h ../libutils/Utils.h
LoggingOption.o: LoggingOption.h option.h /usr/include/stdlib.h
LoggingOption.o: /usr/include/features.h /usr/include/sys/cdefs.h
LoggingOption.o: /usr/include/gnu/stubs.h /usr/include/sys/types.h
LoggingOption.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
LoggingOption.o: /usr/include/bits/typesizes.h /usr/include/time.h
LoggingOption.o: /usr/include/endian.h /usr/include/bits/endian.h
LoggingOption.o: /usr/include/sys/select.h /usr/include/bits/select.h
LoggingOption.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
LoggingOption.o: /usr/include/sys/sysmacros.h
LoggingOption.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
LoggingOption.o: /usr/include/alloca.h
LoggingOption.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
LoggingOption.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
LoggingOption.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
LoggingOption.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
LoggingOption.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
LoggingOption.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
LoggingOption.o: tapstring.h taparray.h /usr/include/memory.h
LoggingOption.o: /usr/include/string.h quicksort.h Logger.h
MEIReader.o: meireader.h /usr/include/stdlib.h /usr/include/features.h
MEIReader.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
MEIReader.o: /usr/include/sys/types.h /usr/include/bits/types.h
MEIReader.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
MEIReader.o: /usr/include/time.h /usr/include/endian.h
MEIReader.o: /usr/include/bits/endian.h /usr/include/sys/select.h
MEIReader.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
MEIReader.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
MEIReader.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
MEIReader.o: /usr/include/alloca.h
MEIReader.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
MEIReader.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
MEIReader.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
MEIReader.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
MEIReader.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
MEIReader.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
MEIReader.o: ../libutils/file.h ../libutils/Utils.h EPGReader.h
MEIReader.o: PackedDateTime.h tapstring.h EPGevent.h TimeSlot.h taparray.h
MEIReader.o: /usr/include/memory.h /usr/include/string.h quicksort.h
MEIReader.o: EPGdata.h DirectoryUtils.h
NumericOption.o: numericoption.h option.h /usr/include/stdlib.h
NumericOption.o: /usr/include/features.h /usr/include/sys/cdefs.h
NumericOption.o: /usr/include/gnu/stubs.h /usr/include/sys/types.h
NumericOption.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
NumericOption.o: /usr/include/bits/typesizes.h /usr/include/time.h
NumericOption.o: /usr/include/endian.h /usr/include/bits/endian.h
NumericOption.o: /usr/include/sys/select.h /usr/include/bits/select.h
NumericOption.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
NumericOption.o: /usr/include/sys/sysmacros.h
NumericOption.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
NumericOption.o: /usr/include/alloca.h
NumericOption.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
NumericOption.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
NumericOption.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
NumericOption.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
NumericOption.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
NumericOption.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
NumericOption.o: tapstring.h taparray.h /usr/include/memory.h
NumericOption.o: /usr/include/string.h quicksort.h
Option.o: option.h /usr/include/stdlib.h /usr/include/features.h
Option.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Option.o: /usr/include/sys/types.h /usr/include/bits/types.h
Option.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
Option.o: /usr/include/time.h /usr/include/endian.h
Option.o: /usr/include/bits/endian.h /usr/include/sys/select.h
Option.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
Option.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
Option.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
Option.o: /usr/include/alloca.h
Option.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
Option.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
Option.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
Option.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
Option.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
Option.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h tapstring.h
Option.o: taparray.h /usr/include/memory.h /usr/include/string.h quicksort.h
Option.o: /usr/include/ctype.h ../libutils/ini.h Options.h
Option.o: OptionUpdateNotifier.h KeyDef.h
Options.o: options.h /usr/include/stdlib.h /usr/include/features.h
Options.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Options.o: /usr/include/sys/types.h /usr/include/bits/types.h
Options.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
Options.o: /usr/include/time.h /usr/include/endian.h
Options.o: /usr/include/bits/endian.h /usr/include/sys/select.h
Options.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
Options.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
Options.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
Options.o: /usr/include/alloca.h
Options.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
Options.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
Options.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
Options.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
Options.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
Options.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h tapstring.h
Options.o: taparray.h /usr/include/memory.h /usr/include/string.h quicksort.h
Options.o: OptionUpdateNotifier.h option.h inifile.h
OptionUpdateNotifier.o: optionupdatenotifier.h /usr/include/stdlib.h
OptionUpdateNotifier.o: /usr/include/features.h /usr/include/sys/cdefs.h
OptionUpdateNotifier.o: /usr/include/gnu/stubs.h /usr/include/sys/types.h
OptionUpdateNotifier.o: /usr/include/bits/types.h
OptionUpdateNotifier.o: /usr/include/bits/wordsize.h
OptionUpdateNotifier.o: /usr/include/bits/typesizes.h /usr/include/time.h
OptionUpdateNotifier.o: /usr/include/endian.h /usr/include/bits/endian.h
OptionUpdateNotifier.o: /usr/include/sys/select.h /usr/include/bits/select.h
OptionUpdateNotifier.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
OptionUpdateNotifier.o: /usr/include/sys/sysmacros.h
OptionUpdateNotifier.o: /usr/include/bits/pthreadtypes.h
OptionUpdateNotifier.o: /usr/include/bits/sched.h /usr/include/alloca.h
OptionUpdateNotifier.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
OptionUpdateNotifier.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
OptionUpdateNotifier.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
OptionUpdateNotifier.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
OptionUpdateNotifier.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
OptionUpdateNotifier.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
OptionUpdateNotifier.o: tapstring.h option.h taparray.h /usr/include/memory.h
OptionUpdateNotifier.o: /usr/include/string.h quicksort.h KeyDef.h
PackedDateTime.o: packeddatetime.h /usr/include/stdlib.h
PackedDateTime.o: /usr/include/features.h /usr/include/sys/cdefs.h
PackedDateTime.o: /usr/include/gnu/stubs.h /usr/include/sys/types.h
PackedDateTime.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
PackedDateTime.o: /usr/include/bits/typesizes.h /usr/include/time.h
PackedDateTime.o: /usr/include/endian.h /usr/include/bits/endian.h
PackedDateTime.o: /usr/include/sys/select.h /usr/include/bits/select.h
PackedDateTime.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
PackedDateTime.o: /usr/include/sys/sysmacros.h
PackedDateTime.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
PackedDateTime.o: /usr/include/alloca.h
PackedDateTime.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
PackedDateTime.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
PackedDateTime.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
PackedDateTime.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
PackedDateTime.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
PackedDateTime.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
PackedDateTime.o: tapstring.h
PerformanceTimer.o: performancetimer.h
PerformanceTimer.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
PerformanceTimer.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
PerformanceTimer.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
PerformanceTimer.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
PerformanceTimer.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
PerformanceTimer.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
PerformanceTimer.o: tapstring.h logger.h
ProgressNotification.o: ProgressNotification.h
stringarchive.o: stringarchive.h tapstring.h /usr/include/stdlib.h
stringarchive.o: /usr/include/features.h /usr/include/sys/cdefs.h
stringarchive.o: /usr/include/gnu/stubs.h /usr/include/sys/types.h
stringarchive.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
stringarchive.o: /usr/include/bits/typesizes.h /usr/include/time.h
stringarchive.o: /usr/include/endian.h /usr/include/bits/endian.h
stringarchive.o: /usr/include/sys/select.h /usr/include/bits/select.h
stringarchive.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
stringarchive.o: /usr/include/sys/sysmacros.h
stringarchive.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
stringarchive.o: /usr/include/alloca.h
stringarchive.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
stringarchive.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
stringarchive.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
stringarchive.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
stringarchive.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
stringarchive.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
stringarchive.o: /usr/include/string.h /usr/include/stdio.h
stringarchive.o: /usr/include/libio.h /usr/include/_G_config.h
stringarchive.o: /usr/include/wchar.h /usr/include/bits/wchar.h
stringarchive.o: /usr/include/gconv.h /usr/include/bits/stdio_lim.h
stringarchive.o: /usr/include/bits/sys_errlist.h logger.h
taparray.o: taparray.h /usr/local/topfield-gcc/topfield-sdk/include/tap.h
taparray.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
taparray.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
taparray.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
taparray.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
taparray.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
taparray.o: /usr/include/memory.h /usr/include/features.h
taparray.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
taparray.o: /usr/include/string.h quicksort.h
tapstring.o: /usr/include/string.h /usr/include/features.h
tapstring.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
tapstring.o: /usr/include/stdio.h /usr/include/bits/types.h
tapstring.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
tapstring.o: /usr/include/libio.h /usr/include/_G_config.h
tapstring.o: /usr/include/wchar.h /usr/include/bits/wchar.h
tapstring.o: /usr/include/gconv.h /usr/include/bits/stdio_lim.h
tapstring.o: /usr/include/bits/sys_errlist.h /usr/include/ctype.h
tapstring.o: /usr/include/endian.h /usr/include/bits/endian.h taparray.h
tapstring.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
tapstring.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
tapstring.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
tapstring.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
tapstring.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
tapstring.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
tapstring.o: /usr/include/memory.h quicksort.h tapstring.h
Timer.o: timer.h /usr/include/stdlib.h /usr/include/features.h
Timer.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Timer.o: /usr/include/sys/types.h /usr/include/bits/types.h
Timer.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
Timer.o: /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h
Timer.o: /usr/include/sys/select.h /usr/include/bits/select.h
Timer.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
Timer.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
Timer.o: /usr/include/bits/sched.h /usr/include/alloca.h
Timer.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
Timer.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
Timer.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
Timer.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
Timer.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
Timer.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h Timeslot.h
Timer.o: PackedDateTime.h tapstring.h Channels.h taparray.h
Timer.o: /usr/include/memory.h /usr/include/string.h quicksort.h EPGevent.h
Timer.o: TimeSlot.h globals.h EPGdata.h EPGchannel.h Timers.h
Timers.o: timers.h /usr/include/stdlib.h /usr/include/features.h
Timers.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Timers.o: /usr/include/sys/types.h /usr/include/bits/types.h
Timers.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
Timers.o: /usr/include/time.h /usr/include/endian.h
Timers.o: /usr/include/bits/endian.h /usr/include/sys/select.h
Timers.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
Timers.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
Timers.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
Timers.o: /usr/include/alloca.h
Timers.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
Timers.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
Timers.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
Timers.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
Timers.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
Timers.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h taparray.h
Timers.o: /usr/include/memory.h /usr/include/string.h quicksort.h Timer.h
Timers.o: Timeslot.h PackedDateTime.h tapstring.h EPGevent.h TimeSlot.h
Timers.o: Channels.h Globals.h EPGdata.h Logger.h
TimeSlot.o: timeslot.h /usr/include/stdlib.h /usr/include/features.h
TimeSlot.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TimeSlot.o: /usr/include/sys/types.h /usr/include/bits/types.h
TimeSlot.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
TimeSlot.o: /usr/include/time.h /usr/include/endian.h
TimeSlot.o: /usr/include/bits/endian.h /usr/include/sys/select.h
TimeSlot.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
TimeSlot.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
TimeSlot.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
TimeSlot.o: /usr/include/alloca.h PackedDateTime.h
TimeSlot.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
TimeSlot.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
TimeSlot.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
TimeSlot.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
TimeSlot.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
TimeSlot.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h tapstring.h
vtable.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
vtable.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
vtable.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
vtable.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
vtable.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
vtable.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h vtable.h
vtable.o: Logger.h ../libutils/dlmalloc.h
YesNoOption.o: yesnooption.h option.h /usr/include/stdlib.h
YesNoOption.o: /usr/include/features.h /usr/include/sys/cdefs.h
YesNoOption.o: /usr/include/gnu/stubs.h /usr/include/sys/types.h
YesNoOption.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
YesNoOption.o: /usr/include/bits/typesizes.h /usr/include/time.h
YesNoOption.o: /usr/include/endian.h /usr/include/bits/endian.h
YesNoOption.o: /usr/include/sys/select.h /usr/include/bits/select.h
YesNoOption.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
YesNoOption.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
YesNoOption.o: /usr/include/bits/sched.h /usr/include/alloca.h
YesNoOption.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
YesNoOption.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
YesNoOption.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
YesNoOption.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
YesNoOption.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
YesNoOption.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h tapstring.h
YesNoOption.o: taparray.h /usr/include/memory.h /usr/include/string.h
YesNoOption.o: quicksort.h
