#include "_arm64_conn.h"
#if __ARCH_ARM64__

asm_text_section
.globl     ASM_SYM(arm64_add64c)
.globl     ASM_SYM(arm64_sub64b)
.globl     ASM_SYM(arm64_wmul128)

FUNC_TYPE(ASM_SYM(arm64_add64c))
FUNC_TYPE(ASM_SYM(arm64_sub64b))
FUNC_TYPE(ASM_SYM(arm64_wmul128))
FUNC_TYPE(ASM_SYM(arm64_wdiv128))

FUNC_SIZE(ASM_SYM(arm64_add64c))
FUNC_SIZE(ASM_SYM(arm64_sub64b))
FUNC_SIZE(ASM_SYM(arm64_wmul128))
FUNC_SIZE(ASM_SYM(arm64_wdiv128))

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
    ldr x3, [x2] // Load *carry into register x3
    cmp x3, #1   // Sets the C flag to 1 (NO-BORROW) if carry = 1

    // 1. Add a = a + b + carry
    // -----> x0 = x0 + x1 + C_flag
    // 2. Set carry flag (Automatic by ADCS)
    adcs    x0, x0, x1
    adc x3, xzr, xzr
    str x3, [x2]
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
    ldr x3, [x2] // Load *carry into register x3
    and x3, x3, #1 // Force x3 to be strictly 0 or 1
    // - Sets the C flag inversely relative to x3
    // +) x3 = 1 (BORROW) --> 0 - 1 = -1 (UNDERFLOWS) ---> C_flag = 0
    // +) x3 = 0 (NO BORROW) -> 0 - 0 = 0 (NORMAL) ------> C_flag = 1
    subs xzr, xzr, x3

    // 1. Subtract a = a - b - carry
    // ---------> x0 = x0 - x1 - C_flag
    sbcs    x0, x0, x1
    cset    x3, cc
    str x3, [x2]
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
    ret


/* ------- 64 BIT WIDE DIVISION OF 128 BIT DIVIDEND -------
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
    cmp x1, x2
    b.hs .overflow
    /* Main Division + Returns */
    mov x5, #64 // Loop Counter
    mov x6, xzr // Quotient q
.div_loop:
    // 1. Shift the 128-bit dividend [x1:x0] left by 1 bit.
    //    We extract the top bit of x0 and shift it into the bottom bit of x1.
    lsl x7, x1, #1                  // x7 = x1 << 1  (hi << 1)
    lsr x8, x0, #(U64_BITS - 1)     // x8 = x0 >> x5 (lo >> 63) MSB extraction
    orr x1, x7, x8                  // x1 (hi) becomes the updated test_rem
    lsl x0, x0, #1                  // x0 = x0 << 1 (shift 'lo' left)

    // 2. Shift our accumulated quotient left by 1 to make room for the next bit
    lsl x6, x6, #1
    cmp x1, x2 // 3. Compare test_rem/hi (x1) with divisor (x2)
    b.lo .skip_sub // If test_rem < divisor, current quotient bit is 0

    // 4. If remainder >= divisor: subtract divisor and set quotient bit to 1
    sub x1, x1, x2 // x1 -= divisor
    orr x6, x6, #1 // Set lowest bit of quotient to 1
    subs x5, x5, #1 // Decrement loop counter
    b.ne .div_loop // Repeat until all 64 bits are processed
    ret
.skip_sub:
    subs x5, x5, #1 // Decrement loop counter
    b.ne .div_loop // Repeat until all 64 bits are processed
    // --- Normal Return Path ---
    str x1, [x3] // *rem = hi;
    mov x0, x6 // return q
    cbz x4, .done // if (overflowed != NULL) *overflowed = 0
    strb wzr, [x4] // *overflowed = 0
    ret
.done:
    ret

/* Overflow handling blocks */
.overflow:
    // *rhat = hi % div2
    udiv x8, x1, x2
    msub x9, x8, x2, x1
    str x9, [x3] // *x3 = x9
    movn x0, #0 // return UINT64_MAX
    // if (overflow != NULL) *overflow = 1
    cbz x4, done_overflow
    mov x8, #1 
    str x8, [x4] // *x4 = x8/1
    ret
.done_overflow:
    ret

#endif