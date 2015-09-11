CFLAGS = -I../include -std=c99 -g 

all: emulator

emulator: main.o 
	cc $(CFLAGS) -o jemulator main.o -ljemulator -L../common

main.o: main.c
	cc $(CFLAGS) -c main.c



install:
	cp jemulator /usr/bin

deinstall:
	rm /usr/bin/jemulator

clean:
	rm *.o
	rm jemulator
