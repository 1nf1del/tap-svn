include taps.mak

CFLAGS += -DOS=\"$(OS)\" -Wall -I.
LDFLAGS += -Wl,-Map,$*.map

all: menulibc.tap libctest.tap tapcall.tap simple.tap testosd.tap testmenu.tap
#	stdlibtest.tap stringtest.tap mathtest.tap

simple.elf: simple.o
	$(LINK) $(LDFLAGS) -o $@ simple.o $(LDLIBS) 

libctest.elf: libctest.o
	$(LINK) $(LDFLAGS) -o $@ libctest.o $(LDLIBS) 

menulibc.elf: menulibc.o
	$(LINK) $(LDFLAGS) -o $@ menulibc.o $(LDLIBS) 

tapcall.elf: tapcall.o
	$(LINK) $(LDFLAGS) -o $@ tapcall.o $(LDLIBS) 

testosd.elf: testosd.o osdlib.o
	$(LINK) $(LDFLAGS) -o $@ testosd.o osdlib.o $(LDLIBS) 

testmenu.elf: testmenu.o
	$(LINK) $(LDFLAGS) -o $@ testmenu.o $(LDLIBS) 

stdlibtest.elf: stdlibtest.o
	$(LINK) $(LDFLAGS) -o $@ stdlibtest.o $(LDLIBS) 

stringtest.elf: stringtest.o
	$(LINK) $(LDFLAGS) -o $@ stringtest.o $(LDLIBS) 

mathtest.elf: mathtest.o
	$(LINK) $(LDFLAGS) -o $@ mathtest.o $(LDLIBS) 


