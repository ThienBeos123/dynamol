#include "_rv64_conn.h"
#if __ARCH_RVI64__



asm_text_section
.option     nopic
.attribute  arch, "rv64gc_zkr"
.attribute  unaligned_access, 0
.attribute  stack_align, 16
.align      1
.globl      _rv64_hw_trng
.type       _rv64_hw_trng, @function



/* ------- HARDWARE TRUE RANDOM ENTROPY 64 BIT -------
* uint64_t _rv64_hw_trng(int *err)
* arg:
*   a1: int *err (16 bit)
* ret:
*   a0: Random Number
*   a1: 1 (error) / 0 (success)
*/
_rv64_hw_trng:
    /* ----------------- VALUE RETRIEVING BLOCKS ----------------- */
    csrrw t0, 0x015, x0
    srli t1, t0, 30  # Shifting the OPT code [30:31] bits to [0:1]
    andi t1, t1, 0b11 # Masking to get the value of the [0:1] bits
    li t2, 0b01 # Load the OPT code for WAIT [01] into t2 for comparison
    beq t1, t2, .TRNG_exhausted # Branch into the EXHAUSTED handling
    li t2, 0b11 # Load the OPT code for DEAD [11] into t2 for comparison
    beq t1, t2, .TRNG_fatal # Branch into the DEAD handling

    csrrw t1, 0x015, x0 # t1 is the VALUE HOLDER here
    srli t2, t1, 30 # t2 is the STATUS HOLDER here
    andi t2, t2, 0b11 # t3 is the OPT CODE HOLDER here
    li t3, 0b01
    beq t2, t3, .TRNG_exhausted
    li t3, 0b11
    beq t2, t3, .TRNG_fatal

    csrrw t2, 0x015, x0 # t2 is the VALUE HOLDER here
    srli t3, t2, 30 # t3 is the STATUS HOLDER here
    andi t3, t3, 0b11 # t4 is the OPT CODE HOLDER here
    li t4, 0b01
    beq t3, t4, .TRNG_exhausted
    li t4, 0b11
    beq t3, t4, .TRNG_fatal

    csrrw t3, 0x015, x0 # t3 is the VALUE HOLDER here
    srli t4, t3, 30 # t4 is the STATUS HOLDER here
    andi t4, t4, 0b11 # t5 is the OPT CODE HOLDER here
    li t5, 0b01
    beq t4, t5, .TRNG_exhausted
    li t5, 0b11
    beq t4, t5, .TRNG_fatal

    /* --------------- STICHING --------------- */
    # Clear out the top bits and then move to respective positions
    slli  t0, t0, 48
    srli  t0, t0, 48 # t0 is clean bits [0:15]
    slli  t1, t1, 48
    srli  t1, t1, 32 # t1 is shifted to bits [16:31]
    slli  t2, t2, 48
    srli  t2, t2, 16 # t2 is shifted to bits [32:47]
    slli  t3, t3, 48 # t3 is shifted to bits [48:63]
    # Combining them via BITWISE OR
    or t0, t0, t1
    or t0, t0, t2
    or t0, t0, t3
    # Successful Exit with Successful exit code
    mv a0, t0 # Move t0 --> a0 (a0 = t0)
    li t0, 0 # t0 = 0
    sd t0, 0(a1) # *a1 = t0 (no error)
    ret


.TRNG_exhausted:
    li    t6, 1
    sw    t6, 0(a1) # *err = 1 (Exhausted/WAIT)
    li    a0, 0 # Return 0
    ret

.TRNG_fatal:
    li    t6, 2
    sw    t6, 0(a1) # *err = 1 (Fatal/DEAD)
    li    a0, 0 # Return 0
    ret




#endif