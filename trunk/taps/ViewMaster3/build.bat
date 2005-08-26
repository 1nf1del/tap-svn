call ..\gcc ViewMaster3.c -Wparentheses
mips-ld --cref -o ViewMaster3.elf -T ..\TAP.LD ViewMaster3.o -l tap -l c -Map ViewMaster3.map
mips-objcopy -O binary ViewMaster3.elf ViewMaster3.tap
rem pause
