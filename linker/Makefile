CFLAGS = -I../include -std=c11 -g

all: linker

linker: io.o labels.o main.o section.o
	cc $(CFLAGS) -o jlink io.o labels.o main.o section.o

section.o: section.c
	cc $(CFLAGS) -c section.c

io.o: io.c
	cc $(CFLAGS) -c io.c

labels.o: labels.c
	cc $(CFLAGS) -c labels.c

main.o: main.c
	cc $(CFLAGS) -c main.c

clean:
	rm *.o
	rm jlink

install:
	cp jlink /usr/bin/jlink