# $Id$
#
# Copyright (c) 2004-2005 Peter Urbanec
#
# Steve Bennett <steveb@workware.net.au>
#

# Allow the path and SDK to be overriden with command line arguments
# Example: make TAP_GCC_DIR=/opt/tap-gcc TAP_HOME=/tap/home 
#
TAP_GCC_DIR?=/usr/local/topfield-gcc
TAP_HOME?=$(TAP_GCC_DIR)/topfield-sdk

# All of the following flags are required when compiling taps.
# # Note that we *must* use -fno-delayed-branch or else the topfield goes gaga
#
MANDATORY_C_FLAGS= -mqnxpic -fno-delayed-branch -mlong-calls -msoft-float -isystem $(TAP_HOME)/include
MANDATORY_CXX_FLAGS=$(MANDATORY_C_FLAGS) -fno-rtti -fno-exceptions -Wno-pmf-conversions -fno-weak
MANDATORY_LD_FLAGS=--script $(TAP_GCC_DIR)/mips/lib/tap.ld

# Specify crosscompilation prefix
CROSS_PREFIX:=$(TAP_GCC_DIR)/bin/mips-
AR=$(CROSS_PREFIX)ar
AS=$(CROSS_PREFIX)as
LD=$(CROSS_PREFIX)ld $(MANDATORY_LD_FLAGS)
NM=$(CROSS_PREFIX)nm
OBJCOPY=$(CROSS_PREFIX)objcopy
OBJDUMP=$(CROSS_PREFIX)objdump
RANLIB=$(CROSS_PREFIX)ranlib
SIZE=$(CROSS_PREFIX)size
STRINGS=$(CROSS_PREFIX)strings
STRIP=$(CROSS_PREFIX)strip
CC=$(CROSS_PREFIX)gcc $(MANDATORY_C_FLAGS)
CXX=$(CROSS_PREFIX)g++ $(MANDATORY_CXX_FLAGS)

LDFLAGS= -L$(TAP_HOME)/lib -ltap -lc -lm 
CFLAGS= -O2

ifdef DEBUG
CFLAGS+=-DDEBUG
endif

%.tap: %.elf
	$(OBJCOPY) -O binary $< $@

define link-elf
$(LD) $^ $(LDFLAGS) -o $@
endef

%.elf: %.o
	$(link-elf)

%.a: %.o
	$(AR) -cr $@

