put m 0xFF 600
put r ar2 56
jmp l test
put r ar4 0xFF
hlt
lbl test
tst
;This is the equivalent of ret
pop r ar3
jmp r ar3

