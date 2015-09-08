; .dta section
raws .dta
lbl jos_version
rawb JOSREV
rawb 0x00
rawb 0x00
rawb 0x00
lbl jos_ok
raws OK
rawb 0x00
lbl jos_fail
raws FAIL
rawb 0x00
lbl jos_offset
room 4
lbl jos_startupfail
rawb 0x0a
raws jOS startup failed.
rawb 0x0a
rawb 0x0a
rawb 0x00
lbl jos_magicnum
raws .jos
lbl initlog_res1
raws Welcome to jOS
rawb 0x0a
rawb 0x00
lbl initlog_res2
raws Revision
rawb 0x20
rawb 0x00
lbl checkinterop_res1
raws Checking for interop...
rawb 0x20
rawb 0x00
lbl interrupts_res1
raws Registering interrupts...
rawb 0x20
rawb 0x00
lbl loadshell_res1
raws Could not load shell.bin. Error
rawb 0x20
rawb 0x00
lbl loadshell_res2
raws shell.bin
rawb 0x00
lbl end_res1
rawb 0x0a
rawb 0x0a
raws System exited.
rawb 0x0a
rawb 0x0a
rawb 0x00
