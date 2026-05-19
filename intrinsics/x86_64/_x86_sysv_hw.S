#include "_x86_conn.h"
#if __ARCH_X86_64__
#if __ABI_X64_SYSV__


asm_text_section
.intel_syntax   noprefix
.globl          _x86sv_full_halt
.globl          _x86sv_shallow_halt



.type           _x86sv_full_halt, %function
.type           _x86sv_shallow_halt, %function
.p2align	    4, 0x90




; Halt CPU Execution by a few cycles via the PAUSE instruction,
; Which does a system flush or whatever I'm sorry dawg
/* ----- HARDWRE CPU EXECUTION HALT - FULL SYSTEM FLUSH -----
* void _x86sv_full_halt(void)
* arg: NONE
* ret: NONE
*/
_x86sv_full_halt:
    pause
    ret


; Halt CPU Execution by a single cycles via the 9-byte NOP instruction.
; It basically does nothing, and is a really shallow NOP, in
; which it delays about at 1 CPU cycle / call
/* ----- HARDWRE CPU EXECUTION HALT - SHALLOW NOP HALTH -----
* void _x86sv_shallow_halt(void)
* arg: NONE
* ret: NONE
*/
_x86sv_shallow_halt:
    nop [eax + eax*1 + 00000000h]
    ret





#endif
#endif