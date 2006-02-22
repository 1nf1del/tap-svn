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

Archive.o: Archive.h /usr/include/stdlib.h /usr/include/_ansi.h
Archive.o: /usr/include/newlib.h /usr/include/sys/config.h
Archive.o: /usr/include/machine/ieeefp.h /usr/local/include/stddef.h
Archive.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
Archive.o: /usr/include/sys/lock.h /usr/include/machine/stdlib.h
Archive.o: /usr/include/alloca.h ArchivedProgram.h tapstring.h
Archive.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
Archive.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
Archive.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
Archive.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
Archive.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
Archive.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h taparray.h
Archive.o: /usr/include/memory.h /usr/include/string.h
Archive.o: /usr/include/sys/string.h quicksort.h DirectoryUtils.h
Archive.o: ../libutils/file.h ../libutils/Utils.h /usr/local/include/stdarg.h
ArchivedProgram.o: archivedprogram.h tapstring.h
ArchivedProgram.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
ArchivedProgram.o: /usr/local/include/stddef.h
ArchivedProgram.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
ArchivedProgram.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
ArchivedProgram.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
ArchivedProgram.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
ArchivedProgram.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
ArchivedProgram.o: ../libutils/file.h ../libutils/Utils.h
ArchivedProgram.o: /usr/local/include/stdarg.h
Channels.o: .\channels.h /usr/include/stdlib.h /usr/include/_ansi.h
Channels.o: /usr/include/newlib.h /usr/include/sys/config.h
Channels.o: /usr/include/machine/ieeefp.h /usr/local/include/stddef.h
Channels.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
Channels.o: /usr/include/sys/lock.h /usr/include/machine/stdlib.h
Channels.o: /usr/include/alloca.h
Channels.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
Channels.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
Channels.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
Channels.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
Channels.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
Channels.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h tapstring.h
Channels.o: taparray.h /usr/include/memory.h /usr/include/string.h
Channels.o: /usr/include/sys/string.h quicksort.h
DirectoryUtils.o: DirectoryUtils.h /usr/include/string.h /usr/include/_ansi.h
DirectoryUtils.o: /usr/include/newlib.h /usr/include/sys/config.h
DirectoryUtils.o: /usr/include/machine/ieeefp.h /usr/include/sys/reent.h
DirectoryUtils.o: /usr/include/sys/_types.h /usr/include/sys/lock.h
DirectoryUtils.o: /usr/local/include/stddef.h /usr/include/sys/string.h
DirectoryUtils.o: /usr/include/stdlib.h /usr/include/machine/stdlib.h
DirectoryUtils.o: /usr/include/alloca.h
DirectoryUtils.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
DirectoryUtils.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
DirectoryUtils.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
DirectoryUtils.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
DirectoryUtils.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
DirectoryUtils.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
DirectoryUtils.o: tapstring.h ../libutils/file.h ../libutils/Utils.h
DirectoryUtils.o: /usr/local/include/stdarg.h taparray.h
DirectoryUtils.o: /usr/include/memory.h quicksort.h
EPGReader.o: EPGreader.h /usr/include/stdlib.h /usr/include/_ansi.h
EPGReader.o: /usr/include/newlib.h /usr/include/sys/config.h
EPGReader.o: /usr/include/machine/ieeefp.h /usr/local/include/stddef.h
EPGReader.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
EPGReader.o: /usr/include/sys/lock.h /usr/include/machine/stdlib.h
EPGReader.o: /usr/include/alloca.h
EPGReader.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
EPGReader.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
EPGReader.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
EPGReader.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
EPGReader.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
EPGReader.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h EPGevent.h
EPGReader.o: tapstring.h TimeSlot.h PackedDateTime.h taparray.h
EPGReader.o: /usr/include/memory.h /usr/include/string.h
EPGReader.o: /usr/include/sys/string.h quicksort.h EPGdata.h globals.h
EPGReader.o: channels.h
EPGSourceOption.o: .\epgsourceoption.h option.h /usr/include/stdlib.h
EPGSourceOption.o: /usr/include/_ansi.h /usr/include/newlib.h
EPGSourceOption.o: /usr/include/sys/config.h /usr/include/machine/ieeefp.h
EPGSourceOption.o: /usr/local/include/stddef.h /usr/include/sys/reent.h
EPGSourceOption.o: /usr/include/sys/_types.h /usr/include/sys/lock.h
EPGSourceOption.o: /usr/include/machine/stdlib.h /usr/include/alloca.h
EPGSourceOption.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
EPGSourceOption.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
EPGSourceOption.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
EPGSourceOption.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
EPGSourceOption.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
EPGSourceOption.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
EPGSourceOption.o: tapstring.h taparray.h /usr/include/memory.h
EPGSourceOption.o: /usr/include/string.h /usr/include/sys/string.h
EPGSourceOption.o: quicksort.h EPGdata.h
EPGVisitor.o: .\epgvisitor.h /usr/include/stdlib.h /usr/include/_ansi.h
EPGVisitor.o: /usr/include/newlib.h /usr/include/sys/config.h
EPGVisitor.o: /usr/include/machine/ieeefp.h /usr/local/include/stddef.h
EPGVisitor.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
EPGVisitor.o: /usr/include/sys/lock.h /usr/include/machine/stdlib.h
EPGVisitor.o: /usr/include/alloca.h
EPGchannel.o: .\epgchannel.h /usr/include/stdlib.h /usr/include/_ansi.h
EPGchannel.o: /usr/include/newlib.h /usr/include/sys/config.h
EPGchannel.o: /usr/include/machine/ieeefp.h /usr/local/include/stddef.h
EPGchannel.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
EPGchannel.o: /usr/include/sys/lock.h /usr/include/machine/stdlib.h
EPGchannel.o: /usr/include/alloca.h
EPGchannel.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
EPGchannel.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
EPGchannel.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
EPGchannel.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
EPGchannel.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
EPGchannel.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h taparray.h
EPGchannel.o: /usr/include/memory.h /usr/include/string.h
EPGchannel.o: /usr/include/sys/string.h quicksort.h epgevent.h tapstring.h
EPGchannel.o: TimeSlot.h PackedDateTime.h EPGvisitor.h
EPGdata.o: .\epgdata.h /usr/include/stdlib.h /usr/include/_ansi.h
EPGdata.o: /usr/include/newlib.h /usr/include/sys/config.h
EPGdata.o: /usr/include/machine/ieeefp.h /usr/local/include/stddef.h
EPGdata.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
EPGdata.o: /usr/include/sys/lock.h /usr/include/machine/stdlib.h
EPGdata.o: /usr/include/alloca.h
EPGdata.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
EPGdata.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
EPGdata.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
EPGdata.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
EPGdata.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
EPGdata.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h taparray.h
EPGdata.o: /usr/include/memory.h /usr/include/string.h
EPGdata.o: /usr/include/sys/string.h quicksort.h tapstring.h epgchannel.h
EPGdata.o: epgevent.h TimeSlot.h PackedDateTime.h Globals.h EPGdata.h
EPGdata.o: Channels.h MEIReader.h ../libutils/file.h ../libutils/Utils.h
EPGdata.o: /usr/local/include/stdarg.h EPGReader.h ProgressNotification.h
EPGdata.o: Logger.h JagCSVReader.h
EPGevent.o: .\epgevent.h /usr/include/stdlib.h /usr/include/_ansi.h
EPGevent.o: /usr/include/newlib.h /usr/include/sys/config.h
EPGevent.o: /usr/include/machine/ieeefp.h /usr/local/include/stddef.h
EPGevent.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
EPGevent.o: /usr/include/sys/lock.h /usr/include/machine/stdlib.h
EPGevent.o: /usr/include/alloca.h
EPGevent.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
EPGevent.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
EPGevent.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
EPGevent.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
EPGevent.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
EPGevent.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h tapstring.h
EPGevent.o: TimeSlot.h PackedDateTime.h globals.h EPGdata.h taparray.h
EPGevent.o: /usr/include/memory.h /usr/include/string.h
EPGevent.o: /usr/include/sys/string.h quicksort.h Channels.h timers.h
EPGevent.o: /usr/include/ctype.h
Globals.o: .\globals.h EPGdata.h /usr/include/stdlib.h /usr/include/_ansi.h
Globals.o: /usr/include/newlib.h /usr/include/sys/config.h
Globals.o: /usr/include/machine/ieeefp.h /usr/local/include/stddef.h
Globals.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
Globals.o: /usr/include/sys/lock.h /usr/include/machine/stdlib.h
Globals.o: /usr/include/alloca.h
Globals.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
Globals.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
Globals.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
Globals.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
Globals.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
Globals.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h taparray.h
Globals.o: /usr/include/memory.h /usr/include/string.h
Globals.o: /usr/include/sys/string.h quicksort.h tapstring.h Timers.h
Globals.o: EPGData.h Channels.h Archive.h ArchivedProgram.h
IniFile.o: /usr/include/stdlib.h /usr/include/_ansi.h /usr/include/newlib.h
IniFile.o: /usr/include/sys/config.h /usr/include/machine/ieeefp.h
IniFile.o: /usr/local/include/stddef.h /usr/include/sys/reent.h
IniFile.o: /usr/include/sys/_types.h /usr/include/sys/lock.h
IniFile.o: /usr/include/machine/stdlib.h /usr/include/alloca.h
IniFile.o: /usr/include/string.h /usr/include/sys/string.h
IniFile.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
IniFile.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
IniFile.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
IniFile.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
IniFile.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
IniFile.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
IniFile.o: ../libutils/tapapifix.h inifile.h taparray.h /usr/include/memory.h
IniFile.o: quicksort.h tapstring.h
JagCSVReader.o: JagCSVreader.h MEIReader.h /usr/include/stdlib.h
JagCSVReader.o: /usr/include/_ansi.h /usr/include/newlib.h
JagCSVReader.o: /usr/include/sys/config.h /usr/include/machine/ieeefp.h
JagCSVReader.o: /usr/local/include/stddef.h /usr/include/sys/reent.h
JagCSVReader.o: /usr/include/sys/_types.h /usr/include/sys/lock.h
JagCSVReader.o: /usr/include/machine/stdlib.h /usr/include/alloca.h
JagCSVReader.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
JagCSVReader.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
JagCSVReader.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
JagCSVReader.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
JagCSVReader.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
JagCSVReader.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
JagCSVReader.o: ../libutils/file.h ../libutils/Utils.h
JagCSVReader.o: /usr/local/include/stdarg.h EPGReader.h tapstring.h
JagCSVReader.o: EPGevent.h TimeSlot.h PackedDateTime.h
KeyDef.o: .\keydef.h /usr/local/topfield-gcc/topfield-sdk/include/tap.h
KeyDef.o: /usr/local/include/stddef.h
KeyDef.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
KeyDef.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
KeyDef.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
KeyDef.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
KeyDef.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
KeyOption.o: .\keyoption.h option.h /usr/include/stdlib.h
KeyOption.o: /usr/include/_ansi.h /usr/include/newlib.h
KeyOption.o: /usr/include/sys/config.h /usr/include/machine/ieeefp.h
KeyOption.o: /usr/local/include/stddef.h /usr/include/sys/reent.h
KeyOption.o: /usr/include/sys/_types.h /usr/include/sys/lock.h
KeyOption.o: /usr/include/machine/stdlib.h /usr/include/alloca.h
KeyOption.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
KeyOption.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
KeyOption.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
KeyOption.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
KeyOption.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
KeyOption.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h tapstring.h
KeyOption.o: taparray.h /usr/include/memory.h /usr/include/string.h
KeyOption.o: /usr/include/sys/string.h quicksort.h KeyDef.h
KeyOption.o: ..\..\..\trunk\libs\Firmware\RawKey.h
Logger.o: .\logger.h /usr/local/topfield-gcc/topfield-sdk/include/tap.h
Logger.o: /usr/local/include/stddef.h
Logger.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
Logger.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
Logger.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
Logger.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
Logger.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
Logger.o: /usr/local/include/stdarg.h ../libutils/file.h ../libutils/Utils.h
MEIReader.o: .\meireader.h /usr/include/stdlib.h /usr/include/_ansi.h
MEIReader.o: /usr/include/newlib.h /usr/include/sys/config.h
MEIReader.o: /usr/include/machine/ieeefp.h /usr/local/include/stddef.h
MEIReader.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
MEIReader.o: /usr/include/sys/lock.h /usr/include/machine/stdlib.h
MEIReader.o: /usr/include/alloca.h
MEIReader.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
MEIReader.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
MEIReader.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
MEIReader.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
MEIReader.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
MEIReader.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
MEIReader.o: ../libutils/file.h ../libutils/Utils.h
MEIReader.o: /usr/local/include/stdarg.h EPGReader.h tapstring.h EPGevent.h
MEIReader.o: TimeSlot.h PackedDateTime.h taparray.h /usr/include/memory.h
MEIReader.o: /usr/include/string.h /usr/include/sys/string.h quicksort.h
MEIReader.o: EPGdata.h DirectoryUtils.h
NumericOption.o: .\numericoption.h option.h /usr/include/stdlib.h
NumericOption.o: /usr/include/_ansi.h /usr/include/newlib.h
NumericOption.o: /usr/include/sys/config.h /usr/include/machine/ieeefp.h
NumericOption.o: /usr/local/include/stddef.h /usr/include/sys/reent.h
NumericOption.o: /usr/include/sys/_types.h /usr/include/sys/lock.h
NumericOption.o: /usr/include/machine/stdlib.h /usr/include/alloca.h
NumericOption.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
NumericOption.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
NumericOption.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
NumericOption.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
NumericOption.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
NumericOption.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
NumericOption.o: tapstring.h taparray.h /usr/include/memory.h
NumericOption.o: /usr/include/string.h /usr/include/sys/string.h quicksort.h
Option.o: .\option.h /usr/include/stdlib.h /usr/include/_ansi.h
Option.o: /usr/include/newlib.h /usr/include/sys/config.h
Option.o: /usr/include/machine/ieeefp.h /usr/local/include/stddef.h
Option.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
Option.o: /usr/include/sys/lock.h /usr/include/machine/stdlib.h
Option.o: /usr/include/alloca.h
Option.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
Option.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
Option.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
Option.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
Option.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
Option.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h tapstring.h
Option.o: taparray.h /usr/include/memory.h /usr/include/string.h
Option.o: /usr/include/sys/string.h quicksort.h /usr/include/ctype.h
Option.o: ../libutils/ini.h Options.h OptionUpdateNotifier.h KeyDef.h
OptionUpdateNotifier.o: .\optionupdatenotifier.h /usr/include/stdlib.h
OptionUpdateNotifier.o: /usr/include/_ansi.h /usr/include/newlib.h
OptionUpdateNotifier.o: /usr/include/sys/config.h
OptionUpdateNotifier.o: /usr/include/machine/ieeefp.h
OptionUpdateNotifier.o: /usr/local/include/stddef.h /usr/include/sys/reent.h
OptionUpdateNotifier.o: /usr/include/sys/_types.h /usr/include/sys/lock.h
OptionUpdateNotifier.o: /usr/include/machine/stdlib.h /usr/include/alloca.h
OptionUpdateNotifier.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
OptionUpdateNotifier.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
OptionUpdateNotifier.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
OptionUpdateNotifier.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
OptionUpdateNotifier.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
OptionUpdateNotifier.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
OptionUpdateNotifier.o: tapstring.h option.h taparray.h /usr/include/memory.h
OptionUpdateNotifier.o: /usr/include/string.h /usr/include/sys/string.h
OptionUpdateNotifier.o: quicksort.h KeyDef.h
Options.o: .\options.h /usr/include/stdlib.h /usr/include/_ansi.h
Options.o: /usr/include/newlib.h /usr/include/sys/config.h
Options.o: /usr/include/machine/ieeefp.h /usr/local/include/stddef.h
Options.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
Options.o: /usr/include/sys/lock.h /usr/include/machine/stdlib.h
Options.o: /usr/include/alloca.h
Options.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
Options.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
Options.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
Options.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
Options.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
Options.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h tapstring.h
Options.o: taparray.h /usr/include/memory.h /usr/include/string.h
Options.o: /usr/include/sys/string.h quicksort.h OptionUpdateNotifier.h
Options.o: option.h inifile.h
PackedDateTime.o: .\packeddatetime.h /usr/include/stdlib.h
PackedDateTime.o: /usr/include/_ansi.h /usr/include/newlib.h
PackedDateTime.o: /usr/include/sys/config.h /usr/include/machine/ieeefp.h
PackedDateTime.o: /usr/local/include/stddef.h /usr/include/sys/reent.h
PackedDateTime.o: /usr/include/sys/_types.h /usr/include/sys/lock.h
PackedDateTime.o: /usr/include/machine/stdlib.h /usr/include/alloca.h
PackedDateTime.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
PackedDateTime.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
PackedDateTime.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
PackedDateTime.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
PackedDateTime.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
PackedDateTime.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
PackedDateTime.o: tapstring.h
PerformanceTimer.o: .\performancetimer.h
PerformanceTimer.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
PerformanceTimer.o: /usr/local/include/stddef.h
PerformanceTimer.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
PerformanceTimer.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
PerformanceTimer.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
PerformanceTimer.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
PerformanceTimer.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
PerformanceTimer.o: tapstring.h logger.h /usr/local/include/stdarg.h
ProgressNotification.o: ProgressNotification.h
TimeSlot.o: .\timeslot.h /usr/include/stdlib.h /usr/include/_ansi.h
TimeSlot.o: /usr/include/newlib.h /usr/include/sys/config.h
TimeSlot.o: /usr/include/machine/ieeefp.h /usr/local/include/stddef.h
TimeSlot.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
TimeSlot.o: /usr/include/sys/lock.h /usr/include/machine/stdlib.h
TimeSlot.o: /usr/include/alloca.h PackedDateTime.h
TimeSlot.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
TimeSlot.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
TimeSlot.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
TimeSlot.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
TimeSlot.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
TimeSlot.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h tapstring.h
Timer.o: .\timer.h /usr/include/stdlib.h /usr/include/_ansi.h
Timer.o: /usr/include/newlib.h /usr/include/sys/config.h
Timer.o: /usr/include/machine/ieeefp.h /usr/local/include/stddef.h
Timer.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
Timer.o: /usr/include/sys/lock.h /usr/include/machine/stdlib.h
Timer.o: /usr/include/alloca.h
Timer.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
Timer.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
Timer.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
Timer.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
Timer.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
Timer.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h Timeslot.h
Timer.o: PackedDateTime.h tapstring.h Channels.h taparray.h
Timer.o: /usr/include/memory.h /usr/include/string.h
Timer.o: /usr/include/sys/string.h quicksort.h EPGevent.h TimeSlot.h
Timer.o: globals.h EPGdata.h EPGchannel.h Timers.h
Timers.o: .\timers.h /usr/include/stdlib.h /usr/include/_ansi.h
Timers.o: /usr/include/newlib.h /usr/include/sys/config.h
Timers.o: /usr/include/machine/ieeefp.h /usr/local/include/stddef.h
Timers.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
Timers.o: /usr/include/sys/lock.h /usr/include/machine/stdlib.h
Timers.o: /usr/include/alloca.h
Timers.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
Timers.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
Timers.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
Timers.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
Timers.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
Timers.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h taparray.h
Timers.o: /usr/include/memory.h /usr/include/string.h
Timers.o: /usr/include/sys/string.h quicksort.h Timer.h Timeslot.h
Timers.o: PackedDateTime.h tapstring.h EPGevent.h TimeSlot.h Channels.h
Timers.o: Globals.h EPGdata.h Logger.h /usr/local/include/stdarg.h
YesNoOption.o: .\yesnooption.h option.h /usr/include/stdlib.h
YesNoOption.o: /usr/include/_ansi.h /usr/include/newlib.h
YesNoOption.o: /usr/include/sys/config.h /usr/include/machine/ieeefp.h
YesNoOption.o: /usr/local/include/stddef.h /usr/include/sys/reent.h
YesNoOption.o: /usr/include/sys/_types.h /usr/include/sys/lock.h
YesNoOption.o: /usr/include/machine/stdlib.h /usr/include/alloca.h
YesNoOption.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
YesNoOption.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
YesNoOption.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
YesNoOption.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
YesNoOption.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
YesNoOption.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h tapstring.h
YesNoOption.o: taparray.h /usr/include/memory.h /usr/include/string.h
YesNoOption.o: /usr/include/sys/string.h quicksort.h
stringarchive.o: stringarchive.h tapstring.h /usr/include/stdlib.h
stringarchive.o: /usr/include/_ansi.h /usr/include/newlib.h
stringarchive.o: /usr/include/sys/config.h /usr/include/machine/ieeefp.h
stringarchive.o: /usr/local/include/stddef.h /usr/include/sys/reent.h
stringarchive.o: /usr/include/sys/_types.h /usr/include/sys/lock.h
stringarchive.o: /usr/include/machine/stdlib.h /usr/include/alloca.h
stringarchive.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
stringarchive.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
stringarchive.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
stringarchive.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
stringarchive.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
stringarchive.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
stringarchive.o: /usr/include/string.h /usr/include/sys/string.h
stringarchive.o: /usr/include/stdio.h /usr/local/include/stdarg.h
stringarchive.o: /usr/include/sys/types.h /usr/include/machine/_types.h
stringarchive.o: /usr/include/limits.h /usr/include/features.h
stringarchive.o: /usr/include/sys/cdefs.h /usr/include/sys/features.h
stringarchive.o: /usr/include/machine/types.h /usr/include/sys/stdio.h
stringarchive.o: logger.h
taparray.o: .\taparray.h /usr/local/topfield-gcc/topfield-sdk/include/tap.h
taparray.o: /usr/local/include/stddef.h
taparray.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
taparray.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
taparray.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
taparray.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
taparray.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
taparray.o: /usr/include/memory.h /usr/include/string.h /usr/include/_ansi.h
taparray.o: /usr/include/newlib.h /usr/include/sys/config.h
taparray.o: /usr/include/machine/ieeefp.h /usr/include/sys/reent.h
taparray.o: /usr/include/sys/_types.h /usr/include/sys/lock.h
taparray.o: /usr/include/sys/string.h quicksort.h
tapstring.o: /usr/include/string.h /usr/include/_ansi.h /usr/include/newlib.h
tapstring.o: /usr/include/sys/config.h /usr/include/machine/ieeefp.h
tapstring.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
tapstring.o: /usr/include/sys/lock.h /usr/local/include/stddef.h
tapstring.o: /usr/include/sys/string.h /usr/include/stdio.h
tapstring.o: /usr/local/include/stdarg.h /usr/include/sys/types.h
tapstring.o: /usr/include/machine/_types.h /usr/include/limits.h
tapstring.o: /usr/include/features.h /usr/include/sys/cdefs.h
tapstring.o: /usr/include/sys/features.h /usr/include/machine/types.h
tapstring.o: /usr/include/sys/stdio.h /usr/include/ctype.h taparray.h
tapstring.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
tapstring.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
tapstring.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
tapstring.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
tapstring.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
tapstring.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
tapstring.o: /usr/include/memory.h quicksort.h .\tapstring.h
vtable.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
vtable.o: /usr/local/include/stddef.h
vtable.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
vtable.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
vtable.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
vtable.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
vtable.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h vtable.h
vtable.o: Logger.h /usr/local/include/stdarg.h ../libutils/dlmalloc.h
