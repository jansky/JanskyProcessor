put r pr1 0x00
put r pr2 0x02
int 0x10
cpy r ar1 r ar2
put r pr1 0x01
put r pr2 0x01
cpy lo title r ar1
int 0x10
put r pr2 0x04
cpy r ar2 r ar1
int 0x10
put r pr2 0x01
cpy lo byte r ar1
int 0x10
put r pr2 0x04
div r ar2 a 1024
cpy r ar2 r ar1
int 0x10
put r pr2 0x01
cpy lo kilo r ar1
int 0x10
put r pr2 0x04
div r ar2 a 1024
cpy r ar2 r ar1
int 0x10
put r pr2 0x01
cpy lo mega r ar1
int 0x10
hlt
sec .dta
raws .dta
flbl title
raws Memory Statistics
rawb 0x0a
rawb 0x0a
rawb 0x00
flbl byte
rawb 0x20
raws bytes
rawb 0x0a
rawb 0x00
flbl kilo
rawb 0x20
raws kilobyte(s)
rawb 0x0a
rawb 0x00
flbl mega
rawb 0x20
raws megabyte(s)
rawb 0x0a
rawb 0x00
