call ..\gcc	timer_extend.c
mips-ld --cref -o timer_extend.elf -T ..\TAP.LD timer_extend.o -l tap -l c -l gcc -l m -Map timer_extend.map
mips-objcopy -O binary timer_extend.elf timer_extend.tap
mips-objdump -D timer_extend.elf > timer_extend.lst
pause