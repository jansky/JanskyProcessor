CFLAGS = -I../include -std=c99 -g

all: sasm

sasm: io.o location.o labels.o asm.o main.o
	cc $(CFLAGS) -o jsasm io.o location.o labels.o asm.o main.o

io.o: io.c
	cc $(CFLAGS) -c io.c

location.o: location.c
	cc $(CFLAGS) -c location.c
    
labels.o: labels.c
	cc $(CFLAGS) -c labels.c

main.o: main.c
	cc $(CFLAGS) -c main.c

install:
	cp jsasm /usr/bin
	
deinstall:
	rm /usr/bin/jsasm
	
clean:
	rm *.o
	rm jsasm
	


    
