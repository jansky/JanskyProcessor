; load everything and find and execute shell
jmp l initlog
jmp l checkinterop
jmp l interrupts
jmp l loadshell
; when we are back here the shell has exited
hlt

; initlog - tell the user hello
lbl initlog
put r pr1 0x01
put r pr2 0x01
cpy lo initlog_res1 r ar1
int 0x10
cpy lo initlog_res2 r ar1
int 0x10
cpy lo jos_version r nr1
cpy pr nr1 r ar1
put r pr2 0x04
int 0x10
put r ar1 0x0a
put r pr2 0x05
int 0x10
int 0x10
ret

; checkinterop - ensure disk interop support is enabled
lbl checkinterop
put r pr1 0x01
put r pr2 0x01
cpy lo checkinterop_res1 r ar1
int 0x10
put r pr1 0x03
put r pr2 0x01
put r ar1 0x01
int 0x10
cmp r ar1 a 1
jmpc ne l checkinterop_fail
put r pr1 0x01
put r pr2 0x01
cpy lo jos_ok r ar1
int 0x10
put r pr2 0x05
put r ar1 0x0a
int 0x10
ret
lbl checkinterop_fail
put r pr1 0x01
put r pr2 0x01
cpy lo jos_fail r ar1
int 0x10
cpy lo jos_startupfail r ar1
int 0x10
hlt

; interrupts - register software interrupts
lbl interrupts
put r pr1 0x01
put r pr2 0x01
cpy lo interrupts_res1 r ar1
int 0x10
cpy lo jos_offset r ar1
cpy r or1 pr ar1
; register interrupt 0x6a
cpy lo interrupthandler m 424
put r pr1 0x00
int 0x6a
cmp r nr7 a 0x6a
jmpc ne l interrupts_error
put r pr1 0x01
put r pr2 0x01
cpy lo jos_ok r ar1
int 0x10
put r pr2 0x05
put r ar1 0x0a
int 0x10
ret

lbl interrupts_error
put r pr1 0x01
put r pr2 0x01
cpy lo jos_fail r ar1
int 0x10
cpy lo jos_startupfail r ar1
int 0x10
hlt

; loadshell - load the shell
lbl loadshell
put r pr1 0x02
add r ar1 a 0x8000
cpy lo loadshell_res2 r ar2
int 0x6a
cmp r ar5 a 0
jmpc ne l loadshellerror
ret

lbl loadshellerror
put r pr1 0x01
put r pr2 0x01
cpy lo loadshell_res1 r ar1
int 0x10
put r pr2 0x02
cpy r ar5 r ar1
int 0x10
put r pr2 0x05
put r ar1 0x0a
int 0x10
int 0x10
hlt



; interrupthandler - handles os system calls
lbl interrupthandler
; save where to return to
push r bp
push r or1
cpy lo jos_offset r nr1
cpy pr nr1 r or1
cpy l systemcall r bp
iret

; systemcall - performs the system call requested
lbl systemcall
cmp r pr1 a 0x00
jmprc eq l systemcall_test l systemcallend
cmp r pr1 a 0x01
jmprc eq l systemcall_getosrev l systemcallend
cmp r pr1 a 0x02
jmprc eq l systemcall_execfile l systemcallend
lbl systemcallend
pop r ar1
pop r ar2
uip r ar1 r ar2

lbl systemcall_test
put r nr7 0x6a
ret

lbl systemcall_getosrev
cpy lo jos_version r ar1
cpy pr ar1 r ar1
ret

lbl systemcall_execfile
put r pr1 0x03
put r pr2 0x02
int 0x10
cmp r ar5 a 0
jmpc ne l systemcall_execfile_return
; verify that program loaded is a valid jOS program
cpy lo jos_magicnum r nr1
cmp pr ar1 r nr1
jmprc ne l systemcall_execfile_invaliderror l systemcall_execfile_return
; execute the program
add r ar1 a 4
cpy lo systemcall_execfile_return r ar3
push r ar3
push r or1
uip r ar1 a 0

lbl systemcall_execfile_invaliderror
put r ar5 0x100
ret


lbl systemcall_execfile_return
pop r nr7
ret

; .dta section
raws .dta
lbl jos_version
rawb 0x01
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
lbl end
room 4



