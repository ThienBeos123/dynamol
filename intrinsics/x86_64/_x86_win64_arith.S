#include "_x86_conn.h"
#if __ARCH_X86_64__
#if __ABI_X64_WIN64__


asm_text_section
.intel_syntax   noprefix
.globl	        _x86w64_add64c
.globl          _x86w64_sub64b
.globl          _x86w64_wmul128
.globl          _x86w64_wdiv128

.type	        _x86w64_add64c, %function
.type           _x86w64_sub64b, %function
.type           _x86w64_wmul128, %function
.type           _x86w64_wdiv128, %function
.p2align        4, 0x90



/* ------- 64 BIT ADDITION WITH CARRY -------
* uint64_t _x86w64_add64c(uint64_t a, uint64_t b, uint8_t *carry)
* arg: 
*   rcx: uint64_t a       (64 BIT)
*   rdx: uint64_t b       (64 BIT)
*   r8:  uint8_t *carry   (8  BIT)
* ret: 
*   rax: (a + b + *carry) mod(2^64)
*   r8:  boolean - 1/0
*/
_x86w64_add64c:
    mov rax, qword ptr [r8] ; Load *carry into rax
    add rax, -1 ; rax + UINT64_MAX;
    ; +) If rax = 1 ---> UINT64_MAX + 1 overflows --> C flag set (carry)
    ; +) If rax = 0 ---> UINT64_MAX + 0 is normal --> C flag not set (no carry)
    ; Add a = a + b + *carry
    ; --> rcx = rcx + rdx + C flag
    adc rcx, rdx

    xor rax, rax ; Zeroing rax
    setc    al ; al (8bit-rax) = C flag
    movzx rax, al ; Zero extends al's into 64 bit rax

    mov [r8], rax ; *r8/*carry = rax
    mov rax, rcx ; return rcx
    ret


/* ------- 64 BIT SUBTRACTION WITH BORROW -------
* uint64_t _x86w64_sub64b(uint64_t a, uint64_t b, uint8_t *borrow)
* arg: 
*   rcx: uint64_t a       (64 BIT)
*   rdx: uint64_t b       (64 BIT)
*   r8:  uint8_t *borrow  (8  BIT)
* ret: 
*   rax: (a - b - *borrow) mod(2^64)
*   r8:  boolean - 1/0
*/
_x86w64_sub64b:
    mov rax, qword ptr [r8] ; Load *borrow into rax
    bt  rax, 0 ; Copies rax_bits[0] into C_flag
    ; Subtract a = a - b - *borrow
    ; -------> rcx = rcx - rdx - C flag
    sbc rcx, rdx

    xor rax, rax ; Clearing out rax
    setc    al ; Set rax/al based on C flag
    movzx rax, al ; Zero-extends al into rax
    
    mov [r8], rax ; *carry/*rdi = rax
    mov rax, rcx ; return rdi
    ret


/* ------- 64 BIT WIDE MULTIPLICATION --> 128 BIT PRODUCT -------
* uint64_t _x86w64_wmul128(uint64_t a, uint64_t b, uint64_t *hi) 
* arg:
*   rcx: uint64_t a ----> (64 BIT)
*   rdx: uint64_t b ----> (64 BIT)
*   r8:  uint64_t *hi --> (64 BIT)
* ret:
*   rax - lo: (a * b) mod(2^64)
*   r8  - hi: (a * b) >> 64;
*/
_x86w64_wmul128:
    mov rax, rcx ; rax = rcx/a
    mul rdx ; rax * rdx --> stored in [rdx:rax]
    mov [r8], rdx ; *r8 = rdx (the high 64 bit)
    ret


/* ------- 64 BIT WIDE DIVISION OF 128 BIT DIVIDEND -------
* uint64_t _x86sv_wdiv128(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rem, uint8_t *overflowed) 
* arg:
*   rcx:        uint64_t lo ----> (64 BIT)
*   rdx:        uint64_t hi ----> (64 BIT)
*   r8:         uint64_t div ---> (64 BIT)
*   r9:         uint64_t *rem --> (64 BIT)
*   [RSP + 40]: uint8_t *overflowed --> (1 | 0)
* ret:
*   rax - quot: ([lo:hi] / div)
*   r9  - rhat: ([lo:hi] % div)
* pre-conditions: hi/rsi < div
*/
_x86w64_wdiv128:
    cmp rdx, r8
    ja .overflow

    ; Divison + Returns
    mov rax, rcx ; rax = lo
    div r8 ; [rdx:rax] / r8
    ; rax = quot | rdx = rem
    mov [r9], rdx
    ; if (overflowed != NULL) *overflowed = 0
    mov r10, [rsp + 40]
    test r10, r10
    jz .done
    mov byte [r10], 0
    ret
.done:
    ret

/* Overflow handling blocs */
.overflow:
    mov rax, rdx
    xor rdx, rdx
    div r8
    ; rax = quot | rdx = rem
    mov [r9], rdx
    mov rax, -1
    ; if (overflowed != NULL) *overflowed = 1
    mov r10, [rsp + 40]
    test r10, r10
    jz .done_overflow
    mov byte [r10], 1
    ret
.done_overflow:
    ret


#endif
#endif
