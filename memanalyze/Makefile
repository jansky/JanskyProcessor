CFLAGS = -I../include -std=c99 -g

all: memanalyze

memanalyze: memanalyze.o ../emulator/memory.o
	cc $(CFLAGS) -o jmemanalyze memanalyze.o ../emulator/memory.o

memanalyze.o: memanalyze.c
	cc $(CFLAGS) -c memanalyze.c

install:
	cp jmemanalyze /usr/bin

deinstall:
	rm /usr/bin/jmemanalyze

clean:
	rm *.o
	rm jmemanalyze
