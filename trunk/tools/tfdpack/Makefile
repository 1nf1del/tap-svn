CFLAGS+=-Wall

tfdpack: tfdpack.o encode.o decode.o huf.o io.o maketbl.o maketree.o

clean:
	-rm *.o tfdpack

decode.o: decode.c ar.h
encode.o: encode.c ar.h
huf.o: huf.c ar.h
io.o: io.c ar.h
maketbl.o: maketbl.c ar.h
maketree.o: maketree.c ar.h
tfdpack.o: tfdpack.c ar.h

