CFLAGS = -I../include -std=c99 -g

all: common

common: memory.o cpu.o bios.o interrupt.o screen.o input.o interop.o
	ar rcs libjemulator.a memory.o cpu.o bios.o interrupt.o screen.o input.o interop.o

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

input.o: input.c
	cc $(CFLAGS) -c input.c

interop.o: interop.c
	cc $(CFLAGS) -c interop.c

clean:
	rm *.o
	rm *.a
