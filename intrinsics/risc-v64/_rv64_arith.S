#include "__rv64_conn__.h"
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
.globl      _rv64_crt_wdiv128

.type       _rv64_add64c, @function
.type       _rv64_sub64b, @function
.type       _rv64_wmul128, @function
.type       _rv64_wdiv128, @function
.type       _rv64_crt_wdiv128, @function


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
    ld t0, 0(a2) // Load *carry into t0
    add a3, a0, a1 // sum = a + b
    sltu t1, a3, a0 // ab_carry = (sum < a);
    
    add a0, a3, t0 // final = sum + *carry
    sltu t0, a0, a3 // final_carry = (final < sum)

    // Returning and mutating
    or t0, t0, t1 // t0 (final_carry) |= t1 (ab_carry)
    sd t0, 0(a2) // *a2 = t0
    // The final sum is already in a0 (RETURNED)
    ld a1, x0 // Register clearance
    ld a3, x0 // Register clearance
    ld t0, x0 // Register clearance
    ld t1, x0 // Register clearance
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
    ld t0, 0(a2) // Load *carry into t0
    sub a3, a0, a1 // diff = a - b
    sltu t1, a3, a0 // ab_borrow = (diff < a);
    xori t1, t1, 1 // invert ab_borrow
    // ab_borrow = (diff > a);

    sub a0, a3, t0 // final = diff - *borrow
    sltu t0, a0, a3 // final_borrow = (final < diff)
    xori t0, t0, 1 // invert final_borrow
    // final_borrow = (final > diff);

    // Returning and mutating
    or t0, t0, t1 // t0 (final_borrow) |= t1 (ab_borrow)
    sd t0, 0(a2) // *a2 = t0
    // The final difference is already in a0 (RETURNED)
    ld a1, x0 // Register clearance
    ld a3, x0 // Register clearance
    ld t0, x0 // Register clearance
    ld t1, x0 // Register clearance
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
    mv t0, a0 // t0 = a0/a
    mul a0, t0, a1 // a0 = (a * b) mod(2^64)
    mulhu   t1, t0, a1 // t1 = (__int128)(a * b) >> 64
    sd t1, 0(a2) // *a2 = t1
    // The return value - low bits is already in a0
    ld a1, x0 // Register clearance
    ld t0, x0 // Register clearance
    ld t1, x0 // Register clearance
    ret


/* ------- 64 BIT WIDE DIVISION OF 128 BIT DIVIDEND - PERFORMANCE-BASED -------
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
    ld t0, 64 // Loop Counter - i
    ld t1, x0 // Quotient - q
.div_loop:
    slli t2, a1, 1              // t2 = a1 << 1  (hi << 1)
    srli t3, a0, 63             // t3 = a0 >> t0 (lo >> 63) MSB extraction
    or  a1, t2, t3              // a1 (hi) becomes the updated tesst_rem
    slli a0, a0, 1              // a0 = a0 << 1 (shift 'lo' left)

    // 2. Shift our accumulated quotient left by 1 to make room for the next bit
    slli t1, t1, 1 // Shift left ig idk what to say else more
    bltu a1, a2, .skip_sub // Compare test/rem/hi (a1) with divisor (a2)
    // If test_rem/hi < div, current quotient bit is 0 --> skip subtraction

    // 3. If remainder >= divisor: Subtract divisior + set quotient bit to 1
    sub a1, a1, a2 // a1 -= a2 (hi -= div)
    ori t1, t1, 1 // Set lowest bit of quotient to 1
    addi t0, t0, -1 // Decrement loop counter
    bnez t0, .div_loop // Repeat until all 64 bits are processed
    ret
.skip_sub:
    addi t0, t0, -1 // Decrement loop counter
    bnez t0, .div_loop // Repeat until all 64 bits are processed
    // ---- Normal Return Path ----
    sd a1, 0(a3) // *rhat = hi
    mv a0, t1 // a0 = q (return q)
    beq a4, x0, .done // if (overflowed != NULL) *overflowed = 0
    sb x0, 0(a4) // *overflowed = 0
    ret
.done:
    ret

/* Overflow handling blocks */
.overflow:
    // *rhat = hi % div
    remu t0, a1, a2
    sd t0, 0(a3) // *rhat = t0
    ld a0, -1 // return -1/UINT64_MAX;
    // if (overflow != NULL) *overflow = 1
    beq a4, x0, .done_overflow
    ld t0, 1 // Load the value of 1 into t0
    sb t0, 0(a4) // *overflowed = 1
    ret
.done_overflow:
    ret




/* ------- 64 BIT WIDE DIVISION OF 128 BIT DIVIDEND - CRYPTOGRAPHY-BASED -------
* uint64_t _rv64_crt_wdiv128(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rhat, uint8_t *ovf);
* arg:
*   a0: uint64_t lo ----> (64 BIT)
*   a1: uint64_t hi ----> (64 BIT)
*   a2: uint64_t div ---> (64 BIT)
*   a3: uint64_t *rem --> (64 BIT)
*   a4: uint8_t *ovf ---> (1 | 0 -- 64 BIT ADDRESS)
* ret:
*   a0 - quot: ([lo:hi] / div)
*   a3 - rhat: ([lo:hi] % div)
* pre-conditions: hi/rsi < div
*/
_rv64_crt_wdiv128:
    // =========== 1. Determine Overflow Flag & Write to Memory ===========
    sltu t0, a1, a2 // Set to t0 if a1 < a2
    xori t0, t0, 1 // Flip t0 to a1 >= a2
    sb t0, 0(a4) // *ovf = overflow_flag
    // --- 2. Setup Saturated / Fake Input Options ---
    ld a5, 1
    li a6, 0x1928356720912386

    // =========== 3. Choosing Real vs. Fake Values (Multiplexing) ===========
    // We turn our 0/1 overflow_flag (t0) into a broad bitmask (t1)
    neg t1, t0 // If t0 == 1 -> t1 = 0xFFFFFFFFFFFFFFFF (BITMASK)
    not t2, t1 // t2 = ~mask (Inv-mask)
    // chosen_lo (t3) = (fake_lo & mask) | (lo & ~mask)
    and t3, a6, t1
    and t4, a0, t2
    or  t3, t3, t4
    // chosen_hi (t4) = (fake_hi & mask) | (hi & ~mask)
    and t5, a5, t1
    and t6, a1, t2
    or  t4, t5, t6
    /* Variables-Registers List:
        - t0 -- OVERFLOWED_FLAG
        - t3 -- chosen_lo               - a6 -- fake_lo: 0x1928356720912386
        - t4 -- chosen_hi               - a5 -- fake_hi: 1
        - t5 -- LOOP COUNTER (i)        - a0 - q (RESULT QUOTIENT)
    * UNMODIFIABLE VARIALBES/REGISTERS LIST:
        - a3 -- *rem    |   - a4 -- *ovf
    */ 
    // Loop Configurations
    li a0, x0 // a0 = q (quotient accumulator)
    li t5, 64 // a5 = Loop counter (i = 64 down to 1)

.crt_div_loop:
    slli a0, a0, 1 // q <<= 1
    // test_rem = (chosen_hi << 1) | ((chosen_lo >> i) & 1)
    // In this implementation, we operate directly onto x9 (chosen_hi)
    slli t4, t4, 1 // chosen_hi <<= 1
    srli t1, t3, 63 // Extract bit 63 from chosen_lo
    or t4, t4, t1 // t4 (chosen_hi) |= t1 (lo >> 63)
    slli t3, t3, 1 // Shift chosen_lo (<<= 1) left for the next iteration

    /* --- Quotient & Remainder value manipulation --- */
    sltu t6, t4, a2 // t6 = (chosen_hi < div)?
    xori t6, t6, 1 // t6 = (chosen_hi >= div)?
    neg t6, t6 // mask = -(chosen_hi >= div) --> -1 (UINT64_MAX) | 0
    /* -------- Quotient Calculations -------- */
    andi t1, t6, 1 // t1 = 1 & mask
    or a0, a0, t1 // q |= (1 & mask)
    /* ------- Remainder Calculations ------- */
    and t1, a2, t6 // t1 = div & mask
    sub t4, t4, t1 // chosen_hi (hi) -= (div & mask)

    // Decreasing the loop counter
    addi t5, t5, -1 // Decrement loop counter
    bnez t5, .crt_div_loop // Looping back to the start of .crt_div_loop


    /* =========== 5. Final Output Selections =========== */
    // If overflowed (t0 == 1): return UINT64_MAX. Else: return q. 
    // If overflowed (t0 == 1): *rhat = 0. Else: *rhat = final accumulated chosen_hi (x9).
    li t6, -1 // UINT64_MAX
    neg t1, t0 // If t0 == 1 -> t1 = 0xFFFFFFFFFFFFFFFF (BITMASK)
    not t2, t1 // t2 = ~(t1) --> 0xFFFFFFFFFFFFFFFF --> 0x000000000000000
    // ---- Quotient RETURNS ----
    and t6, t6, t1 // Only keep on OVERFLOW (t1 = 0xFFF..)
    and a0, a0, t2 // Only keep on NON-OVERFLOW (t2 = 0xFFF..)
    or a0, a0, t6 // Merge them in RETURN_QUOTIENT
    // ---- Remainder RETURNS ----
    and t4, t4, t2 // Only keep on NON-OVERFLOW (t2 = 0xFFF..)
    sd t4, 0(a3) // Else, t4 (chosen_hi == 0)


    /* =========== 6. Aggressive Post-operation Clearance =========== */
    // General-purpose Registers
    ld a1, x0 // Register Clerance
    ld a2, x0 // Register Clerance
    ld a5, x0 // Register Clerance
    ld a6, x0 // Register Clerance
    // Temporary Registers
    ld t0, x0 // Register Clerance
    ld t1, x0 // Register Clerance
    ld t2, x0 // Register Clerance
    ld t3, x0 // Register Clerance
    ld t4, x0 // Register Clerance
    ld t5, x0 // Register Clerance
    ld t6, x0 // Register Clerance
    ret


#endif
