# $Id$
#
# Copyright (c) 2004-2005 Peter Urbanec
# 

ifndef TAP_HOME
TAP_HOME:=/home/tap
$(warning TAP_HOME is not set, using $(TAP_HOME))
endif

ARCH:=mips-
AR=$(ARCH)ar
AS=$(ARCH)as
LD=$(ARCH)ld
NM=$(ARCH)nm
OBJCOPY=$(ARCH)objcopy
OBJDUMP=$(ARCH)objdump
RANLIB=$(ARCH)ranlib
SIZE=$(ARCH)size
STRINGS=$(ARCH)strings
STRIP=$(ARCH)strip
CC=$(ARCH)gcc
CXX=$(ARCH)g++

CFLAGS=-O2 -mtap -mlong-calls -msoft-float -I $(TAP_HOME) -isystem /usr/local/include
CXXFLAGS=$(CFLAGS) -fno-rtti -fno-exceptions -Wno-pmf-conversions
LDFLAGS=

define link-elf
$(LD) $^ --script $(TAP_HOME)/tap.ld -L $(TAP_HOME) -L /usr/local/lib -l tap -l c $(LDFLAGS) -o $@
endef

%.elf: %.o
	$(link-elf)

%.tap: %.elf
	$(OBJCOPY) -O binary $< $@
