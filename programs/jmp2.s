;Note: Nested jumps are supported, but stack space IS LIMITED
put r ar2 2
jmp l test
hlt
lbl test
tst
jmp l div
jmp l div
ret
lbl div
div r ar1 r ar2
cpy r ar1 r ar2
ret
