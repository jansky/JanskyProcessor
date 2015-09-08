all: jos shell pow exit strlen

jos: jos.o
	mv jos.o jos.bin

jos.o: jos.s syscall.s dta.s
	./sasm.sh jos.s

shell: shell.s
	jassembler shell.s shell.bin

pow: pow.s
	jassembler pow.s pow.bin

exit: exit.s
	jassembler exit.s exit.bin

strlen: strlen.s
	jassembler strlen.s strlen.bin
clean:
	rm *.bin
	rm *.o	
	rm *.sp
