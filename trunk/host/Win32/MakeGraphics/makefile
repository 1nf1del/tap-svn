OBJECTS = decode.o io.o huf.o makegraphics.o maketbl.o encode.o maketree.o tfdcompress.o

makegraphics.exe : $(OBJECTS) FreeImage/libfreeimage.a
	g++ -o makegraphics.exe $(OBJECTS) FreeImage/libfreeimage.a -lgcc -lc -lm 
	
clean:
	rm -f *.o
	
install: makegraphics.exe
	cp makegraphics.exe /usr/local/bin
	
FreeImage/libfreeimage.a:
	$(MAKE) -C FreeImage