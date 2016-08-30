all:
	$(MAKE) -C common all
	$(MAKE) -C emulator all
	$(MAKE) -C sasm all
	$(MAKE) -C disassembler all
	$(MAKE) -C memanalyze all
	$(MAKE) -C jos all

clean:
	$(MAKE) -C common clean
	$(MAKE) -C emulator clean
	$(MAKE) -C sasm clean
	$(MAKE) -C disassembler clean
	$(MAKE) -C memanalyze clean
	$(MAKE) -C jos clean

install:
	$(MAKE) -C emulator install
	$(MAKE) -C sasm install
	$(MAKE) -C disassembler install
	$(MAKE) -C memanalyze install

