CFLAGS = -I../include -std=c99 -g

all: disassembler

disassembler: disassembler.o
	cc $(CFLAGS) -o jdisassembler disassembler.o

disassembler.o: disassembler.c
	cc $(CFLAGS) -c disassembler.c

install:
	cp jdisassembler /usr/bin

deinstall:
	rm /usr/bin/jdisassembler

clean:
	rm *.o
	rm jdisassembler
