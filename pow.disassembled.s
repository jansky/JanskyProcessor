put r ar1 0x10
put r ar2 0x3
put r ar3 0x1
cpy r ar1 r ar4
cmp r ar2 a 0x0
jmpc eq a 0x6a
cmp r ar3 r ar2
jmprc ne a 0x53 a 0x3b
hlt
mul r ar1 r ar4
add r ar3 a 0x1
ret
put r ar1 0x1
hlt
