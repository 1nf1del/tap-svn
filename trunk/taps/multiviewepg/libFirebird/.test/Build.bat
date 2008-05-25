@echo off
rem cls
set Project=Test

echo ---%Project%
call ..\..\gcc %Project%.c

echo ---Linker
mips-ld --cref -o %Project%.elf -T ..\..\TAP.LD %Project%.o -l FireBird -l c -l tap -l gcc -l FireBird -Map %Project%.map
mips-objcopy -O binary %Project%.elf %Project%.tap
erase %Project%.o %Project%.elf %Project%.map

echo ------
