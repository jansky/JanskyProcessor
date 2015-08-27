CFLAGS = -I../include -std=c99 -g

all: emulator

emulator: main.o memory.o cpu.o bios.o interrupt.o screen.o
	cc $(CFLAGS) -o jemulator main.o memory.o cpu.o bios.o interrupt.o screen.o

main.o: main.c
	cc $(CFLAGS) -c main.c

memory.o: memory.c
	cc $(CFLAGS) -c memory.c

cpu.o: cpu.c
	cc $(CFLAGS) -c cpu.c

interrupt.o: interrupt.c
	cc $(CFLAGS) -c interrupt.c

bios.o: bios.c
	cc $(CFLAGS) -c bios.c

screen.o: screen.c
	cc $(CFLAGS) -c screen.c

install:
	cp jemulator /usr/bin

deinstall:
	rm /usr/bin/jemulator

clean:
	rm *.o
	rm jemulator
