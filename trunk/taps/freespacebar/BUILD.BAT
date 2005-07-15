call ..\gcc FreeSpaceBar.c
mips-ld --cref -o FreeSpaceBar.elf -T ..\TAP.LD FreeSpaceBar.o -l tap -l c -Map FreeSpaceBar.map
mips-objcopy -O binary FreeSpaceBar.elf FreeSpaceBar.tap
pause