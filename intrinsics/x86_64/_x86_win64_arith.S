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
#if __ABI_X64_WIN64__


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
    ; --- 1. Evaluate Overflow State ---
    cmp rdx, r8 ; Compare hi (RDX) with div (R8)
    setae bl ; r9b = 1 if hi >= div, else 0 (overflow flag)

    ; --- 2. Neutralize Hardware Divide Errors ---
    ; If hi >= div, the CPU will fault. We use conditional moves to feed
    ; a safe dummy configuration (hi=0, lo=0, div=1) to the execution engine.
    mov r10, rdx ; r10 = working_hi
    mov r11, rcx ; r11 = working_lo
    mov r12, r8 ; r12 = working_div

    mov rax, 0 ; Dummy lo/hi value
    mov rbx, 1 ; Dummy div value (prevents division by zero)

    cmovae r10, rax ; If overflowed: working_hi = 0
    cmovae r11, rax ; If overflowed: working_lo = 0
    cmovae r12, rbx ; If overflowed: working_div = 1

    ; --- 3. Execute Native Hardware Division ---
    mov rdx, r10 ; RDX = working_hi
    mov rax, r11 ; RAX = working_lo
    div r12 ; [RDX:RAX] / r12
    ; RAX = calculated_quot, RDX = calculated_rem

    ; --- 4. Final Output Formatting (Bitwise Multiplexing) ---
    ; Setup our overflow default constants
    mov rsi, -1 ; rsi = UINT64_MAX
    mov rdi, 0 ; rdi = 0

    ; If an overflow actually happened, override the hardware results
    cmovae rax, rsi ; If overflowed: quotient = UINT64_MAX
    cmovae rdx, rdi ; If overflowed: remainder = 0
    ; Commit remainder to memory (*rem)
    mov [r9], rdx

    ; --- 5. Optional Pointer Safety Check for *overflowed ---
    ; If the user passed NULL (0) for R8, we skip writing to it.
    ; While this utilizes a jump, it is totally safe for cryptography/constant-time
    ; security because pointer presence is an execution environment property,
    ; completely independent of secret mathematical data keys.
    mov r10, [rsp + 40]
    test r10, r10
    jz .crt_done
    mov [r10], bl ; *overflowed = overflow_flag
    /* --------- POST-OPERATION AGGRESSIVE REGISTER CLEARANCE --------- */
    mov rcx, 0 ; Parameters / Arguments
    mov rdx, 0 ; Parameters / Arguments
    mov r8, 0 ; Parameters / Arguments
    mov rbx, 0
    mov r10, 0
    mov r11, 0
    mov r12, 0
    mov rsi, 0
    mov rdi, 0
.crt_done:
    /* --------- POST-OPERATION AGGRESSIVE REGISTER CLEARANCE --------- */
    mov rcx, 0 ; Parameters / Arguments
    mov rdx, 0 ; Parameters / Arguments
    mov r8, 0 ; Parameters / Arguments
    mov rbx, 0
    mov r10, 0
    mov r11, 0
    mov r12, 0
    mov rsi, 0
    mov rdi, 0
    ret



#endif
#endif
