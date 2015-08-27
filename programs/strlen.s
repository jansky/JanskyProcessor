; implementation of strlen function
; pr1 - memory address of string
; ar1 - output - length of string (result is copied to ar2)
cpy l teststring r pr1
jmp l strlen
cpy r ar1 r ar2
; print the counted string
put r pr1 0x01
put r pr2 0x01
cpy l res3 r ar1
int 0x10
put r ar5 0xFF
cpy l teststring r ar1
int 0x10
put r pr2 0x05
put r ar1 0x0a
int 0x10
int 0x10
; print the string count
put r pr2 0x01
cpy l res1 r ar1
int 0x10
put r pr2 0x04
cpy r ar2 r ar1
int 0x10
put r pr2 0x01
cpy l res2 r ar1
int 0x10
hlt
lbl strlen
cpy r pr1 r ar1
lbl strlenloop
cmp bpr pr1 a 0
jmprc ne l count l strlenloop
sub r pr1 r ar1
cpy r pr1 r ar1
ret
lbl count
add r pr1 a 1
ret
hlt
raws .dta
lbl teststring
raws This is a test.
rawb 0x00
lbl res1
raws String length:
rawb 0x20
rawb 0x00
lbl res2
rawb 0x20
raws character(s)
rawb 0x0a
rawb 0x00
lbl res3
raws String counted:
rawb 0x20
rawb 0x00
