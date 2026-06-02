#include "__x86_conn__.h"
#if __ARCH_X86_64__
#if __ABI_X64_SYSV__


asm_text_section
.intel_syntax   noprefix
.p2align	    4, 0x90
.globl          _x86sv_crt_ispos
.globl          _x86sv_crt_isneg
.globl          _x86sv_crt_eq
.globl          _x86sv_crt_neq
.globl          _x86sv_crt_select

.type           _x86sv_crt_ispos, %function
.type           _x86sv_crt_isneg, %function
.type           _x86sv_crt_eq, %function
.type           _x86sv_crt_neq, %function
.type           _x86sv_crt_select, %function


/* ------- 64 BIT POSITIVITY CHECK -------
* uint8_t _x86sv_crt_ispos(int64_t a)
* arg: rdi -- int64_t a    (64 BIT)
* ret: rax -- 1 (true) | 0 (false)
*/
_x86sv_crt_ispos:
    test rdi, rdi ; Test if positive
    sets al ; Set sign
    movzx rax, al ; Zero extension
    mov rdi, 0 # Register Clearance
    ret


/* ------- 64 BIT NEGATIVITY CHECK -------
* uint8_t _x86sv_crt_isneg(int64_t a)
* arg: rdi -- int64_t a    (64 BIT)
* ret: rax -- 1 (true) | 0 (false)
*/
_x86sv_crt_isneg:
    test rdi, rdi ; Test if negative
    setns al ; Set if NEGATIVE sign
    movzx rax, al ; Zero extension
    mov rdi, 0 # Register Clearance
    ret


/* ------- 64 BIT EQUALITY -------
* uint8_t _x86sv_crt_eq(uint64_t a, uint64_t b)
* arg:
*   rdi: uint64_t a       (64 BIT)
*   rsi: uint64_t b       (64 BIT)
* ret: rax -- 1 (true) | 0 (false)
*/
_x86sv_crt_eq:
    cmp rdi, rsi
    sete al
    movzx rax, al ; Zero extension
    mov rdi, 0 # Register Clearance
    mov rsi, 0 # Register Clearance
    ret



/* ------- 64 BIT INEQUALITY -------
* uint8_t _x86sv_crt_neq(uint64_t a, uint64_t b)
* arg:
*   rdi: uint64_t a       (64 BIT)
*   rsi: uint64_t b       (64 BIT)
* ret: rax -- 1 (true) | 0 (false)
*/
_x86sv_crt_neq:
    cmp rdi, rsi
    setne al
    movzx rax, al ; Zero extension
    mov rdi, 0 # Register Clearance
    mov rsi, 0 # Register Clearance
    ret


/* ------- 64 BIT SELECTION -------
* uint8_t _x86sv_crt_select(uint8_t cond, uint64_t a, uint64_t b)
* arg:
*   rdi: uint8_t cond     (8 BIT -- 1 | 0)
*   rsi: uint64_t a       (64 BIT)
*   rdx: uint64_t b       (64 BIT)
* ret: EITHER x1 OR x2
*/
_x86sv_crt_select:
    test rdi, rdi ; Test the condition register
    mov rax, rdx ; Assume the condition is false ---> rax = b
    cmovnz rax, rsi
    ; If test evaluates to TRUE ---> rax = a
    ; ELSE (test evaluates to FALSE) ---> cmovnz is IGNORED
    mov rdi, 0 # Register Clearance
    mov rsi, 0 # Register Clearance
    mov rdx, 0 # Register Clearance
    ret


#endif
#endif