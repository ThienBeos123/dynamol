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



;include "__x86_conn__.h"
;if __ARCH_X86_64__
;if __ABI_X64_WIN64__


asm_text_section
.intel_syntax   noprefix
.globl	        _x86w64_add64c
.globl          _x86w64_sub64b
.globl          _x86w64_wmul128
.globl          _x86w64_wdiv128
.globl          _x86w64_crt_wdiv128

.type	        _x86w64_add64c, %function
.type           _x86w64_sub64b, %function
.type           _x86w64_wmul128, %function
.type           _x86w64_wdiv128, %function
.type           _x86w64_crt_wdiv128, %function
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
    /* ----- Register Clearance ----- */
    mov rcx, 0 ; Function arguments
    mov rdx, 0 ; Function arguments
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
    /* ----- Register Clearance ----- */
    mov rcx, 0 ; Function arguments
    mov rdx, 0 ; Function arguments
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
    /* ----- Register Clearance ----- */
    mov rcx, 0 ; Function arguments
    mov rdx, 0 ; Function arguments
    ret


/* ------- 64 BIT WIDE DIVISION OF 128 BIT DIVIDEND - PERFORMANCE-BASED -------
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
* pre-conditions: hi/rdx < div
*/
_x86w64_wdiv128:
    /* Early Returns  */
    test r8, r8
    jz .zero_div // if (!div)
    // if (!lo && !hi)
    mov r9, rcx
    or  r9, rdx
    jz .zero_dend
    // if (!hi)
    test rdx, rdx
    jz .zero_hi
    // if (hi >= div)
    cmp rdx, r8
    jae .overflow

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

/* Early Return Blocks */
.zero_dend:
    mov rdx, 0
    mov [r9], rdx // *rhat = 0
    mov rax, 0 // return 0
    ; if (overflow != NULL) *overflow = 1
    mov r10, [rsp + 40]
    test r10, r10
    .jz .done
    mov byte [r10], 0
    ret
.zero_hi:
    mov rax, rcx
    xor rdx, rdx
    div r8 // lo / div
    ; rax = quot | rdx = rem
    mov [r9], rdx // *rhat = lo % div
    // return is already in RAX
    ; if (overflow != NULL) *overflow = 1
    mov r10, [rsp + 40]
    test r10, r10
    .jz .done_overflow
    mov byte [r10], 0
    ret
.done:
    ret

/* Overflow hanndling blocks */
.zero_div:
    mov rdx, 0
    mov [r9], rdx // *rhat = 0
    mov rax, -1 // return UINT64_MAX
    ; if (overflow != NULL) *overflow = 1
    mov r10, [rsp + 40]
    test r10, r10
    .jz .done_overflow
    mov byte [r10], 1
    ret
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



/* ------- 64 BIT WIDE DIVISION OF 128 BIT DIVIDEND - CRYPTOGRAPHY-BASED -------
* uint64_t _x86w64_crt_wdiv128(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rem, uint8_t *ovf)
* arg:
*   rcx:        uint64_t lo ----> (64 BIT)
*   rdx:        uint64_t hi ----> (64 BIT)
*   r8:         uint64_t div ---> (64 BIT)
*   r9:         uint64_t *rem --> (64 BIT)
*   [RSP + 40]: uint8_t *ovf --> (1 | 0)
* ret:
*   rax - quot: ([lo:hi] / div)
*   r9  - rhat: ([lo:hi] % div)
* pre-conditions: hi/rdx < div
*/
_x86w64_crt_wdiv128:
    ; =========== 1. Determine Overflow Flag & Write to Memory ===========
    test r8, r8 ; div == 0?
    setz r10b ; r10b = (div == 0) ? 1 : 0
    cmp rdx, r8 ; Compare hi and div
    setae r11b ; r10b = (hi >= div) ? 1 : 0
    or r10b, r11b ; overflow_flag = (hi >= div) | (div == 0)
    mov r11, [rsp + 40] ; r11 = Address value at [RSP + 40]
    mov byte ptr [r11], r10b ; *overflowed = overflow_flag
    

    ; =========== 2. Multiplexing of Fake vs Real Values ===========
    mov rbx, 1 ; fake_hi = 1
    mov rax, 0x1928356720912386 ; fake_lo = 0x1928356720912386 
    test r10b, r10b
    cmovnz rcx, rax ; chosen_lo = overflow_flag ? fake_lo : lo
    cmovnz rdx, rbx ; chosen_hi = overflow_flag ? fake_hi : hi
    ; Loop Configurations
    xor rax, rax ; q = 0
    xor r11, r11 ; Loop counter (i)
    /* Variables-Registers List:
        - rcx: chosen_lo
        - rdx: chosen_hi
        - rax: q - RETURN QUOTIENT
        - r10: OVERFLOW FLAG
        - r11d: i - LOOP COUNTER
    */
.crt_div_loop:
    ; Early high bit overflow check
    mov rbx, rdx ; rbx = rdx (chosen_hi)
    shr rbx, 63 ; rbx = rdx >> 63 (Isolating the MSB)
    and rbx, 1 ; rbx = (rdx >> 63) & 1 (Explicitly isolating the MSB)

    ; Normal, 128-bit dividend shifting routine
    shl rax, 1 ; q <<= 1
    ; test_rem = (chosen_hi << 1) | ((chosen_lo >> i) & 1)
    ; In this implementation, we operate directly on rsi (chosen_hi)
    mov rdi, rcx ; rdi = chosen_lo (rcx)
    shr rdi, 63 ; rdi = chosen_lo >> 63
    and rdi, 1 ; rdi = (chosen_lo >> 63) & 1
    shl rcx, 1 ; chosen_lo (rcx) <<= 1
    add rdx, rdx ; chosen_hi (rdx) <<= 1
    or rdx, rdi ; chosen_hi (rdx) |= extracted_bit (rdi)


    /* --- Quotient & Remainder value manipulation --- */
    xor rsi, rsi ; Clean the RSI register
    cmp rdx, r8 ; (chosen_hi >= div)?
    setae sil ; sil = (chosen_hi >= div)?
    or rsi, rbx ; rsi |= rbx (chosen_hi >= div | hi_overflow)
    neg rsi ; mask = -(rsi_cond) --> -1 (UINT64_MAX) : 0
    /* Quotient Calculations */
    mov rdi, rsi
    and rdi, 1 ; 1 & mask
    or rax, rdi ; q |= (1 & mask)
    /* Remainder Calculations */
    mov rdi, r8
    and rdi, rsi ; div & mask
    sub rdx, rdi ; chosen_hi -= (div & mask)
    ; Incrementing loop counter
    inc r11d
    cmp r11d, 64
    jl .crt_div_loop


    ; =========== 5. Final Output Selections ===========
    test r10b, r10b ; Check overflow flag again
    ; ----- QUOTIENT RETURNS -----
    mov r10, -1 ; r10 = (uint64_t)-1 = UINT64_MAX (0xFFFFFFFFFFFFFFFF)
    cmovnz rax, r10 ; rax ? (overflow_flag/r10b) ? UINT64_MAX : quot
    ; ----- REMAINDER RETURNS -----
    mov rcx, rdx ; rcx = chosen_hi (rdx)
    mov rsi, 0 ; rsi = 0
    cmovnz rcx, rsi ; rcx ? (overflow_flag/r10b) ? 0 (rsi) : chosen_hi (rcx)
    mov qword ptr [r9], rcx ; *rhat (*r9) = rcx (selected remainder)


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



;endif
;endif
