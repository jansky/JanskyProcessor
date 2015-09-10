#include "jos.h"

; implementation of strlen function
; pr1 - memory address of string
; ar1 - output - length of string (result is copied to ar2)
raws .jos

put r pr1 0x01
put r pr2 0x01
cpy lo res4 r ar1
int 0x10
put r pr1 0x02
put r pr2 0x01
put r ar2 200
cpy lo inputstr r ar1
int 0x10
cmp r ar5 a 0
jmpc ne l error
cpy lo inputstr r pr1
jmp l strlen
cpy r ar1 r ar2
; print the counted string
put r pr1 0x01
put r pr2 0x01
cpy lo res3 r ar1
int 0x10
put r ar5 0xFF
cpy lo inputstr r ar1
int 0x10
put r pr2 0x05
put r ar1 0x0a
int 0x10
int 0x10
; print the string count
put r pr2 0x01
cpy lo res1 r ar1
int 0x10
put r pr2 0x04
cpy r ar2 r ar1
int 0x10
put r pr2 0x01
cpy lo res2 r ar1
int 0x10
jmp l end
hlt
lbl strlen
cpy r pr1 r ar1
push r ar1
put r pr1 SYSCALL_STRLEN
int 0x6a
cpy r nr1 r ar1
ret
hlt
lbl error
put r pr1 0x01
put r pr2 0x02
cpy lo res5 r ar1
int 0x10
jmp l end

lbl end
pop r nr7
put r ar5 0x00
pop r ar1
pop r ar2
uip r ar1 r ar2
hlt
raws .dta
lbl res1
raws String length:
rawb 0x20
rawb 0x00
lbl res2
rawb 0x20
raws character(s)
rawb 0x0a
rawb 0x0a
rawb 0x00
lbl res3
raws String counted:
rawb 0x20
rawb 0x00
lbl res4
raws Enter a string (less than 200 characters):
rawb 0x20
rawb 0x00
lbl res5
raws There was an error getting the string.
rawb 0x0a
rawb 0x0a
rawb 0x00
lbl inputstr
rawb 0x00
