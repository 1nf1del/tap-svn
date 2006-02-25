
makegraphics:
	g++ -IFreeImage/Dist/ *.c FreeImage/Dist/libfreeimage.a -O3 -Wall -o $@
	strip $@.exe

clean:
	-rm *.o makegraphics
