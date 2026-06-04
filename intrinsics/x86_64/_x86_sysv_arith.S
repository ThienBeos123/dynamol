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
    ; --- 1. Evaluate Overflow State ---
    cmp rsi, rdx ; Compare hi (RSI) with div (RDX)
    setae r9b ; r9b = 1 if hi >= div, else 0 (overflow flag)

    ; --- 2. Neutralize Hardware Divide Errors ---
    ; If hi >= div, the CPU will fault. We use conditional moves to feed
    ; a safe dummy configuration (hi=0, lo=0, div=1) to the execution engine.
    mov r10, rsi ; r10 = working_hi
    mov r11, rdi ; r11 = working_lo
    mov r12, rdx ; r12 = working_div

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
    mov r13, -1 ; r13 = UINT64_MAX
    mov r14, 0 ; r14 = 0

    ; If an overflow actually happened, override the hardware results
    cmovae rax, r13 ; If overflowed: quotient = UINT64_MAX
    cmovae rdx, r14 ; If overflowed: remainder = 0
    ; Commit remainder to memory (*rem)
    mov [rcx], rdx

    ; --- 5. Optional Pointer Safety Check for *overflowed ---
    ; If the user passed NULL (0) for R8, we skip writing to it.
    ; While this utilizes a jump, it is totally safe for cryptography/constant-time 
    ; security because pointer presence is an execution environment property, 
    ; completely independent of secret mathematical data keys.
    test r8, r8
    jz .crt_done
    mov [r8], r9b ; *overflowed = overflow_flag
    /* --------- POST-OPERATION AGGRESSIVE REGISTER CLEARANCE --------- */ 
    mov rdi, 0 # Function parameter / argument
    mov rsi, 0 # Function parameter / argument
    mov rdx, 0 # Function parameter / argument
    mov rbx, 0
    mov r9, 0
    mov r10, 0
    mov r11, 0
    mov r12, 0
    mov r13, 0
    mov r14, 0
.crt_done:
    /* --------- POST-OPERATION AGGRESSIVE REGISTER CLEARANCE --------- */ 
    mov rdi, 0 # Function parameter / argument
    mov rsi, 0 # Function parameter / argument
    mov rdx, 0 # Function parameter / argument
    mov rbx, 0
    mov r9, 0
    mov r10, 0
    mov r11, 0
    mov r12, 0
    mov r13, 0
    mov r14, 0
    ret




#endif
#endif