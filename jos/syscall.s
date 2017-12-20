#include "jos.h"

; systemcall - performs the system call requested
glbl systemcall
cmp r pr1 a 0x00
jmprc eq l systemcall_test l systemcallend
cmp r pr1 a 0x01
jmprc eq l systemcall_getosrev l systemcallend
cmp r pr1 a 0x02
jmprc eq l systemcall_execfile l systemcallend
cmp r pr1 a 0x03
jmprc eq l systemcall_strlen l systemcallend
lbl systemcallend
pop r ar1
pop r ar2
uip r ar1 r ar2

lbl systemcall_test
put r nr7 0x6a
ret

lbl systemcall_getosrev
cpy lo jos_version r ar1
cpy pr ar1 r ar1
ret

lbl systemcall_execfile
pop r ar1
pop r ar2
pop r ar3

pop r nr1
pop r nr2

push r ar3
push r ar2
push r ar1

put r pr1 0x03
put r pr2 0x02
cpy r nr1 r ar2
cpy r nr2 r ar1
int 0x10
cmp r ar5 a 0
jmpc ne l systemcall_execfile_return
; verify that program loaded is a valid jOS program
cpy lo jos_magicnum r nr1
cmp pr ar1 pr nr1
jmprc ne l systemcall_execfile_invaliderror l systemcall_execfile_return
; execute the program
;add r ar2 a 4
cpy l systemcall_execfile_return_from_remote r ar3
push r ar3
push r or1
uip r ar1 a 4

lbl systemcall_execfile_invaliderror
pop r nr7
put r ar5 0x100
ret


lbl systemcall_execfile_return
pop r nr7
ret

lbl systemcall_execfile_return_from_remote
ret

lbl systemcall_strlen
pop r ar1
pop r ar2
pop r ar3

pop r nr1

push r ar3
push r ar2
push r ar1

cpy r nr1 r ar1

lbl systemcall_strlen_loop
cmp bpr nr1 a 0
jmprc ne l systemcall_strlen_count l systemcall_strlen_loop
sub r nr1 r ar1
ret

lbl systemcall_strlen_count
add r nr1 a 1
ret

lbl systemcall_strcmp
pop r ar1
pop r ar2
pop r ar3

pop r nr1
pop r nr2

push r ar3
push r ar2
push r ar1

; ensure the lengths are the same
cpy r nr1 r nr3
cpy r nr2 r nr4

push r nr3
put r pr1 SYSCALL_STRLEN
int 0x6a

cpy r nr1 r nr2

push r nr4
put r pr1 SYSCALL_STRLEN
int 0x6a

cmp r nr1 r nr2

put r nr1 0x01

jmpc eq l systemcall_strcmp_cmp l systemcall_strcmp_end

; the strings are not of equal length, so they must not be of equal length

push a 0x00

lbl systemcall_strcmp_end
pop r nr7
ret


lbl systemcall_strcmp_cmp
jmp l systemcall_strcmp_loop


lbl systemcall_strcmp_loop
pop r nr7
cmp bpr nr3 bpr nr4
jmpc ne l systemcall_strcmp_end
; check for end of string
cmp bpr nr3 a 0

