raws .jos

; print welcome message
put r pr1 0x01
put r pr2 0x01
cpy lo res1 r ar1
int 0x10


; shell loop
lbl shellloop
put r ar5 0x00
pop r nr7
put r pr1 0x01
put r pr2 0x05
put r ar1 0x23
int 0x10
put r ar1 0x20
int 0x10

; get input
put r pr1 0x02
put r pr2 0x01
cpy lo res3 r ar1
put r ar2 200
int 0x10

cmp r ar5 a 0
jmpc ne l error_input

put r nr6 0x150

; execute the filename given
put r pr1 0x02
put r pr2 0x00
cpy lo res3 r ar1
push a 0x20000
push r ar1
int 0x6a
cmp r ar5 a 0
jmpc ne l error_exec

jmp l shellloop

hlt

lbl end
; give control back to os (i.e. exit)
pop r nr7
put r ar5 0x00
pop r ar1
pop r ar2
uip r ar1 r ar2

hlt

lbl error_input
pop r nr7
put r pr1 0x01
put r pr2 0x01
cpy lo res4 r ar1
jmp l shellloop

lbl error_exec
pop r nr7
put r pr1 0x01
put r pr2 0x01
cpy lo res5 r ar1
int 0x10
put r pr2 0x02
cpy r ar5 r ar1
int 0x10
put r pr2 0x05
put r ar1 0x0a
int 0x10
int 0x10
jmp l shellloop

hlt


; .dta section
raws .dta
lbl res1
raws jOS Shell
rawb 0x0a
rawb 0x0a
rawb 0x00
lbl res2
raws exit
lbl res3
room 202
lbl res4
raws There was an error getting the input.
rawb 0x0a
rawb 0x00
lbl res5
raws There was an error:
rawb 0x20
rawb 0x00
lbl res6
raws exit
rawb 0x00


