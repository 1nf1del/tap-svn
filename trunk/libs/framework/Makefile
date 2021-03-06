# Copyright (c) 2005 Simon Capewell. All Rights Reserved.
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

INCLUDEDIRS=-I ../libc++utils -I ../libutils
CXXFLAGS+= $(INCLUDEDIRS)
OBJECTS=Main.o Region.o Tapplication.o Page.o BitmapPage.o TextTools.o Dialog.o graphics.o YesNoBox.o DialogBox.o ProgressBox.o
OBJECTS+=ListPage.o Rect.o ListItem.o ListColumn.o SimpleTextListItem.o ActionListItem.o
OBJECTS+=OptionsPage.o SkinOption.o OptionListItem.o MultiLineListItem.o FooterListItem.o
OBJECTS+=Decorator.o SkinDecorator.o
LIBRARIES=../libutils/libutils.a ../libc++utils/libc++utils.a
LIBDIRS=-L ../libutils -L ../libc++utils

all: libFramework.a

include ../libc++utils/tap++.mk

LDFLAGS:= -u TAP_Main $(LIBDIRS) -lc++utils -lutils $(LDFLAGS)

libFramework.a:  $(OBJECTS)  $(TEMPLATESBUILTFLAG)
	$(AR) cr $@ $(OBJECTS)

clean:
	-rm -f *.a *.o *.elf *.rpo buildtemplates.done

depends:
	makedepend -I $(TAP_HOME)/include $(INCLUDEDIRS) -Y/usr/include -Y/usr/local/include -I/usr/include *.cpp

# DO NOT DELETE

ActionListItem.o: actionlistitem.h SimpleTextListItem.h listitem.h
ActionListItem.o: /usr/include/stdlib.h /usr/include/features.h
ActionListItem.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
ActionListItem.o: /usr/include/sys/types.h /usr/include/bits/types.h
ActionListItem.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
ActionListItem.o: /usr/include/time.h /usr/include/endian.h
ActionListItem.o: /usr/include/bits/endian.h /usr/include/sys/select.h
ActionListItem.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
ActionListItem.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
ActionListItem.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
ActionListItem.o: /usr/include/alloca.h
ActionListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
ActionListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
ActionListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
ActionListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
ActionListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
ActionListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h Rect.h
ActionListItem.o: ../libc++utils/tapstring.h
BitmapPage.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
BitmapPage.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
BitmapPage.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
BitmapPage.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
BitmapPage.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
BitmapPage.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
BitmapPage.o: /usr/include/string.h /usr/include/features.h
BitmapPage.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
BitmapPage.o: Tapplication.h colors.h BitmapPage.h Page.h
Decorator.o: decorator.h /usr/local/topfield-gcc/topfield-sdk/include/tap.h
Decorator.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
Decorator.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
Decorator.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
Decorator.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
Decorator.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h colors.h
Decorator.o: graphics.c TextTools.h Tapplication.h Rect.h
DialogBox.o: dialogbox.h /usr/local/topfield-gcc/topfield-sdk/include/tap.h
DialogBox.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
DialogBox.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
DialogBox.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
DialogBox.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
DialogBox.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h Dialog.h
DialogBox.o: ../libc++utils/tapstring.h Tapplication.h colors.h Decorator.h
Dialog.o: dialog.h /usr/local/topfield-gcc/topfield-sdk/include/tap.h
Dialog.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
Dialog.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
Dialog.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
Dialog.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
Dialog.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h Tapplication.h
Dialog.o: colors.h
FooterListItem.o: FooterListItem.h ListPage.h /usr/include/stdlib.h
FooterListItem.o: /usr/include/features.h /usr/include/sys/cdefs.h
FooterListItem.o: /usr/include/gnu/stubs.h /usr/include/sys/types.h
FooterListItem.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
FooterListItem.o: /usr/include/bits/typesizes.h /usr/include/time.h
FooterListItem.o: /usr/include/endian.h /usr/include/bits/endian.h
FooterListItem.o: /usr/include/sys/select.h /usr/include/bits/select.h
FooterListItem.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
FooterListItem.o: /usr/include/sys/sysmacros.h
FooterListItem.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
FooterListItem.o: /usr/include/alloca.h
FooterListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
FooterListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
FooterListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
FooterListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
FooterListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
FooterListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
FooterListItem.o: listitem.h Rect.h ../libc++utils/tapstring.h colors.h
FooterListItem.o: ../libc++utils/taparray.h /usr/include/memory.h
FooterListItem.o: /usr/include/string.h ../libc++utils/quicksort.h Page.h
FooterListItem.o: SimpleTextListItem.h ActionListItem.h
ListColumn.o: listcolumn.h /usr/include/stdlib.h /usr/include/features.h
ListColumn.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
ListColumn.o: /usr/include/sys/types.h /usr/include/bits/types.h
ListColumn.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
ListColumn.o: /usr/include/time.h /usr/include/endian.h
ListColumn.o: /usr/include/bits/endian.h /usr/include/sys/select.h
ListColumn.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
ListColumn.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
ListColumn.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
ListColumn.o: /usr/include/alloca.h
ListColumn.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
ListColumn.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
ListColumn.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
ListColumn.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
ListColumn.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
ListColumn.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h Rect.h
ListColumn.o: ../libc++utils/tapstring.h colors.h ListPage.h listitem.h
ListColumn.o: ../libc++utils/taparray.h /usr/include/memory.h
ListColumn.o: /usr/include/string.h ../libc++utils/quicksort.h Page.h
ListItem.o: ListItem.h /usr/include/stdlib.h /usr/include/features.h
ListItem.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
ListItem.o: /usr/include/sys/types.h /usr/include/bits/types.h
ListItem.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
ListItem.o: /usr/include/time.h /usr/include/endian.h
ListItem.o: /usr/include/bits/endian.h /usr/include/sys/select.h
ListItem.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
ListItem.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
ListItem.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
ListItem.o: /usr/include/alloca.h
ListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
ListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
ListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
ListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
ListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
ListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h Rect.h
ListItem.o: ../libc++utils/tapstring.h ListPage.h listitem.h colors.h
ListItem.o: ../libc++utils/taparray.h /usr/include/memory.h
ListItem.o: /usr/include/string.h ../libc++utils/quicksort.h Page.h
ListItem.o: ../libc++utils/Logger.h ListColumn.h
ListPage.o: listpage.h /usr/include/stdlib.h /usr/include/features.h
ListPage.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
ListPage.o: /usr/include/sys/types.h /usr/include/bits/types.h
ListPage.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
ListPage.o: /usr/include/time.h /usr/include/endian.h
ListPage.o: /usr/include/bits/endian.h /usr/include/sys/select.h
ListPage.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
ListPage.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
ListPage.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
ListPage.o: /usr/include/alloca.h
ListPage.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
ListPage.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
ListPage.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
ListPage.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
ListPage.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
ListPage.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h listitem.h
ListPage.o: Rect.h ../libc++utils/tapstring.h colors.h
ListPage.o: ../libc++utils/taparray.h /usr/include/memory.h
ListPage.o: /usr/include/string.h ../libc++utils/quicksort.h Page.h
ListPage.o: ListColumn.h Tapplication.h ../libutils/morekeys.h YesNoBox.h
ListPage.o: DialogBox.h Dialog.h ../libc++utils/Logger.h
Main.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
Main.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
Main.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
Main.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
Main.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
Main.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
Main.o: ../libc++utils/vtable.h ../libutils/dlmalloc.h Tapplication.h
Main.o: colors.h ../libc++utils/Logger.h
MultiLineListItem.o: multilinelistitem.h listitem.h /usr/include/stdlib.h
MultiLineListItem.o: /usr/include/features.h /usr/include/sys/cdefs.h
MultiLineListItem.o: /usr/include/gnu/stubs.h /usr/include/sys/types.h
MultiLineListItem.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
MultiLineListItem.o: /usr/include/bits/typesizes.h /usr/include/time.h
MultiLineListItem.o: /usr/include/endian.h /usr/include/bits/endian.h
MultiLineListItem.o: /usr/include/sys/select.h /usr/include/bits/select.h
MultiLineListItem.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
MultiLineListItem.o: /usr/include/sys/sysmacros.h
MultiLineListItem.o: /usr/include/bits/pthreadtypes.h
MultiLineListItem.o: /usr/include/bits/sched.h /usr/include/alloca.h
MultiLineListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
MultiLineListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
MultiLineListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
MultiLineListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
MultiLineListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
MultiLineListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
MultiLineListItem.o: Rect.h ../libc++utils/tapstring.h ListPage.h colors.h
MultiLineListItem.o: ../libc++utils/taparray.h /usr/include/memory.h
MultiLineListItem.o: /usr/include/string.h ../libc++utils/quicksort.h Page.h
OptionListItem.o: optionlistitem.h listitem.h /usr/include/stdlib.h
OptionListItem.o: /usr/include/features.h /usr/include/sys/cdefs.h
OptionListItem.o: /usr/include/gnu/stubs.h /usr/include/sys/types.h
OptionListItem.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
OptionListItem.o: /usr/include/bits/typesizes.h /usr/include/time.h
OptionListItem.o: /usr/include/endian.h /usr/include/bits/endian.h
OptionListItem.o: /usr/include/sys/select.h /usr/include/bits/select.h
OptionListItem.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
OptionListItem.o: /usr/include/sys/sysmacros.h
OptionListItem.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
OptionListItem.o: /usr/include/alloca.h
OptionListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
OptionListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
OptionListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
OptionListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
OptionListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
OptionListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h Rect.h
OptionListItem.o: ../libc++utils/tapstring.h ../libc++utils/option.h
OptionListItem.o: ../libc++utils/taparray.h /usr/include/memory.h
OptionListItem.o: /usr/include/string.h ../libc++utils/quicksort.h
OptionListItem.o: ../libutils/morekeys.h Page.h
OptionsPage.o: optionspage.h ListPage.h /usr/include/stdlib.h
OptionsPage.o: /usr/include/features.h /usr/include/sys/cdefs.h
OptionsPage.o: /usr/include/gnu/stubs.h /usr/include/sys/types.h
OptionsPage.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
OptionsPage.o: /usr/include/bits/typesizes.h /usr/include/time.h
OptionsPage.o: /usr/include/endian.h /usr/include/bits/endian.h
OptionsPage.o: /usr/include/sys/select.h /usr/include/bits/select.h
OptionsPage.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
OptionsPage.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
OptionsPage.o: /usr/include/bits/sched.h /usr/include/alloca.h
OptionsPage.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
OptionsPage.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
OptionsPage.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
OptionsPage.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
OptionsPage.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
OptionsPage.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h listitem.h
OptionsPage.o: Rect.h ../libc++utils/tapstring.h colors.h
OptionsPage.o: ../libc++utils/taparray.h /usr/include/memory.h
OptionsPage.o: /usr/include/string.h ../libc++utils/quicksort.h Page.h
OptionsPage.o: ListColumn.h OptionListItem.h ../libc++utils/Options.h
OptionsPage.o: ../libc++utils/OptionUpdateNotifier.h
Page.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
Page.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
Page.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
Page.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
Page.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
Page.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h Tapplication.h
Page.o: colors.h Page.h ../libc++utils/logger.h
ProgressBox.o: progressbox.h dialogbox.h
ProgressBox.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
ProgressBox.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
ProgressBox.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
ProgressBox.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
ProgressBox.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
ProgressBox.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h Dialog.h
ProgressBox.o: ../libc++utils/tapstring.h Tapplication.h colors.h Decorator.h
Rect.o: Rect.h /usr/local/topfield-gcc/topfield-sdk/include/tap.h
Rect.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
Rect.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
Rect.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
Rect.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
Rect.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
Rect.o: /usr/include/string.h /usr/include/features.h
Rect.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Rect.o: /usr/include/ctype.h /usr/include/bits/types.h
Rect.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
Rect.o: /usr/include/endian.h /usr/include/bits/endian.h texttools.h
Region.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
Region.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
Region.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
Region.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
Region.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
Region.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
Region.o: /usr/include/string.h /usr/include/features.h
Region.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h Region.h
SimpleTextListItem.o: simpletextlistitem.h listitem.h /usr/include/stdlib.h
SimpleTextListItem.o: /usr/include/features.h /usr/include/sys/cdefs.h
SimpleTextListItem.o: /usr/include/gnu/stubs.h /usr/include/sys/types.h
SimpleTextListItem.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
SimpleTextListItem.o: /usr/include/bits/typesizes.h /usr/include/time.h
SimpleTextListItem.o: /usr/include/endian.h /usr/include/bits/endian.h
SimpleTextListItem.o: /usr/include/sys/select.h /usr/include/bits/select.h
SimpleTextListItem.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
SimpleTextListItem.o: /usr/include/sys/sysmacros.h
SimpleTextListItem.o: /usr/include/bits/pthreadtypes.h
SimpleTextListItem.o: /usr/include/bits/sched.h /usr/include/alloca.h
SimpleTextListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
SimpleTextListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
SimpleTextListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
SimpleTextListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
SimpleTextListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
SimpleTextListItem.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
SimpleTextListItem.o: Rect.h ../libc++utils/tapstring.h
SkinDecorator.o: skindecorator.h decorator.h
SkinDecorator.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
SkinDecorator.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
SkinDecorator.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
SkinDecorator.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
SkinDecorator.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
SkinDecorator.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h colors.h
SkinDecorator.o: Tapplication.h Rect.h
SkinOption.o: skinoption.h ../libc++utils/option.h /usr/include/stdlib.h
SkinOption.o: /usr/include/features.h /usr/include/sys/cdefs.h
SkinOption.o: /usr/include/gnu/stubs.h /usr/include/sys/types.h
SkinOption.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
SkinOption.o: /usr/include/bits/typesizes.h /usr/include/time.h
SkinOption.o: /usr/include/endian.h /usr/include/bits/endian.h
SkinOption.o: /usr/include/sys/select.h /usr/include/bits/select.h
SkinOption.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
SkinOption.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
SkinOption.o: /usr/include/bits/sched.h /usr/include/alloca.h
SkinOption.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
SkinOption.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
SkinOption.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
SkinOption.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
SkinOption.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
SkinOption.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
SkinOption.o: ../libc++utils/tapstring.h ../libc++utils/taparray.h
SkinOption.o: /usr/include/memory.h /usr/include/string.h
SkinOption.o: ../libc++utils/quicksort.h Tapplication.h colors.h
SkinOption.o: ../libc++utils/Logger.h ../libc++utils/DirectoryUtils.h
SkinOption.o: ../libutils/file.h ../libutils/Utils.h
Tapplication.o: /usr/include/stdlib.h /usr/include/features.h
Tapplication.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Tapplication.o: /usr/include/sys/types.h /usr/include/bits/types.h
Tapplication.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
Tapplication.o: /usr/include/time.h /usr/include/endian.h
Tapplication.o: /usr/include/bits/endian.h /usr/include/sys/select.h
Tapplication.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
Tapplication.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
Tapplication.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
Tapplication.o: /usr/include/alloca.h
Tapplication.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
Tapplication.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
Tapplication.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
Tapplication.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
Tapplication.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
Tapplication.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
Tapplication.o: ../libc++utils/vtable.h ../libc++utils/inifile.h
Tapplication.o: ../libc++utils/taparray.h /usr/include/memory.h
Tapplication.o: /usr/include/string.h ../libc++utils/quicksort.h
Tapplication.o: ../libc++utils/tapstring.h Tapplication.h colors.h Page.h
Tapplication.o: Rect.h ../libc++utils/logger.h dialog.h Decorator.h
TextTools.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
TextTools.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
TextTools.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
TextTools.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
TextTools.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
TextTools.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
TextTools.o: /usr/include/string.h /usr/include/features.h
TextTools.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TextTools.o: /usr/include/ctype.h /usr/include/bits/types.h
TextTools.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
TextTools.o: /usr/include/endian.h /usr/include/bits/endian.h
YesNoBox.o: /usr/local/topfield-gcc/topfield-sdk/include/tap.h
YesNoBox.o: /usr/local/topfield-gcc/topfield-sdk/include/type.h
YesNoBox.o: /usr/local/topfield-gcc/topfield-sdk/include/key.h
YesNoBox.o: /usr/local/topfield-gcc/topfield-sdk/include/hdd.h
YesNoBox.o: /usr/local/topfield-gcc/topfield-sdk/include/font.h
YesNoBox.o: /usr/local/topfield-gcc/topfield-sdk/include/win.h
YesNoBox.o: /usr/include/string.h /usr/include/features.h
YesNoBox.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h YesNoBox.h
YesNoBox.o: DialogBox.h Dialog.h ../libc++utils/tapstring.h
YesNoBox.o: ../../../trunk/libs/framework/Tapplication.h colors.h TextTools.h
YesNoBox.o: Decorator.h
