cc = cc -std=c99
output = emulator

 
ifeq ($(OS),Windows_NT)
	output = emulator.exe
endif
 
 
all: emulator assembler memanalyze disassembler
 
emulator: main.o memory.o cpu.o interrupt.o
	$(cc) -o $(output) main.o memory.o cpu.o interrupt.o

main.o: main.c
	$(cc) -c main.c

memory.o: memory.c
	$(cc) -c memory.c

cpu.o: cpu.c
	$(cc) -c cpu.c

interrupt.o: interrupt.c
	$(cc) -c interrupt.c

assembler: assembler.cpp
	g++ -std=c++0x -o assembler assembler.cpp

memanalyze: memory.o memanalyze.c
	$(cc) -o memanalyze memanalyze.c memory.o

disassembler: disassembler.c
	$(cc) -o disassembler disassembler.c

clean:
	rm *.o
	rm $(output)
	rm assembler
	rm memanalyze
	rm disassembler
