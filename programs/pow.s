; Basic implementation of a pow() function
; ar1 - number to raise to power
; ar2 - power to raise to
; Outputs
; ar1 - ar1^ar2
; Uses
; ar1, ar2, ar3, ar4
; get the input values
; get the base
put r pr1 0x01
put r pr2 0x01
cpy lo baseprompt r ar1
int 0x10
put r pr1 0x02
put r pr2 0x02
cpy lo numbuffer r ar1
int 0x10
; check for errors
cmp r ar5 a 0
jmpc ne l error
; save the data and get the exponent
cpy pr ar1 r nr1
put r pr1 0x01
put r pr2 0x01
cpy lo expprompt r ar1
int 0x10
put r pr1 0x02
put r pr2 0x02
cpy lo numbuffer r ar1
int 0x10
; check for errors
cmp r ar5 a 0
jmpc ne l error
; save the data
cpy pr ar1 r nr2
; copy the data, proceed to old code
cpy r nr1 r ar1
cpy r nr2 r ar2
put r ar3 1
cpy r ar1 r ar4
; Are we raising to the zero power?
cmp r ar2 a 0
; Yes
jmpc eq l zero
; No
lbl loop
cmp r ar3 r ar2
jmprc ne l pow l loop
jmp l result
lbl pow
mul r ar1 r ar4
add r ar3 a 1
ret
lbl zero
; Any number to the zero power is 1
put r ar1 1
jmp l result
hlt
lbl error
put r pr1 0x01
put r pr2 0x01
cpy lo errormsg r ar1
int 0x10
hlt
lbl result
cpy r ar1 r ar5
; output first number
cpy r ar4 r ar1
put r pr1 0x01
put r pr2 0x04
int 0x10
; output " ^ "
put r ar1 0x20
put r pr2 0x05
int 0x10
put r ar1 0x5E
int 0x10
put r ar1 0x20
int 0x10
; output second number
cpy r ar2 r ar1
put r pr2 0x04
int 0x10
; output " = "
put r ar1 0x20
put r pr2 0x05
int 0x10
put r ar1 0x3D
int 0x10
put r ar1 0x20
int 0x10
; output result
cpy r ar5 r ar1
put r pr2 0x04
int 0x10
; output newline
put r ar1 0x0A
put r pr2 0x05
int 0x10
cpy r ar5 r ar1
ret
;.dta section
raws .dta
lbl numbuffer
rawb 0x00
rawb 0x00
rawb 0x00
rawb 0x00
lbl errormsg
raws There was an error getting the number.
rawb 0x0a
rawb 0x0a
rawb 0x00
lbl baseprompt
raws Enter base:
rawb 0x20
rawb 0x00
lbl expprompt
raws Enter exponent:
rawb 0x20
rawb 0x00
