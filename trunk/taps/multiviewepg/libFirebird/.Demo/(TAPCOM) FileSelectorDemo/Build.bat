@echo off
rem cls
set Project=FileSelectorDemoClient

echo ---%Project%
call ..\..\..\gcc %Project%.c

echo ---Linker
mips-ld --cref -o %Project%.elf -T ..\..\..\TAP.LD %Project%.o -l FireBird -l c -l tap -l gcc -l FireBird -Map %Project%.map
mips-objcopy -O binary %Project%.elf %Project%.tap

set Project=FileSelectorDemoServer

echo ---%Project%
call ..\..\..\gcc %Project%.c

echo ---Linker
mips-ld --cref -o %Project%.elf -T ..\..\..\TAP.LD %Project%.o -l FireBird -l c -l tap -l gcc -l FireBird -Map %Project%.map
mips-objcopy -O binary %Project%.elf %Project%.tap


erase *.o *.elf *.map
..\..\..\tapcheck /f
echo ------
