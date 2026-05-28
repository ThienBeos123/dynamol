#include "_x86_conn.h"
#if __ARCH_X86_64__
#if __ABI_X64_WIN64__


asm_text_section
.intel_syntax   noprefix
.globl          _x86w64_full_halt
.globl          _x86w64_shallow_halt

.type           _x86w64_full_halt, %function
.type           _x86w64_shallow_halt, %function
.p2align	    4, 0x90





#endif
#endif