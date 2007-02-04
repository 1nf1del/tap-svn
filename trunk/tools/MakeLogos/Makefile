makelogos: makelogos.c
	g++ -IFreeImage/Dist/ makelogos.c FreeImage/Dist/libfreeimage.a -O3 -Wall -o $@
	strip $@.exe

clean:
	-rm *.o makelogos
