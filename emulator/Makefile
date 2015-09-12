CFLAGS = -I../include -std=c99 -g 

all: emulator

emulator: main.o debug.o
	cc $(CFLAGS) -o jemulator main.o debug.o -ljemulator -L../common

main.o: main.c
	cc $(CFLAGS) -c main.c

debug.o: debug.c
	cc $(CFLAGS) -c debug.c

install:
	cp jemulator /usr/bin

deinstall:
	rm /usr/bin/jemulator

clean:
	rm *.o
	rm jemulator
