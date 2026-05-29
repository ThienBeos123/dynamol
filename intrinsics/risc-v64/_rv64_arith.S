#include "_rv64_conn.h"
#if __ARCH_RVI64__


asm_text_section
.option     nopic
.attribute  arch, "rv64gc"
.attribute  unaligned_access, 0
.attribute  stack_align, 16
.align      1

.globl      _rv64_add64c
.globl      _rv64_sub64b
.globl      _rv64_wmul128
.globl      _rv64_wdiv128

.type       _rv64_add64c, @function
.type       _rv64_sub64b, @function
.type       _rv64_wmul128, @function
.type       _rv64_wdiv128, @function


/* ------- 64 BIT ADDITION WITH CARRY -------
* uint64_t _rv64_add64c(uint64_t a, uint64_t b, uint8_t *carry)
* arg: 
*   a0: uint64_t a       (64 BIT)
*   a1: uint64_t b       (64 BIT)
*   a2: uint8_t *carry   (8  BIT)
* ret: 
*   a0: (a + b + *carry) mod(2^64)
*   a2: boolean - 1/0
*/
_rv64_add64c:
    ld t0, 0(a2) # Load *carry into t0
    add a4, a0, a1 # sum = a + b
    sltu t1, a4, a0 # ab_carry = (sum < a);
    
    add a0, a4, t0 # final = sum + *carry
    sltu t0, a0, a4 # final_carry = (final < sum)

    # Returning and mutating
    or t0, t0, t1 # t0 (final_carry) |= t1 (ab_carry)
    sd t0, 0(a2) # *a2 = t0
    # The final sum is already in a0
    # ---> Returned already
    ret


/* ------- 64 BIT SUBTRACTION WITH BORROW -------
* uint64_t _rv64_sub64b(uint64_t a, uint64_t b, uint8_t *borrow)
* arg: 
*   a0: uint64_t a       (64 BIT)
*   a1: uint64_t b       (64 BIT)
*   a2: uint8_t *borrow  (8  BIT)
* ret: 
*   a0: (a - b - *borrow) mod(2^64)
*   a2: boolean - 1/0
*/
_rv64_sub64b:
    ld t0, 0(a2) # Load *carry into t0
    sub a4, a0, a1 # diff = a - b
    sltu t1, a4, a0 # ab_borrow = (diff < a);
    xori t1, t1, 1 # invert ab_borrow
    # ab_borrow = (diff > a);

    sub a0, a4, t0 # final = diff - *borrow
    sltu t0, a0, a4 # final_borrow = (final < diff)
    xori t0, t0, 1 # invert final_borrow
    # final_borrow = (final > diff);

    # Returning and mutating
    or t0, t0, t1 # t0 (final_borrow) |= t1 (ab_borrow)
    sd t0, 0(a2) # *a2 = t0
    # The final difference is already in a0
    # ---> Returned already
    ret


/* ------- 64 BIT WIDE MULTIPLICATION --> 128 BIT PRODUCT -------
* uint64_t _rv64_wmul128(uint64_t a, uint64_t b, uint64_t *hi) 
* arg:
*   a0: uint64_t a ----> (64 BIT)
*   a1: uint64_t b ----> (64 BIT)
*   a2: uint64_t *hi --> (64 BIT)
* ret:
*   a0 - lo: (a * b) mod(2^64)
*   a2 - hi: (a * b) >> 64;
*/
_rv64_wmul128:
    mv t0, a0 # t0 = a0/a
    mul a0, t0, a1 # a0 = (a * b) mod(2^64)
    mulhu   t1, t0, a1 # t1 = (__int128)(a * b) >> 64
    sd t1, 0(a2) # *a2 = t1
    # The return value - low bits is already in a0
    ret


/* ------- 64 BIT WIDE DIVISION OF 128 BIT DIVIDEND -------
* uint64_t _rv64_wmul128(uint64_t a, uint64_t b, uint64_t *hi) 
* arg:
*   a0: uint64_t lo ----> (64 BIT)
*   a1: uint64_t hi ----> (64 BIT)
*   a2: uint64_t div ---> (64 BIT)
*   a3: uint64_t *rhat -> (64 BIT ADRESS - 64 BIT CONTENTS)
*   a4: uint8_t *overflowed -> (1 | 0)
* ret:
*   a0 - quot: ([lo:hi] / div)
*   a3 - rhat: ([lo:hi] % div)
*/
_rv64_wdiv128:
    bgeu a1, a2, .overflow
    /* Main Divisions + Returns */ 
    ld t0, 64 # Loop Counter - i
    ld t1, 0 # Quotient - q
.div_loop:
    slli t2, a1, 1              # t2 = a1 << 1  (hi << 1)
    srli t3, a0, (U64_BITS - 1) # t3 = a0 >> t0 (lo >> 63) MSB extraction
    or  a1, t2, t3              # a1 (hi) becomes the updated tesst_rem
    slli a0, a0, 1              # a0 = a0 << 1 (shift 'lo' left)

    # 2. Shift our accumulated quotient left by 1 to make room for the next bit
    slli t1, t1, 1 # Shift left ig idk what to say else more
    bltu a1, a2, .skip_sub # Compare test/rem/hi (a1) with divisor (a2)
    # If test_rem/hi < div, current quotient bit is 0 --> skip subtraction

    # 3. If remainder >= divisor: Subtract divisior + set quotient bit to 1
    sub a1, a1, a2 # a1 -= a2 (hi -= div)
    ori t1, t1, 1 # Set lowest bit of quotient to 1
    addi t0, t0, -1 # Decrement loop counter
    bnez t0, .div_loop # Repeat until all 64 bits are processed
    ret
.skip_sub:
    addi t0, t0, -1 # Decrement loop counter
    bnez t0, .div_loop # Repeat until all 64 bits are processed
    # ---- Normal Return Path ----
    sd a1, 0(a3) # *rhat = hi
    mv a0, t1 # a0 = q (return q)
    beq a4, x0, .done # if (overflowed != NULL) *overflowed = 0
    sb x0, 0(a4) # *overflowed = 0
    ret
.done:
    ret

/* Overflow handling blocks */
.overflow:
    # *rhat = hi % div
    remu t0, a1, a2
    sd t0, 0(a3) # *rhat = t0
    ld a0, -1 # return -1/UINT64_MAX;
    # if (overflow != NULL) *overflow = 1
    beq a4, x0, .done_overflow
    ld t0, 1 # Load the value of 1 into t0
    sb t0, 0(a4) # *overflowed = 1
    ret
.done_overflow:
    ret


#endif