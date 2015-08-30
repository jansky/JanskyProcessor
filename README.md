#JanskyProcessor

This is an emulator, assembler, a disassembler and memory dump analyzer, as well as a lot of example programs for a virtual computer (i.e. it does not exist). The goal of this project is to help me (and maybe others) understand how low-level programming and operations work without having to deal with the asininity of the x86 architecture and instruction set (please tell me why it's such a pain to go from real to protected mode again?). 


##How To Use It

First, build everything by running `make`. Ensure you have a *modern* C++ compiler that has C++11 / C++0x support. Mingw doesn't seem to work, as its g++ does not have support for the `std::stoul` function, needed for the assembler. If someone can fix this, please let me know, but I'm not wasting my time dealing with stupid Windows problems. Your best bet is to use Linux or some other UNIX (or maybe Cygwin). Then run `make install`.

To run a program for the emulator (which typically ends with a .bin extension), run `jemulator -p filename.bin' or 'jemulator --program filename.bin'.

By default, the emulator will output the contents of emulated RAM to a file called `mem.dmp`. If you want to change this, run `jemulator -p filename.bin -d memorydump.dmp`.

To enable disk interop support (necessary for allowing emulator programs to access files on the actual hard disk), use the `--roodir` option:

`jemulator -p file.bin --rootdir "/home/you/JanskyProcessor/programs"

You may use any directory you want, but make sure not to include a trailing slash. The root directory is used when emulator programs access the disk. The emulator program must only specify a path relative to the root path. For example, if the root directory is '/home/you/JanskyProcessor/programs' and the emulator program asks for the file 'pow.bin', the emulator will access file '/home/you/JanskyProcessor/programs/pow.bin'.

To analyze a memory dump, run `jmemanalyze dumpfile.dmp`. You will be greeted with a prompt (`%`). Type in `b` for byte, `w` for WORD, or `d` for DWORD and the address in hexadecimal to view the contents of memory at that address (for example, `d FF` will get the DWORD stored at memory location 0xFF). To quit, enter `q 0` and press enter.

To run the assembler on an assembly code file (typically ending with a .s extension), run `jassembler code.s output.bin`. The assembler can only assemble code from *one* file currently. This may (and will likely have to) change.

To run the disassembler on a binary program file (typically ending with a .bin extension), run `jdisassembler program.bin output.s`. All numbers are  outputted as hexadecimal, and label macros are not reinserted. You may opt to display disassembler output onscreen by running `jdisassembler program.bin STDOUT`. Any disassembled program can be fed back into the assembler to produce the same exact program binary.

**NOTE: The disassembler doesn't work so well on newer programs that rely on offset locations (i.e. location type 'lo'). A fix is coming soon.**


##Changing Emulator RAM Size

The default RAM size is 10 MB. To change this, simply run the emulator with the `--memsize` or `-m` argument:

`jemulator -p file.bin --memsize 1000000`

The memory size must be in hexadecimal, and must be greater than 100000 (1MB).
