call ..\gcc.bat EPG_upload.c
mips-ld --cref -o EPG_upload.elf -T ..\TAP.ld ..\Flash.o ..\Firmware.o ..\exTAP.o ..\patches.o EPG_upload.o  -l c -l tap -l gcc -Map EPG_upload.map
mips-objcopy -O binary EPG_upload.elf EPG_upload.tap
mips-objdump -D EPG_upload.elf > EPG_upload.lst
pause 

