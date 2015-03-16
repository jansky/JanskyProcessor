put r ar2 0xFF
lbl loop
cmp r ar1 r ar2
jmprc ne l increment l loop
hlt
lbl increment
add r ar1 a 1
ret
