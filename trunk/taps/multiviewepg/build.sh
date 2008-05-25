#!/bin/sh
cd misc/gd
for i in *.GD; do cat $i >> graphics.h; done;
mv graphics.h ../../
cd ../..
make clean
make
rm *.o *.elf *.map
