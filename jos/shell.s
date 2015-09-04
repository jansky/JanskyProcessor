raws .jos
; print welcome message
put r pr1 0x01
put r pr2 0x01
cpy lo res1 r ar1
int 0x10

; shell loop
lbl shellloop
put r pr1 0x01
put r pr2 0x01
put r ar1 0x23
int 0x10
put r ar1 0x20
int 0x10

hlt


; .dta section
raws .dta
lbl res1
raws jOS Shell
rawb 0x0a
rawb 0x0a
rawb 0x00
