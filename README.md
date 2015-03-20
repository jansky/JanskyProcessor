#JanskyProcessor

This is an emulator, assembler, and memory dump analyzer, as well as a lot of example programs for a virtual computer (i.e. it does not exist). The goal of this project is to help me (and maybe others) understand how low-level programming and operations work without having to deal with the asininity of the x86 architecture and instruction set (please tell me why it's such a pain to go from real to protected mode again?). 

##How To Use It

First, build everything by running `make`. Ensure you have a *modern* C++ compiler that has C++11 / C++0x support. Mingw doesn't seem to work, as it's g++ does not have support for the `std::stoul` function, needed for the assembler. If someone can fix this, please let me know, but I'm not wasting my time dealing with stupid Windows problems. Your best bet is to use Linux or some other UNIX (or maybe Cygwin). 

To run a program for the emulator (which typically ends with a .bin extension), run `./emulator filename.bin'.

By default, the emulator will output the contents of emulated RAM to a file called `mem.dmp`. If you want to change this, run `./emulator filename.bin memorydump.dmp`.

To analyze a memory dump, run `./memanalyze dumpfile.dmp`. You will be greeted with a prompt (`%`). Type in `b` for byte, `w` for WORD, or `d` for DWORD and the address in hexadecimal to view the contents of memory at that address (for example, `d FF` will get the DWORD stored at memory location 0xFF). To quit, enter `q 0` and press enter.

To run the assembler on an assembly code file (typically ending with a .s extension), run `./assembler code.s output.bin`. The assembler can only assemble code from *one* file currently. This may (and will likely have to) change.

##Architecture Info & Assembly Guide

I am currently writing documentation for the processor architecture and an assembly guide using Latex.

##Changing Emulator RAM Size

Currently, the size of emulator RAM is hardcoded at 4 KB (4096 bytes). If you want to change this, edit the `main.c` file, changing the following lines.

```
uint32_t memsize = 4096;
```

to

```
uint32_t memsize = size in bytes;
```

```
cpu->sp = 4095;
cpu->sb = 3895;
```

to

```
cpu->sp = (size in bytes) - 1;
cpu->sb = (cpu->sp value) - (num of stack elements you wish to have * 4);
```

This just ensures that the proper amount of memory is allocated *and* that the stack works properly.