#  WIN32.MAK should be included at the front of every Win32 makefile.
#
!include <tap.mak>

UTILDIR = ..
RELEASEDIR = C:\user\jda\TV\Topfield\TAPfiles

TARGETS = fileTest.tap  fileTestSimple.tap   testINI.tap   list.tap    $(RELEASEDIR)\list.ini

all:	$(TARGETS)

fileTest.tap:  fileTest.o tools.o $(UTILDIR)\libUtils.a
	mips-ld --cref -o $*.elf -T $(TAPDIR)\TAP.LD fileTest.o tools.o -l Utils -l tap -l c -l gcc -l m -Map $*.map
	mips-objcopy -O binary $*.elf $*.tap
	copy $*.tap $(RELEASEDIR)

fileTestSimple.tap:  fileTestSimple.o $(UTILDIR)\libUtils.a
	mips-ld --cref -o $*.elf -T $(TAPDIR)\TAP.LD fileTestSimple.o -l Utils -l tap -l c -l gcc -l m -Map $*.map
	mips-objcopy -O binary $*.elf $*.tap
	copy $*.tap $(RELEASEDIR)

testINI.tap:  testINI.o $(UTILDIR)\libUtils.a
	mips-ld --cref -o $*.elf -T $(TAPDIR)\TAP.LD testINI.o -l Utils -l tap -l c -l gcc -l m -Map $*.map
	mips-objcopy -O binary $*.elf $*.tap
	copy $*.tap $(RELEASEDIR)

list.tap:  list.o $(UTILDIR)\libUtils.a
	mips-ld --cref -o $*.elf -T $(TAPDIR)\TAP.LD list.o -l Utils -l tap -l c -l gcc -l m -Map $*.map
	mips-objcopy -O binary $*.elf $*.tap
	copy $*.tap $(RELEASEDIR)

$(RELEASEDIR)\list.ini:	list.ini
	copy list.ini $(RELEASEDIR)

