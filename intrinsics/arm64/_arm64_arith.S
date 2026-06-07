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



#include "__arm64_conn__.h"
#if __ARCH_ARM64__

asm_text_section
.globl     ASM_SYM(arm64_add64c)
.globl     ASM_SYM(arm64_sub64b)
.globl     ASM_SYM(arm64_wmul128)
.globl     ASM_SYM(arm64_wdiv128)
.globl     ASM_SYM(arm64_crt_wdiv128)


FUNC_TYPE(ASM_SYM(arm64_add64c))
FUNC_TYPE(ASM_SYM(arm64_sub64b))
FUNC_TYPE(ASM_SYM(arm64_wmul128))
FUNC_TYPE(ASM_SYM(arm64_wdiv128))
FUNC_TYPE(ASM_SYM(arm64_crt_wdiv128))

FUNC_SIZE(ASM_SYM(arm64_add64c))
FUNC_SIZE(ASM_SYM(arm64_sub64b))
FUNC_SIZE(ASM_SYM(arm64_wmul128))
FUNC_SIZE(ASM_SYM(arm64_wdiv128))
FUNC_SIZE(ASM_SYM(arm64_crt_wdiv128))

.p2align      2

/* ------- 64 BIT ADDITION WITH CARRY -------
* uint64_t _arm64_add64c(uint64_t a, uint64_t b, uint8_t *carry)
* arg:
*   x0: uint64_t a       (64 BIT)
*   x1: uint64_t b       (64 BIT)
*   x2: uint8_t *carry   (8  BIT)
* ret:
*   x0: (a + b + *carry) mod(2^64)
*   x2: boolean - 1/0
*/
ASM_SYM(arm64_add64c):
    ldrb w3, [x2] // Load *carry into register x3
    and  w3, w3, #1
    cmp  w3, #1

    // 1. Add a = a + b + carry
    // -----> x0 = x0 + x1 + C_flag
    // 2. Set carry flag (Automatic by ADCS)
    adcs x0, x0, x1
    adc x3, xzr, xzr
    strb w3, [x2]
    mov x1, #0 // Register clearance
    mov w3, #0 // Register clearance
    ret



/* ------- 64 BIT SUBTRACTION WITH BORROW -------
* uint64_t _arm64_sub64b(uint64_t a, uint64_t b, uint8_t *borrow)
* arg:
*   x0: uint64_t a       (64 BIT)
*   x1: uint64_t b       (64 BIT)
*   x2: uint8_t *borrow  (8  BIT)
* ret:
*   x0: (a - b - *borrow) mod(2^64)
*   x2: boolean - 1/0
*/
ASM_SYM(arm64_sub64b):
    ldrb w3, [x2] // Load *carry into register x3
    and w3, w3, #1 // Force x3 to be strictly 0 or 1
    // - Sets the C flag inversely relative to x3
    // +) x3 = 1 (BORROW) --> 0 - 1 = -1 (UNDERFLOWS) ---> C_flag = 0
    // +) x3 = 0 (NO BORROW) -> 0 - 0 = 0 (NORMAL) ------> C_flag = 1
    subs xzr, xzr, x3

    // 1. Subtract a = a - b - carry
    // ---------> x0 = x0 - x1 - C_flag
    sbcs x0, x0, x1
    cset w3, cc
    strb w3, [x2]
    mov x1, #0 // Register clearance
    mov w3, #0 // Register clearance
    ret



/* ------- 64 BIT WIDE MULTIPLICATION --> 128 BIT PRODUCT -------
* uint64_t _arm64_wmul128(uint64_t a, uint64_t b, uint64_t *hi)
* arg:
*   x0: uint64_t a ----> (64 BIT)
*   x1: uint64_t b ----> (64 BIT)
*   x2: uint64_t *hi --> (64 BIT)
* ret:
*   x0 - lo: (a * b) mod(2^64)
*   x2 - hi: (a * b) >> 64//
*/
ASM_SYM(arm64_wmul128):
    mov x3, x0 // Move the value of x0 (a) into x3 as a temporary
    mul x0, x3, x1 // x0 = (x3 * x1) mod(2^64)// (GET LOW 64 BIT)
    umulh   x4, x3, x1      // x4 = (__int128)(x0/x3 * x1) >> 64// (GET HIGH 64 BIT)
    str x4, [x2] // *x2 = x4// ----> Mutate *hi with the value in x4
    mov x1, #0 // Register clearance
    mov x3, #0 // Register clearance
    mov x4, #0 // Register clearance
    ret






/* ------- 64 BIT WIDE DIVISION OF 128 BIT DIVIDEND - PERFORMANCE-BASED -------
* uint64_t _arm64_wdiv128(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rem, uint8_t *overflowed)
* arg:
*   x0: uint64_t lo ----> (64 BIT)
*   x1: uint64_t hi ----> (64 BIT)
*   x2: uint64_t div ---> (64 BIT)
*   x3: uint64_t *rem --> (64 BIT)
*   x4: uint8_t *overflowed -> (1 | 0 -- 64 BIT ADRESS)
* ret:
*   x0 - quot: ([lo:hi] / div)
*   x3 - rhat: ([lo:hi] % div)
* pre-conditions: hi/rsi < div
*/
ASM_SYM(arm64_wdiv128):
    /* Early Returns  */
    cbz x2, .div_zero // div == 0?
    orr x8, x0, x1
    cbz x8, .zero_dend // if (!lo && !hi)
    cbz x1, .zero_hi // if (!hi)
    cmp x1, x2 // hi >= div?
    b.hs .overflow

    /* Main Division + Returns */
    mov w5, #0 // Loop counter
    mov x6, xzr // Quotient q
.div_loop:
    // 1. Shift the 128-bit dividend [x1:x0] left by 1 bit.
    //    We extract the top bit of x0 and shift it into the bottom bit of x1.
    lsl x6, x6, #1 // q <<= 1;
    lsr x8, x0, #63 // x8 = x0 >> 63 (Getting the top bit of)
    and x8, x8, #1 // (lo >> 63) & 1 (extract bit at i)
    lsl x0, x0, #1 // lo <<= 1;
    // High bits overflowing checking
    adds x7, x1, x1 // test_rem = hi + hi --> hi * 2 --> hi << 1
    orr x9, x7, x8 // Updated test_rem

    // 2. Shift our accumulated quotient left by 1 to make room for the next bit
    ccmp x9, x2, #2, cc // 3. Compare test_rem (x1) with divisor (x2) ONLY when the C flag == 0
    // If not, then NZCV gets turn into 0010 --> (All other 0, C flag = 1) --> Preservation
    b.cc .skip_sub // ONLY branch when the C flag is 0 (x9 >= x2 --> C flag = 1)

    // 4. If remainder >= divisor: subtract divisor and set quotient bit to 1
    sub x1, x9, x2 // x1/hi = test_rem - div
    orr x6, x6, #1 // Set lowest bit of quotient to 1
    add w5, w5, #1 // Increment loop counter
    cmp w5, #64 // (i < 64) - Loop bound
    b.lt .div_loop
    // Subtraction path
    str x1, [x3] // *rem = hi//
    mov x0, x6 // return q
    cbz x4, .done // if (overflowed != NULL) *overflowed = 0
    strb wzr, [x4] // *overflowed = 0
    ret
.skip_sub:
    mov x1, x9 // Update hi = test_rem
    add w5, w5, #1 // Increment loop counter
    cmp w5, #64 // (i < 64) - Loop bound
    b.lt .div_loop
    // --- Normal Return Path ---
    str x1, [x3] // *rem = hi//
    mov x0, x6 // return q
    cbz x4, .done // if (overflowed != NULL) *overflowed = 0
    strb wzr, [x4] // *overflowed = 0
    ret

/* Early return blocks */
.zero_dend:
    mov x9, #0
    str x9, [x3] // *rhat = 0
    mov x0, #0 // return UINT64_MAX
    cbz x4, .done
    mov x8, #0
    str x8, [x4] // *ovf = 0
    ret
.zero_hi:
    udiv x9, x0, x2
    msub x8, x9, x2, x0
    str x8, [x3] // *rhat = lo % div
    mov x0, x9 // return lo / div
    cbz x4, .done
    mov x8, #0
    str x8, [x4] // *ovf = 0
    ret
.done:
    ret

/* Overflow handling blocks */
.div_zero:
    mov x9, #0
    str x9, [x3] // *rhat = 0
    movn x0, #0 // return UINT64_MAX
    // if (overflow != NULL) *overflow = 1
    cbz x4, .done_overflow
    mov x8, #1
    str x8, [x4] // *ovf = 1
    ret
.overflow:
    // *rhat = hi % div2
    udiv x8, x1, x2
    msub x9, x8, x2, x1
    str x9, [x3] // *x3 = x9
    movn x0, #0 // return UINT64_MAX
    // if (overflow != NULL) *overflow = 1
    cbz x4, .done_overflow
    mov x8, #1
    str x8, [x4] // *x4 = x8/1
    ret
.done_overflow:
    ret





/* ------- 64 BIT WIDE DIVISION OF 128 BIT DIVIDEND - CRYPTOGRAPHY-BASED -------
* uint64_t _arm64_crt_wdiv128(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rhat, uint8_t *ovf)
* arg:
*   x0: uint64_t lo ----> (64 BIT)
*   x1: uint64_t hi ----> (64 BIT)
*   x2: uint64_t div ---> (64 BIT)
*   x3: uint64_t *rem --> (64 BIT)
*   x4: uint8_t *ovf ---> (1 | 0 -- 64 BIT ADDRESS)
* ret:
*   x0 - quot: ([lo:hi] / div)
*   x3 - rhat: ([lo:hi] % div)
* pre-conditions: hi/rsi < div
*/
ASM_SYM(arm64_crt_wdiv128):
    // =========== 1. Determine Overflow Flag & Write to Memory ===========
    cmp x2, #0 // if (!div)
    cset w6, eq // w6 = (div == 0) ? 1 : 0
    cmp x1, x2 // Compare hi and div
    cset w5, hs // w5 = (hi >= div) ? 1 : 0
    orr  w5, w5, w6 // w5 |= w6
    strb w5, [x4] // *overflowed = overflow_flag
    tst w5, w5 // if w5 == 1 --> INVALID STATE
    // --- 2. Setup Saturated / Fake Input Options ---
    mov x6, #1
    movk x7, #0x2386, lsl #0
    movk x7, #0x2091, lsl #16
    movk x7, #0x3567, lsl #32
    movk x7, #0x1928, lsl #48


    // =========== 3. Choosing Real vs. Fake Values ===========
    // If 'hs' (WILL OVERFLOW) is true, choose fake. Else choose real.
    csel x8, x0, x7, eq // x8 = chosen_lo
    csel x9, x1, x6, eq // x9 = chosen_hi
    /* Variables-Registers List:
        - x6 -- fake_hi: 1          - x7 -- fake_lo: 0x1928356720912386
        - x8 -- chosen_lo           - x9 -- chosen_hi
        - x10 - q (RESULT QUOTIENT)
    * UNMODIFIABLE VARIALBES/REGISTERS LIST:
        - x3 -- *rem
        - x4 -- *ovf
    */
    // Loop Configurations
    mov x10, xzr // q = 0
    mov w11, #0 // Loop counter (i)

.crt_div_loop:
    lsl x10, x10, #1 // q <<= 1
    // test_rem = (chosen_hi << 1) | ((chosen_lo >> i) & 1)
    // In this implementation, we operate directly onto x9 (chosen_hi)
    lsr x12, x8, #63 // Extract MSB from chosen_lo
    and x12, x12, #1 // (lo >> 63) & 1 (Isolate the ith bit)
    lsl x8, x8, #1 // chosen_lo <<= 1
    adds x9, x9, x9 // chosen_hi + chosen_hi == chosen_hi * 2 == chosen_hi << 1
    orr x9, x9, x12 // x9 (chosen_hi) |= x12


    // --- Quotient & Remainder value manipulation ---
    ccmp x9, x2, #2, cc // chosen_hi >= div? ONLY when the C FLAG IS NOT SET
    // If not, then NZCV gets turn into 0010 --> (All other 0, C flag = 1) --> Preservation
    csetm x12, cs // If the C flag is
    // True ---> mask = UINT64_MAX (Put in 1 inside quotient scenario)
    // False --> mask = 0 (Skip bit/subtraction scenario)
    /* ------- Quotient Calculations ------- */
    and x13, x12, #1 // 1 & mask
    orr x10, x10, x13 // q |= (1 & mask)
    /* ------- Remainder Calculations ------- */
    and x14, x2, x12 // div & mask
    sub x9, x9, x14 // chosen_hi (hi) -= (div & mask)

    // Incrementing loop counter
    add w11, w11, #1
    cmp w11, #64 // (i < 64) -- loop bound
    b.lt .crt_div_loop


    /* =========== 5. Final Output Selections =========== */
    // If overflowed (hs): return UINT64_MAX. Else: return q
    // If overflowed (hs): *rhat = 0. Else: *rhat = final accumulated chosen_hi (x9).
    tst w5, w5 // Re-evaluation of INVALID STATE from above
    mov x15, #-1 // Set error return (UINT64_MAX)
    csel x0, x10, x15, eq // return (overflow_flag) ? x10 (q) : x15 (UINT64_MAX);
    csel x15, x9, xzr, eq // *rhat = (overflow_flag) ? x9 (chosen_hi) : 0;
    str x15, [x3]//
    /* =========== 6. Aggressive Post-operation Clearance =========== */
    mov x1, #0 // Register Clearance
    mov x2, #0 // Register Clearance
    mov x5, #0 // Register Clearance
    mov x6, #0 // Register Clearance
    mov x7, #0 // Register Clearance
    mov x8, #0 // Register Clearance
    mov x9, #0 // Register Clearance
    mov x10, #0 // Register Clearance
    mov x11, #0 // Register Clearance
    mov x12, #0  // Register Clearance
    mov x13, #0 // Register Clearance
    mov x14, #0 // Register Clearance
    mov x15, #0 // Register Clearance
    ret


#endif
