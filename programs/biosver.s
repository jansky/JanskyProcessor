; gets the BIOS version
; ar1 - BIOS version output
put r pr1 0x00
put r pr2 0x01
int 0x10
hlt