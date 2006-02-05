call ..\gcc.bat UkAuto.c
mips-ld -o UkAuto.elf -T ..\TAP.ld UkAuto.o -l tap -l c -Map UkAuto.map
mips-objcopy -O binary UkAuto.elf UkAuto.tap
mips-objdump -D UkAuto.elf > UkAuto.lst
pause
