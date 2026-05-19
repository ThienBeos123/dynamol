#include "_arm64_conn.h"
#if __ARCH_ARM64__

asm_text_section
.globl     ASM_SYM(arm64_add64c)
.globl     ASM_SYM(arm64_sub64b)
.globl     ASM_SYM(arm64_wmul128)

FUNC_TYPE(ASM_SYM(arm64_add64c))
FUNC_TYPE(ASM_SYM(arm64_sub64b))
FUNC_TYPE(ASM_SYM(arm64_wmul128))

FUNC_SIZE(ASM_SYM(arm64_add64c))
FUNC_SIZE(ASM_SYM(arm64_sub64b))
FUNC_SIZE(ASM_SYM(arm64_wmul128))

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
*   x2 - hi: (a * b) >> 64;
*/
ASM_SYM(arm64_wmul128):
    mov x3, x0 // Move the value of x0 (a) into x3 as a temporary
    mul x0, x3, x1 // x0 = (x3 * x1) mod(2^64); (GET LOW 64 BIT)
    umulh   x4, x3, x1      // x4 = (__int128)(x0/x3 * x1) >> 64; (GET HIGH 64 BIT)
    str x4, [x2] // *x2 = x4; ----> Mutate *hi with the value in x4
    ret

#endif