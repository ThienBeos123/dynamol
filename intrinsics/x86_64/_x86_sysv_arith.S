/*
Copyright (C) 2026 @ThienBeos123/@Poly-glon

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

  http://apache.org

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/



#include "__x86_conn__.h"
#if __ARCH_X86_64__
#if __ABI_X64_SYSV__


asm_text_section
.intel_syntax   noprefix
.globl	        _x86sv_add64c
.globl          _x86sv_sub64b
.globl          _x86sv_wmul128
.globl          _x86sv_wdiv128
.globl          _x86sv_crt_wdiv128

.type	        _x86sv_add64c, %function
.type           _x86sv_sub64b, %function
.type           _x86sv_wmul128, %function
.type           _x86sv_wdiv128, %function
.type           _x86sv_crt_wdiv128, %function
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
    /* ----- Memory / Register Clearance ----- */
    mov rdi, 0
    mov rsi, 0
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
    /* ----- Memory / Register Clearance ----- */
    mov rdi, 0
    mov rsi, 0
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
    /* ----- Memory / Register Clearance ----- */
    mov rdi, 0
    mov rsi, 0
    mov rcx, 0
    ret


/* ------- 64 BIT WIDE DIVISION OF 128 BIT DIVIDEND - PERFORMANCE-BASED -------
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
    /* Early Returns  */
    test rdx, rdx
    jz .zero_div // if (!div)
    // if (!lo && !hi)
    mov r9, rdi
    or  r9, rsi
    jz .zero_dend
    // if (!hi)
    test rsi, rsi
    jz .zero_hi
    // if (hi >= div)
    cmp rsi, rdx
    jae .overflow

    /* Division + Returns */
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

/* Early Return Blocks */
.zero_dend:
    mov rdx, 0
    mov [rcx], rdx // *rhat = 0
    mov rax, 0 // return 0
    ; if (overflow != NULL) *overflow = 1
    test r8, r8
    .jz .done
    mov byte [r8], 0
    ret
.zero_hi:
    mov rax, rdi
    mov r9,  rdx
    xor rdx, rdx
    div r9 // lo / div
    ; rax = quot | rdx = rem
    mov [rcx], rdx // *rhat = lo % div
    // return is already in RAX
    ; if (overflow != NULL) *overflow = 1
    test r8, r8
    .jz .done_overflow
    mov byte [r8], 0
    ret
.done:
    ret

/* Overflow hanndling blocks */
.zero_div:
    mov rdx, 0
    mov [rcx], rdx // *rhat = 0
    mov rax, -1 // return UINT64_MAX
    ; if (overflow != NULL) *overflow = 1
    test r8, r8
    .jz .done_overflow
    mov byte [r8], 1
    ret
.overflow:
    mov rax, rsi
    mov r9,  rdx
    xor rdx, rdx
    div r9
    ; rax = quot | rdx = rem
    mov [rcx], rdx // *rhat = hi % div
    mov rax, -1 // return UINT64_MAX
    ; if (overflow != NULL) *overflow = 1
    test r8, r8
    .jz .done_overflow
    mov byte [r8], 1
    ret
.done_overflow:
    ret



/* ------- 64 BIT WIDE DIVISION OF 128 BIT DIVIDEND - CRYTOGRAPHY-BASED -------
* uint64_t _x86sv_crt_wdiv128(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rem, uint8_t *ovf)
* arg:
*   rdi: uint64_t lo ----> (64 BIT)
*   rsi: uint64_t hi ----> (64 BIT)
*   rdx: uint64_t div ---> (64 BIT)
*   rcx: uint64_t *rem --> (64 BIT)
*   r8:  uint8_t *ovf -> (1 | 0 -- 64 BIT ADRESS)
* ret:
*   rax - quot: ([lo:hi] / div)
*   rcx - rhat: ([lo:hi] % div)
* pre-conditions: hi/rsi < div
*/
_x86sv_crt_wdiv128:
    ; =========== 1. Determine Overflow Flag & Write to Memory ===========
    test rdx, rdx ; div == 0?
    setz r9b ; r9b = (div == 0) ? 1 : 0
    cmp rsi, rdx ; Compare hi and div
    setae r10b ; r10b = (hi >= div) ? 1 : 0
    or r9b, r10b ; overflow_flag = (hi >= div) | (div == 0)
    mov byte ptr [r8], r10b ; *overflowed = overflow_flag
    

    ; =========== 2. Multiplexing of Fake vs Real Values ===========
    mov r9, 1 ; fake_hi = 1
    mov rax, 0x1928356720912386 ; fake_lo = 0x1928356720912386 
    test r10b, r10b
    cmovnz rdi, rax ; chosen_lo = overflow_flag ? fake_lo : lo
    cmovnz rsi, r9 ; chosen_hi = overflow_flag ? fake_hi : hi
    ; Loop Configurations
    xor rax, rax ; q = 0
    xor r9, r9 ; Loop counter (i)
    /* Variables-Registers List:
        - rdi: chosen_lo
        - rsi: chosen_hi
        - rax: q - RETURN QUOTIENT
        - r9d: i - LOOP COUNTER
        - r10: OVERFLOW FLAG
    */
.crt_div_loop:
    ; Early high bit overflow check
    mov r11, rsi ; r11 = rsi (chosen_hi)
    shr r11, 63 ; r11 = rsi >> 63 (Isolating the MSB)
    and r11, 1 ; r11 = (rsi >> 63) & 1 (Explicitly isolating the MSB)

    ; Normal, 128-bit dividend shifting routine
    shl rax, 1 ; q <<= 1
    ; test_rem = (chosen_hi << 1) | ((chosen_lo >> i) & 1)
    ; In this implementation, we operate directly on rsi (chosen_hi)
    mov r10, rdi ; r10 = chosen_lo (rdi)
    shr r10, 63 ; r10 = chosen_lo << 63
    and r10, 1 ; r10 = (chosen_lo << 63) & 1
    shl rdi, 1 ; chosen_lo (rdi) <<= 1
    add rsi, rsi ; chosen_hi (rsi) <<= 1
    or rsi, r10 ; chosen_hi |= extracted_bit


    /* --- Quotient & Remainder value manipulation --- */
    xor rbx, rbx ; Clean the RBX register
    cmp rsi, rdx ; (chosen_hi >= div)?
    setae al ; al = (chosen_hi >= div)?
    or rbx, r11 ; rbx |= r11 (chosen_hi >= div | hi_overflow)
    neg rbx ; mask = -(rbx_cond) --> -1 (UINT64_MAX) : 0
    /* Quotient Calculations */
    mov r11, rbx
    and r11, 1 ; 1 & mask
    or rax, r11 ; q |= (1 & mask)
    /* Remainder Calculations */
    mov r11, rdx
    and r11, rbx ; div & mask
    sub rsi, r11 ; chosen_hi -= (div & mask)
    ; Incrementing loop counter
    inc r9d
    cmp r9d, 64
    jl .crt_div_loop


    ; =========== 5. Final Output Selections ===========
    test r10b, r10b ; Check overflow flag again
    ; ----- QUOTIENT RETURNS -----
    mov r10, -1 ; r10 = (uint64_t)-1 = UINT64_MAX (0xFFFFFFFFFFFFFFFF)
    cmovnz rax, r10 ; rax ? (overflow_flag/r10b) ? UINT64_MAX : quot
    ; ----- REMAINDER RETURNS -----
    mov rdi, rsi ; rdi = chosen_hi (rsi)
    mov r11, 0 ; r11 = 0
    cmovnz rdi, r11 ; rdi ? (overflow_flag/r10b) ? 0 (r11) : chosen_hi (rdi)
    mov qword ptr [rcx], rdi ; *rhat (*rcx) = rdi (selected remainder)


    ; =========== 6. Aggressive Post-operation Clearance ===========
    xor rbx, rbx
    xor rcx, rcx
    xor rdx, rdx
    xor rsi, rsi
    xor rdi, rdi
    xor r8, r8
    xor r9, r9
    xor r10, r10
    xor r11, r11
    ret




#endif
#endif
