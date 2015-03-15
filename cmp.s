put r ar1 0xFF
put r ar2 0xFE
cmp r ar1 r ar2
jmpc ne l ifequal
hlt
lbl ifequal
put r ar3 0xFF
ret
