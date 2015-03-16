; Basic implementation of a pow() function
; ar1 - number to raise to power
; ar2 - power to raise to
; Outputs
; ar1 - ar1^ar2
; Uses
; ar1, ar2, ar3, ar4
put r ar1 16
put r ar2 3
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
hlt
lbl pow
mul r ar1 r ar4
add r ar3 a 1
ret
lbl zero
; Any number to the zero power is 1
put r ar1 1
hlt

