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

.type       _rv64_add64c, @function
.type       _rv64_sub64b, @function
.type       _rv64_wmul128, @function


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


#endif