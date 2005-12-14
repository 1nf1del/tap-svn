call ..\gcc Surfer.c
mips-ld --cref -o Surfer.elf -T ..\TAP.LD Surfer.o -l tap -l c -Map Surfer.map
mips-objcopy -O binary Surfer.elf Surfer.tap
pause
copy Surfer.tap y:\
