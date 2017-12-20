#include "jos.h"
; .dta section
raws .dta
glbl jos_version
rawb JOSREV
rawb 0x00
rawb 0x00
rawb 0x00
glbl jos_ok
raws OK
rawb 0x00
glbl jos_fail
raws FAIL
rawb 0x00
glbl jos_offset
room 4
glbl jos_startupfail
rawb 0x0a
raws jOS startup failed.
rawb 0x0a
rawb 0x0a
rawb 0x00
glbl jos_magicnum
raws .jos
glbl initlog_res1
raws Welcome to jOS
rawb 0x0a
rawb 0x00
glbl initlog_res2
raws Revision
rawb 0x20
rawb 0x00
glbl checkinterop_res1
raws Checking for interop...
rawb 0x20
rawb 0x00
glbl interrupts_res1
raws Registering interrupts...
rawb 0x20
rawb 0x00
glbl loadshell_res1
raws Could not load shell.bin. Error
rawb 0x20
rawb 0x00
glbl loadshell_res2
raws shell.bin
rawb 0x00
glbl end_res1
rawb 0x0a
rawb 0x0a
raws System exited.
rawb 0x0a
rawb 0x0a
rawb 0x00
