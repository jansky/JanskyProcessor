put r ar1 0xFF
put r ar2 2
cpy r ar1 m 0xFF
cpy r ar2 m 0x103
div r ar1 r ar2
cpy r ar1 m 0x107
div r ar1 r ar2
cpy r ar1 m 0x10B
div m 0x10B r ar2
hlt
