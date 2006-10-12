call ..\gcc Archive.c -I .\FIRMWARE -I .\LIBUTILS

mips-ld --cref -o Archive.elf -T ..\TAP.LD Archive.o -l tap -l c -Map Archive.map
mips-objcopy -O binary Archive.elf Archive.tap
pause
REM copy Archive.tap Y:\
