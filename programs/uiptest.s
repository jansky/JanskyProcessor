put r nr1 16
put r nr2 2
push r or1
cpy l jmpto r ar1
push r ar1
cpy r or1 r ar2
cpy l uiptest r ar1
add r ar1 r ar2
uip a 0 r ar1
hlt
lbl jmpto
put r nr4 0x100
hlt
lbl uiptest
mul r nr1 r nr2
put r nr3 0x50
pop r ar3
pop r ar4
uip r ar4 r ar3
