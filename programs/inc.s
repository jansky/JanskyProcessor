;This does not seem to work
;We get a stack overflow
put r ar2 0xFF
push a 0x00
lbl compare
pop r ar3
cmp r ar1 r ar2
jmpc ne l increment
hlt
lbl increment
add r ar1 a 1
pop r ar3
jmp l compare

