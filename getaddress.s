put m 0xFF 600
put r ar2 56
jmp l test
hlt
lbl test
tst
jmp l getaddress
ret
lbl getaddress
pop r ar1
push r ar1
ret
