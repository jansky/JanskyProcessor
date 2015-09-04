all:
	$(MAKE) -C emulator all
	$(MAKE) -C assembler all
	$(MAKE) -C disassembler all
	$(MAKE) -C memanalyze all
	$(MAKE) -C jos all

clean:
	$(MAKE) -C emulator clean
	$(MAKE) -C assembler clean
	$(MAKE) -C disassembler clean
	$(MAKE) -C memanalyze clean
	$(MAKE) -C jos clean

install:
	$(MAKE) -C emulator install
	$(MAKE) -C assembler install
	$(MAKE) -C disassembler install
	$(MAKE) -C memanalyze install

