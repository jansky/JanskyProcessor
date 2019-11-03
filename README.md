# JanskyProcessor

JanskyProcessor is a byte code interpreter for a simple register machine and associated software.
Byte code programs can interact with the terminal and perform simple disk operations. I have been
working on this project on and off for several years to deepen my understanding of byte code, 
executable file formats, and general programming in C. Documentation for this software is still a
work in progress. If you have any questions, please contact me at <ian.duncan@u.yale-nus.edu.sg>.

This project contains:

* A byte code interpreter with a debugger
* A two-pass assembler generating relocatable object code
* A linker
* A disassembler
* A small "operating system" called jOS, that supports programs with a small set of system
calls

## Building

JanskyProcessor uses CMake. To build all of the programs, create a new build directory and run
CMake, and then make:

```
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
```

## Writing your First Program

Byte code programs are written in assembly language. Let's write a simple program that will add
two numbers. Open a text editor, and start entering the following text:

```
put r ar1 10
put r ar2 15
```
These two commands set the values of arithmetic registers `ar1` and `ar2` (there are five
such registers, `ar1` to `ar5`).
```
add r ar1 ar2
hlt
```
Now we add the two numbers (the result will be stored in the first register, `ar1`). Finally,
tell the byte code interpreter to termnate with the `hlt` command.

Save this file as `add.s`, and compile it with the assembler:
```
$ jsasm add.s add.o
```
There should now be a file called `add.o` in the current directory. This file cannot be run
by the byte code interpreter just yet - it is relocatable object code that must be linked
by the linker. You can run the linker to produce the final executable output with the following
command:
```
$ jlink .text add.o add.bin
```
Relocatable object code contains named sections that are organized by linker according
to the first argument. By default, the assembler will generate an object code file with all
code in the `.text` section.

Now, we can run our byte code program:
```
$ jemulator -p add.bin
Allocated 10485760 bytes of RAM.
CPU Halted

-----
Register Dump

ar1 - 19
ar2 - f
ar3 - 0
ar4 - 0
ar5 - 0
ip  - 1f
bp  - 0
sp  - 9fffff
sb  - 9ffbff
nr1 - 0
nr2 - 0
nr3 - 0
nr4 - 0
nr5 - 0
nr6 - 0
nr7 - 0
or1 - 400
cr1 - 0
pr1 - 0
pr2 - 0
flr1 - 0
CPU last instruction: 0
CPU last instruction result: 0
Instructions executed: 4
Wrote RAM dump to 'mem.dmp'.
```
You can see the results of the addition in the register dump. Register `ar1` holds the
hexadecimal value `19`, which is `25` in decimal (i.e., 10+15).

## Hello World
We can do something slightly more impressive, and print `Hello, World!` to the terminal
window. Create a new file, and enter the following text:
```
sec .dta
flbl res1
raws Hello, world!
rawb 0x0a
rawb 0x00
```
We start by defining our `Hello, World!` string. This will end up going in the `.dta` section,
which we will tell the linker to put last. We then create a label accessible in the entire 
file (i.e., `flbl`) that will point to the first character of the string. The macro `raws`
will insert the entire contents of the line into the object code file as an ASCII-encoded
string. Finally, we insert a newline (`0x10` in ASCII), and a null-terminator (`0x00`), which
is needed for the byte-code interpreter to correctly interpret the string.

Now we can define the code that will print the string. We first enter the `.text` section: 
```
sec .text
```
We then tell the byte code interpreter we want to print a string by putting the arguments
`0x01` in registers `pr1` and `pr2`. We then put the location of the first character of the
string into `ar1`. We use our label to help with this.
```
put r pr1 0x01
put r pr2 0x01
cpy lo res1 r ar1
```
Finally, we perform the printing by invoking interrupt `0x10`:
```
int 0x10
hlt
```
Save the file as `hello.s`. We can now compile our file:
```
$ jsasm hello.s hello.o
(file global) .dta: res1 0
```
As the assembler compiles the file, it outputs the addresses of all labels within the file,
which can be helpful for debugging purposes.

Now we link our file. We must specify that the `.text` section comes first, followed by
the `.dta` section:
```
$ jlink ".text;.dta" hello.o hello.bin
```
Now we can run our program:
```
Allocated 10485760 bytes of RAM.
Hello, world!
CPU Halted

-----
Register Dump

ar1 - 422
ar2 - 0
ar3 - 0
ar4 - 0
ar5 - 0
ip  - 21
bp  - 0
sp  - 9fffff
sb  - 9ffbff
nr1 - 0
nr2 - 0
nr3 - 0
nr4 - 0
nr5 - 0
nr6 - 0
nr7 - 0
or1 - 400
cr1 - 0
pr1 - 1
pr2 - 1
flr1 - 0
CPU last instruction: 0
CPU last instruction result: 0
Instructions executed: 5
Wrote RAM dump to 'mem.dmp'.
```
## Linking Multiple Files and Debugging
Let's implement our hello world program in a more interesting way. Create a file called `print.s`
containing the text:
```
; A global label can be accessed in any file
glbl print
put r pr1 0x01
put r pr2 0x01
int 0x10
ret
```
This file contains a print function of sorts. Calling code can set `ar1` to the memory location
containing the first character of the string, and jump to `print` to print it. 

Now we need to define our entry point, which will call the print function:
```
cpy lo res1 r ar1
jmpr la print la done
lbl done
hlt
lbl end
```
Here, we set the location of the string to be printed, and jump to the location of the print code. 
When the print code is done, the `ret` instruction will return us to the label `done`, which will
just halt execution. We include a label `end` so we can compute the location of the `print`
subroutine, which we can use to set a debugging breakpoint.

Finally, we create a file `res.s` to store our string:
```
sec .dta
glbl res1
raws Hello, world!
rawb 0x0a
rawb 0x00
```
Now we can compile and link our three source files:
```
$ jsasm print.s print.o
(global) .text: print 0
$ jsasm entry.s entry.o
.text: done 16
.text: end 17
$ jsasm res.s res.o
(global) .dta: res1 0
$ jlink ".text;.dta" entry.o print.o res.o print.bin
```
Running this program produces the same output:
```
$ jemulator -p print.bin
Allocated 10485760 bytes of RAM.
Hello, world!
CPU Halted

-----
Register Dump

ar1 - 42e
ar2 - 0
ar3 - 0
ar4 - 0
ar5 - 0
ip  - 16
bp  - 0
sp  - 9fffff
sb  - 9ffbff
nr1 - 0
nr2 - 0
nr3 - 0
nr4 - 0
nr5 - 0
nr6 - 0
nr7 - 0
or1 - 400
cr1 - 0
pr1 - 1
pr2 - 1
flr1 - 0
CPU last instruction: 0
CPU last instruction result: 0
Instructions executed: 7
Wrote RAM dump to 'mem.dmp'.
```
Notice how the instruction pointer (`ip`) contains `16`, which is the location of the `done` label
the print function was supposed to return to with the `ret` command. Support for jumping and returning
is implemented with a stack that grows from the upper boundary of allocated RAM. 

We can also play around with the debugger:
```
$ jemulator -p print.bin --debug
Allocated 10485760 bytes of RAM.

Welcome to the debugger.

or1 - 0x400	ip - 0x0

(dbg) 
```
When the byte code interpreter starts, the debugger prompt automatically opens so we can set
breakpoints. Let's set a breakpoint at the beginning of the `print` subroutine, which is at
location `0x17` (the size of the entry code as determined by the `end` label, plus the offset
of the print subroutine in the print code, which is zero):
```
(dbg) break 0x400 0x17
Added breakpoint at 0x400+0x17
(dbg) 
```
The first argument is for the offset of where the program is loaded (the byte code interpreter
supports relocatable program code). In this case, we specify `0x400`, since this is where the program
has been loaded (indicated by the contents of the `or1` register). Now, we start execution with the
`continue` command:
```
(dbg) continue
Debugger relinquishing control.


Welcome to the debugger.

or1 - 0x400	ip - 0x17

(dbg) 
```
Immediately, we return back to the debugger (but at the correct position, `0x400+0x17`). Let's see
the next command that will be run using the `dsmbl` command:
```
(dbg) dsmbl
0x400+0x17: put r pr1 0x1
(dbg)
```
We can also look at the contents of registers, like `ar1`:
```
(dbg) printr ar1
ar1 - 0x42e (1070)
(dbg)
```
We know that `ar1` points to the location of a string, so let's print it:
```
(dbg) printm s 0x400 0x2e
string at 0x42e - Hello, world!

(dbg) 
```
We can even modify the contents of registers. Let's modify `ar1` so only a slice of the string is
printed:
```
(dbg) setr ar1 0x42f
ar1 set to  0x42f (1071)
(dbg)
```
If we continue execution, we find that the initial `H` is not printed:
```
(dbg) continue       
Debugger relinquishing control.

ello, world!
CPU Halted

-----
Register Dump

ar1 - 42f
ar2 - 0
ar3 - 0
ar4 - 0
ar5 - 0
ip  - 16
bp  - 0
sp  - 9fffff
sb  - 9ffbff
nr1 - 0
nr2 - 0
nr3 - 0
nr4 - 0
nr5 - 0
nr6 - 0
nr7 - 0
or1 - 400
cr1 - 0
pr1 - 1
pr2 - 1
flr1 - 0
CPU last instruction: 0
CPU last instruction result: 0
Instructions executed: 7
Wrote RAM dump to 'mem.dmp'.
```