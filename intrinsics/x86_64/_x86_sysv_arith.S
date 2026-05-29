#include "_x86_conn.h"
#if __ARCH_X86_64__
#if __ABI_X64_SYSV__


asm_text_section
.intel_syntax   noprefix
.globl	        _x86sv_add64c
.globl          _x86sv_sub64b
.globl          _x86sv_wmul128
.globl          _x86sv_wdiv128

.type	        _x86sv_add64c, %function
.type           _x86sv_sub64b, %function
.type           _x86sv_wmul128, %function
.type           _x86sv_wdiv128, %function
.p2align	    4, 0x90


/* ------- 64 BIT ADDITION WITH CARRY -------
* uint64_t _x86sv_add64c(uint64_t a, uint64_t b, uint8_t *carry)
* arg: 
*   rdi: uint64_t a       (64 BIT)
*   rsi: uint64_t b       (64 BIT)
*   rdx: uint8_t *carry   (64 BIT ADRESS)
* ret: 
*   rax: (a + b + *carry) mod(2^64)
*   rdx: boolean - 1/0
*/
_x86sv_add64c:
    mov rax, qword ptr [rdx] ; Load *carry into rax
    add rax, -1 ; rax + UINT64_MAX:
    ; +) If rax = 1 ---> UINT64_MAX + 1 overflows --> C flag set (carry)
    ; +) If rax = 0 ---> UINT64_MAX + 0 is normal --> C flag not set (no carry)
    ; Add a = a + b + *carry
    ; --> rdi = rdi + rsi + C flag
    adc rdi, rsi

    xor rax, rax ; Clearing out rax
    setc    al ; Set rax/al based on C flag
    movzx rax, al ; Zero-extends al into rcx

    mov [rdx], rax ; *carry/*rdx = rax
    mov rax, rdi ; return rdi
    ret


/* ------- 64 BIT SUBTRACTION WITH BORROW -------
* uint64_t _x86sv_sub64b(uint64_t a, uint64_t b, uint8_t *borrow)
* arg: 
*   rdi: uint64_t a       (64 BIT)
*   rsi: uint64_t b       (64 BIT)
*   rdx: uint8_t *borrow  (64 BIT ADRESS)
* ret: 
*   rax: (a - b - *borrow) mod(2^64)
*   rdx: boolean - 1/0
*/
_x86sv_sub64b:
    mov rax, qword ptr [rdx] ; Load *borrow into rax
    bt  rax, 0 ; Copies rax_bits[0] into C_flag
    ; Subtract a = a - b - *borrow
    ; -------> rdi = rdi - rsi - C flag
    sbc rdi, rsi

    xor rax, rax ; Clearing out rax
    setc    al ; Set rax/al based on C flag
    movzx rax, al ; Zero-extends al into rax
    
    mov [rdx], rax ; *carry/*rdi = rax
    mov rax, rdi ; return rdi
    ret


/* ------- 64 BIT WIDE MULTIPLICATION --> 128 BIT PRODUCT -------
* uint64_t _x86sv_wmul128(uint64_t a, uint64_t b, uint64_t *hi) 
* arg:
*   rdi: uint64_t a ----> (64 BIT)
*   rsi: uint64_t b ----> (64 BIT)
*   rdx: uint64_t *hi --> (64 BIT)
* ret:
*   rax - lo: (a * b) mod(2^64)
*   rdx - hi: (a * b) >> 64;
*/
_x86sv_wmul128:
    mov rax, rdi ; rax = rdi/a
    mov rcx, rdx ; rcx = rdx (the pointer adress)
    mul rsi ; rax * rsi --> stored in [rdx:rax]
    mov [rcx], rdx ; *rcx = rdx (the high 64 bit)
    mov rdx, rcx ; rdx = rcx (return the original adress to rdx)
    ret


/* ------- 64 BIT WIDE DIVISION OF 128 BIT DIVIDEND -------
* uint64_t _x86sv_wdiv128(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rem, uint8_t *overflowed) 
* arg:
*   rdi: uint64_t lo ----> (64 BIT)
*   rsi: uint64_t hi ----> (64 BIT)
*   rdx: uint64_t div ---> (64 BIT)
*   rcx: uint64_t *rem --> (64 BIT)
*   r8:  uint8_t *overflowed -> (1 | 0 -- 64 BIT ADRESS)
* ret:
*   rax - quot: ([lo:hi] / div)
*   rcx - rhat: ([lo:hi] % div)
* pre-conditions: hi/rsi < div
*/
_x86sv_wdiv128:
    cmp rsi, rdx
    ja .overflow

    ; Division + Returns
    mov r9, rdx ; r9 = div
    mov rdx, rsi ; rdx = hi
    mov rax, rdi ; rax = lo
    div r9 ; [rdx:rax] / r9
    ; rax = quot | rdx = rem
    mov [rcx], rdx
    ; if (overflow != NULL) *overflow = 0
    test r8, r8
    .jz .done
    mov byte [r8], 0
    ret
.done:
    ret
    
/* Overflow hanndling blocks */
.overflow:
    mov rax, rsi
    mov r9,  rdx
    xor rdx, rdx
    div r9
    ; rax = quot | rdx = rem
    mov [rcx], rdx
    mov rax, -1
    ; if (overflow != NULL) *overflow = 1
    test r8, r8
    .jz .done_overflow
    mov byte [r8], 1
    ret
.done_overflow:
    ret

#endif
#endif