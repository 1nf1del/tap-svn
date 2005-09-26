# Makefile for building taps with the linux gcc toolchain
#
# Mon Sep 12 21:49:51 EST 2005
# Steve Bennett <steveb@workware.net.au>
#
# GCCDIR is where the compiler dir
# SDKDIR is where libtap.a and the tap includes are stored

GCCDIR=/usr/local/topfield-gcc
SDKDIR=$(GCCDIR)/topfield-sdk

CROSS=$(GCCDIR)/bin/mips-
# All of the following flags are required when compiling taps.
# Note that we *must* use -fno-delayed-branch or else the topfield goes gaga
CPUFLAGS = -mlong-calls -msoft-float -mqnxpic -fno-delayed-branch

CFLAGS += -O2

TAPLD=$(GCCDIR)/mips/lib/tap.ld

LINK = $(CC) -nostartfiles -T $(TAPLD)
LIBC_LIBS = -lc -lm

CC=$(CROSS)gcc $(CPUFLAGS)
CXX=$(CROSS)g++ $(CPUFLAGS)
OBJCOPY = $(CROSS)objcopy
AR=$(CROSS)ar

INCLUDES += -I$(SDKDIR)/include $(EXTRA_INCLUDES)

CFLAGS += $(INCLUDES)
CXXFLAGS += -fno-rtti -fno-exceptions -Wno-pmf-conversions
LDFLAGS += #-Wl,--no-warn-mismatch 
LDLIBS = -L$(SDKDIR)/lib -ltap $(LIBC_LIBS)

.SUFFIXES: .elf .tap

.elf.tap:
	@$(OBJCOPY) -O binary $< $*.tap

ALL: all

clean::
	rm -f *.elf *.tap *.o *.map
