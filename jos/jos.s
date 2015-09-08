#include "jos.h"

; load everything and find and execute shell
jmp l initlog
jmp l checkinterop
jmp l interrupts
put r pr1 0
put r pr2 0
jmp l loadshell
; when we are back here the shell has exited
put r pr1 0x01
put r pr2 0x01
cpy lo end_res1 r ar1
int 0x10
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
;add r ar1 a 0x8000
;cpy lo loadshell_res2 r ar2
cpy lo loadshell_res2 r nr1
push a 0x8000
push r nr1
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
; temporary fix, os must be loaded at 0x400 now
put r or1 0x400
cpy l systemcall r bp
iret

#include "syscall.s"
#include "dta.s"



