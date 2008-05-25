@echo off
rem cls

set gccPath=..\..\..\gcc
set TAPLDPath=..\..\..\TAP.LD


set Project=DemoClient1
echo ---%Project%
call %gccPath% %Project%.c

echo ---Linker
mips-ld --cref -o %Project%.elf -T %TAPLDPath% %Project%.o -l FireBird -l c -l tap -l gcc -l FireBird -Map %Project%.map
mips-objcopy -O binary %Project%.elf %Project%.tap
erase %Project%.o %Project%.elf %Project%.map


set Project=DemoClient2
echo ---%Project%
call %gccPath% %Project%.c

echo ---Linker
mips-ld --cref -o %Project%.elf -T %TAPLDPath% %Project%.o -l FireBird -l c -l tap -l gcc -l FireBird -Map %Project%.map
mips-objcopy -O binary %Project%.elf %Project%.tap
erase %Project%.o %Project%.elf %Project%.map


set Project=DemoServer
echo ---%Project%
call %gccPath% %Project%.c

echo ---Linker
mips-ld --cref -o %Project%.elf -T %TAPLDPath% %Project%.o -l FireBird -l c -l tap -l gcc -l FireBird -Map %Project%.map
mips-objcopy -O binary %Project%.elf %Project%.tap
erase %Project%.o %Project%.elf %Project%.map

..\..\..\tapcheck /f
echo ------
