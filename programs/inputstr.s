put r pr1 0x01
put r pr2 0x01
cpy lo prompt r ar1
int 0x10
put r pr1 0x02
put r pr2 0x01
put r ar2 100
cpy lo inputtest r ar1
int 0x10
cmp r ar5 a 0
jmpc ne l error
; write string to screen
put r pr1 0x01
cpy lo result r ar1
int 0x10
cpy lo inputtest r ar1
int 0x10
; write newline
put r ar1 0xa
put r pr2 0x05
int 0x10
int 0x10
hlt
lbl error
hlt
raws .dta
lbl prompt
raws Enter a string (less than 100 characters):
rawb 0x20
rawb 0x00
lbl result
raws You entered: 
rawb 0x20
rawb 0x00
lbl inputtest
