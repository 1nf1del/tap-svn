include taps.mak

CFLAGS += -DOS=\"$(OS)\" -Wall -I.
LDFLAGS += -Wl,-Map,$*.map

all: multiviewepg.tap

multiviewepg.elf: multiviewepg.o
	$(LINK) $(LDFLAGS) -o $@ multiviewepg.o $(LDLIBS) 
