include ../../tools/build/tap.mk

BASELIB_A=/usr/local/topfield-gcc/topfield-sdk/lib
INC_DIR =  -I ./ -I /usr/local/topfield-gcc/mips/include/ -I /usr/local/topfield-gcc/topfield-sdk/include/
#CFLAGS += $(INC_DIR) -v -DFT2_BUILD_LIBRARY -DMINIMAL_OS_SUPPORT
CFLAGS += $(INC_DIR) 


all: fonts.tap

# Copy files to the Toppy using tfcopy
copy: all
	tfcopy *.tap '#:\ProgramFiles\'

# A target for removing generated files.
clean:
	-rm -f *.tap *.o *.elf *.map

fonts.elf::	tapapifix.o RasterFontBrowser.o fonts.o

fonts.o:		fonts.c 
RasterFontBrowser.o:	RasterFontBrowser.c
tapapifix.o:		tapapifix.c
