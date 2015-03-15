cc = cc -std=c99
output = emulator

 
ifeq ($(OS),Windows_NT)
	output = emulator.exe
endif
 
 
all: emulator assembler memanalyze
 
emulator: main.o memory.o cpu.o
	$(cc) -o $(output) main.o memory.o cpu.o

main.o: main.c
	$(cc) -c main.c

memory.o: memory.c
	$(cc) -c memory.c

cpu.o: cpu.c
	$(cc) -c cpu.c

assembler: assembler.cpp
	g++ -std=c++0x -o assembler assembler.cpp

memanalyze: memory.o memanalyze.c
	$(cc) -o memanalyze memanalyze.c memory.o

clean:
	rm *.o
	rm $(output)
	rm assembler
	rm memanalyze
