; location 0x0
put r ar1 0x10
; location 0xa
put r ar2 0x4
; location 0x14
put r ar3 0x1
; location 0x1e
cpy r ar1 r ar4
; location 0x29
cmp r ar2 a 0x0
; location 0x34
jmpc eq a 0x6f
; location 0x3b
cmp r ar3 r ar2
; location 0x46
jmprc ne a 0x58 a 0x3b
; location 0x52
jmp a 0x80
; location 0x58
mul r ar1 r ar4
; location 0x63
add r ar3 a 0x1
; location 0x6e
ret
; location 0x6f
put r ar1 0x1
; location 0x79
jmp a 0x80
; location 0x7f
hlt
; location 0x80
cpy r ar1 r ar5
; location 0x8b
cpy r ar4 r ar1
; location 0x96
put r pr1 0x1
; location 0xa0
put r pr1 0x4
; location 0xaa
int 0x10
; location 0xac
put r ar1 0x20
; location 0xb6
put r pr1 0x5
; location 0xc0
int 0x10
; location 0xc2
put r ar1 0x5e
; location 0xcc
int 0x10
; location 0xce
put r ar1 0x20
; location 0xd8
int 0x10
; location 0xda
cpy r ar2 r ar1
; location 0xe5
put r pr1 0x4
; location 0xef
int 0x10
; location 0xf1
put r ar1 0x20
; location 0xfb
put r pr1 0x5
; location 0x105
int 0x10
; location 0x107
put r ar1 0x3d
; location 0x111
int 0x10
; location 0x113
put r ar1 0x20
; location 0x11d
int 0x10
; location 0x11f
cpy r ar5 r ar1
; location 0x12a
put r pr1 0x4
; location 0x134
int 0x10
; location 0x136
put r ar1 0xa
; location 0x140
put r pr1 0x5
; location 0x14a
int 0x10
; location 0x14c
cpy r ar5 r ar1
; location 0x157
ret
