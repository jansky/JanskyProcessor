put r pr1 0x01
put r pr2 0x01
cpy l helloworldstr r ar1
int 0x10
hlt
raws .dta
lbl helloworldstr
raws Hello World! This is a test program.
;newline and end of string
rawb 0x0a
rawb 0x00