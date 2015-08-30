; check if disk interop is enabled
put r pr1 0x03
put r pr2 0x01
put r ar1 0x01
int 0x10
cmp r ar1 a 1
jmpc ne l nointerop
; get file to load
put r pr1 0x01
put r pr2 0x01
cpy lo res5 r ar1
int 0x10
put r pr1 0x02
put r pr2 0x01
put r ar2 101
cpy lo res2 r ar1
int 0x10
; check for error
cmp r ar5 a 0
jmpc ne l inputerror
put r ar1 0x0a
put r pr1 0x01
put r pr2 0x05
int 0x10
int 0x10
; load file at location 0x8000
put r pr1 0x03
put r pr2 0x02
put r ar1 0x8000
cpy lo res2 r ar2
int 0x10
; check for errors
cmp r ar5 a 0
jmpc ne l ioerror
; uip to 0x8000, and hope it works
uip a 0x8000 a 0
hlt
; if no interop, print error and exit
lbl nointerop
put r pr1 0x01
put r pr2 0x01
cpy lo res1 r ar1
int 0x10
hlt
; if io error, print error and exit
lbl ioerror
put r pr1 0x01
put r pr2 0x01
cpy lo res3 r ar1
int 0x10
cpy r ar5 r ar1
put r pr2 0x02
int 0x10
put r pr2 0x01
cpy lo res4 r ar1
int 0x10
hlt
; if input error, print error and exit
lbl inputerror
put r pr1 0x01
put r pr2 0x01
cpy lo res6 r ar1
int 0x10
hlt
; .dta section
raws .dta
lbl res1
raws Disk interop is disabled.
rawb 0x0a
rawb 0x0a
rawb 0x00
lbl res3
raws Encounted error on reading file:
rawb 0x20
rawb 0x00
lbl res4
raws .
rawb 0x0a
rawb 0x0a
rawb 0x00
lbl res5
raws Enter program name (100 char max):
rawb 0x20
rawb 0x00
lbl res6
raws Error getting filename.
rawb 0x0a
rawb 0x0a
rawb 0x00
lbl res2
rawb 0x00
