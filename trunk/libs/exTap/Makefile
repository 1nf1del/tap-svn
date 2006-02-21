
FILENAME=exTap
INCS=-I ../exTap

TAP_HOME=/usr/local/tap

CC=mips-gcc
CFLAGS=-O2 -mtap -mlong-calls -msoft-float -I $(TAP_HOME)/include -I /usr/local/include $(INCS) -save-temps  -v

AR=mips-ar
ARFLAGS=


all: $(FILENAME).a

$(FILENAME).a: $(FILENAME).o
	$(AR) cr $@ $^
