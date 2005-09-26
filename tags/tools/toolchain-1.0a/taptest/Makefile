include taps.mak

CFLAGS += -DOS=\"$(OS)\" -Wall -I.
LDFLAGS += -Wl,-Map,$*.map

all: menulibc.tap libctest.tap tapcall.tap simple.tap testosd.tap testmenu.tap

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
