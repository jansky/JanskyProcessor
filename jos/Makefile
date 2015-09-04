all: jos shell

jos: jos.s
	jassembler jos.s jos.bin

shell: shell.s
	jassembler shell.s shell.bin
clean:
	rm *.bin
