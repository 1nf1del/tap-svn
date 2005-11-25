call ..\gcc Archive.c
mips-ld --cref -o Archive.elf -T ..\TAP.LD Archive.o -l tap -l c -Map Archive.map
mips-objcopy -O binary Archive.elf Archive.tap
pause
copy Archive.tap Y:\
