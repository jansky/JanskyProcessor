raws .jos

put r pr1 0x01
put r pr2 0x01
cpy lo res1 r ar1
int 0x10

put r ar5 0x00
pop r ar1
pop r ar2
uip r ar1 r ar2

hlt

raws .dta

lbl res1
raws Hello, world!
rawb 0x0a
rawb 0x00
