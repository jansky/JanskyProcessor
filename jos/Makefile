all: jos shell pow exit strlen hello strlen-new

jos: jos.o
	mv jos.o jos.bin

jos.o: jos.s syscall.s dta.s
	./sasm.sh jos.s

shell: shell.s
	../sasm/jsasm shell.s shell.bin

pow: pow.s
	../sasm/jsasm pow.s pow.bin

exit: exit.s
	../sasm/jsasm exit.s exit.bin

strlen: strlen.s
	../sasm/jsasm  strlen.s strlen.bin

hello: hello.s
	../sasm/jsasm hello.s hello.bin

strlen-new: strlen-new.s
	./sasm.sh strlen-new.s
	mv strlen-new.o strlen-new.bin
clean:
	rm *.bin	
	rm *.sp
	rm *.sp.lbls
