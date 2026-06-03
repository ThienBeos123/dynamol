#include "__x86_conn__.h"
#if __ARCH_X86_64__
#if __ABI_X64_WIN64__


asm_text_section
.intel_syntax   noprefix
.p2align	    4, 0x90
.globl          _x86w64_crt_lt
.globl          _x86w64_crt_gt
.globl          _x86w64_crt_leq
.globl          _x86w64_crt_geq

.type           _x86w64_crt_lt, %function
.type           _x86w64_crt_gt, %function
.type           _x86w64_crt_leq, %function
.type           _x86w64_crt_geq, %function


/* ------- UNSIGNED 64 BIT LESS THAN -------
* uint8_t _x86w64_crt_lt(uint64_t a, uint64_t b)
* arg:
*   rcx: uint64_t a       (64 BIT)
*   rdx: uint64_t b       (64 BIT)
* ret: rax -- 1 (true) | 0 (false)
*/
_x86w64_crt_lt:
    cmp rcx, rdx
    setb al
    movzx rax, al
    mov rcx, 0 ; register clearance
    mov rdx, 0 ; register clearance
    ret


/* ------- UNSIGNED 64 BIT GREATER THAN -------
* uint8_t _x86w64_crt_gt(uint64_t a, uint64_t b)
* arg:
*   rcx: uint64_t a       (64 BIT)
*   rdx: uint64_t b       (64 BIT)
* ret: rax -- 1 (true) | 0 (false)
*/
_x86w64_crt_gt:
    cmp rcx, rdx
    seta al
    movzx rax, al
    mov rcx, 0 ; register clearance
    mov rdx, 0 ; register clearance
    ret


/* ------- UNSIGNED 64 BIT LESS THAN OR EQUAL -------
* uint8_t _x86w64_crt_leq(uint64_t a, uint64_t b)
* arg:
*   rcx: uint64_t a       (64 BIT)
*   rdx: uint64_t b       (64 BIT)
* ret: rax -- 1 (true) | 0 (false)
*/
_x86w64_crt_leq:
    cmp rcx, rdx
    setbe al
    movzx rax, al
    mov rcx, 0 ; register clearance
    mov rdx, 0 ; register clearance
    ret



/* ------- UNSIGNED 64 BIT LESS THAN OR EQUAL -------
* uint8_t _x86w64_crt_geq(uint64_t a, uint64_t b)
* arg:
*   rcx: uint64_t a       (64 BIT)
*   rdx: uint64_t b       (64 BIT)
* ret: rax -- 1 (true) | 0 (false)
*/
_x86w64_crt_geq:
    cmp rcx, rdx
    setae al
    movzx rax, al
    mov rcx, 0 ; register clearance
    mov rdx, 0 ; register clearance
    ret



#endif
#endif