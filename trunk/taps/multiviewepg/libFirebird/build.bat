@echo off

set Project=libFireBird
set ZIEL=..
set COMPILE=for %%i in (*.c) do call ..\..\gcc %%i
rem set COMPILE=%BASH% -c "for f in *.c; do command.com /c tcc $f; done"

echo -Compiling...

echo --av
cd av
if exist *.o del *.o
%COMPILE%
cd ..

echo --compression
cd compression
if exist *.o del *.o
%COMPILE%
cd ..

echo --dialog
cd dialog
if exist *.o del *.o
%COMPILE%
cd ..

echo --flash
cd flash
if exist *.o del *.o
%COMPILE%
cd ..

echo --fs
cd fs
if exist *.o del *.o
%COMPILE%
cd ..

echo --fwpatches
cd fwpatches
if exist *.o del *.o
%COMPILE%
cd ..

echo --hdd
cd hdd
if exist *.o del *.o
%COMPILE%
cd ..

echo --hook
cd hook
if exist *.o del *.o
%COMPILE%
cd ..

echo --iic
cd iic
if exist *.o del *.o
%COMPILE%
cd ..

echo --imem
cd imem
if exist *.o del *.o
%COMPILE%
cd ..

echo --ini
cd ini
if exist *.o del *.o
%COMPILE%
cd ..

echo --instruction sequences
cd InstructionSequences
if exist *.o del *.o
%COMPILE%
cd ..

echo --main
cd main
if exist *.o del *.o
%COMPILE%
cd ..

echo --mpvfd
cd mpvfd
if exist *.o del *.o
%COMPILE%
cd ..

echo --rec
cd rec
if exist *.o del *.o
%COMPILE%
cd ..

echo --shutdown
cd shutdown
if exist *.o del *.o
%COMPILE%
cd ..

echo --string
cd string
if exist *.o del *.o
%COMPILE%
cd ..

echo --tap
cd tap
if exist *.o del *.o
%COMPILE%
cd ..

echo --tapcom
cd tapcom
if exist *.o del *.o
%COMPILE%
cd ..

echo --tapapifix
cd tapapifix
if exist *.o del *.o
%COMPILE%
cd ..

echo -Building library...
echo.

if exist %Project%.a del %Project%.a

mips-ar -r %Project%.a av/*.o compression/*.o dialog/*.o flash/*.o fs/*.o fwpatches/*.o hdd/*.o hook/*.o iic/*.o imem/*.o ini/*.o InstructionSequences/*.o main/*.o mpvfd/*.o rec/*.o shutdown/*.o string/*.o tap/*.o tapcom/*.o tapapifix/*.o
mips-ranlib %Project%.a

del av\*.o
del compression\*.o
del dialog\*.o
del flash\*.o
del fs\*.o
del fwpatches\*.o
del hdd\*.o
del hook\*.o
del iic\*.o
del imem\*.o
del ini\*.o
del InstructionSequences\*.o
del main\*.o
del mpvfd\*.o
del rec\*.o
del shutdown\*.o
del string\*.o
del tap\*.o
del tapcom\*.o
del tapapifix\*.o

copy %Project%.h %ZIEL%
copy %Project%.a %ZIEL%

set Project=
set ZIEL=
set COMPILE=

echo Done.
echo.
