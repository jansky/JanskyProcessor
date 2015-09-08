all: jos shell pow exit strlen hello

jos: jos.o
	mv jos.o jos.bin

jos.o: jos.s syscall.s dta.s
	./sasm.sh jos.s

shell: shell.s
	../assembler/jassembler shell.s shell.bin

pow: pow.s
	../assembler/jassembler pow.s pow.bin

exit: exit.s
	../assembler/jassembler exit.s exit.bin

strlen: strlen.s
	../assembler/jassembler  strlen.s strlen.bin

hello: hello.s
	../assembler/jassembler hello.s hello.bin
clean:
	rm *.bin
	rm *.o	
	rm *.sp
