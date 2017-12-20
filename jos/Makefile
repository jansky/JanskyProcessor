JLINK = ../linker/jlink

all: jos shell pow exit strlen hello strlen-new

jos: jos.o syscall.o dta.o
	$(JLINK) jos.o syscall.o dta.o jos.bin

jos.o: jos.s syscall.s dta.s
	./sasm.sh jos.s

syscall.o: syscall.s
	./sasm.sh syscall.s

dta.o: dta.s
	./sasm.sh dta.s

shell: shell.o
	$(JLINK) shell.o shell.bin

shell.o: shell.s
	./sasm.sh shell.s

pow: pow.o
	$(JLINK) pow.o pow.bin

pow.o: pow.s
	./sasm.sh pow.s

exit: exit.o
	$(JLINK) exit.o exit.bin

exit.o: exit.s
	./sasm.sh exit.s

strlen: strlen.o
	$(JLINK) strlen.o strlen.bin

strlen.o: strlen.s
	./sasm.sh  strlen.s

hello: hello.o
	$(JLINK) hello.o hello.bin

hello.o: hello.s
	./sasm.sh hello.s

strlen-new: strlen-new.o
	$(JLINK) strlen-new.o strlen-new.bin

strlen-new.o: strlen-new.s
	./sasm.sh strlen-new.s

clean:
	rm *.o
	rm *.bin	
	rm *.sp
	rm *.sp.lbls
