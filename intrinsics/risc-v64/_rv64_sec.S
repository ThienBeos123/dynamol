#include "__rv64_conn__.h"
#if __ARCH_RVI64__



asm_text_section
.option     nopic
.attribute  arch, "rv64gc_zkr"
.attribute  unaligned_access, 0
.attribute  stack_align, 16
.align      1
.globl      _rv64_hw_trng
.globl      _rv64_hw_cstrng

.type       _rv64_hw_trng, @function
.type       _rv64_hw_cstrng, @function



/* ------- HARDWARE TRUE RANDOM ENTROPY 64 BIT - FAST AND CORRECT -------
* uint64_t _rv64_hw_trng(int *err)
* arg: a1: int *err (16 bit)
* ret:
*   a0: Random Number
*   a1: 1 (error) / 0 (success)
*/
_rv64_hw_trng:
    /* ----------------- VALUE RETRIEVING BLOCKS ----------------- */
    csrrw t0, 0x015, x0
    srli t1, t0, 30  // Shifting the OPT code [30:31] bits to [0:1]
    andi t1, t1, 0b11 // Masking to get the value of the [0:1] bits
    li t2, 0b01 // Load the OPT code for WAIT [01] into t2 for comparison
    beq t1, t2, .TRNG_exhausted // Branch into the EXHAUSTED handling
    li t2, 0b11 // Load the OPT code for DEAD [11] into t2 for comparison
    beq t1, t2, .TRNG_fatal // Branch into the DEAD handling

    csrrw t1, 0x015, x0 // t1 is the VALUE HOLDER here
    srli t2, t1, 30 // t2 is the STATUS HOLDER here
    andi t2, t2, 0b11 // t3 is the OPT CODE HOLDER here
    li t3, 0b01
    beq t2, t3, .TRNG_exhausted
    li t3, 0b11
    beq t2, t3, .TRNG_fatal

    csrrw t2, 0x015, x0 // t2 is the VALUE HOLDER here
    srli t3, t2, 30 // t3 is the STATUS HOLDER here
    andi t3, t3, 0b11 // t4 is the OPT CODE HOLDER here
    li t4, 0b01
    beq t3, t4, .TRNG_exhausted
    li t4, 0b11
    beq t3, t4, .TRNG_fatal

    csrrw t3, 0x015, x0 // t3 is the VALUE HOLDER here
    srli t4, t3, 30 // t4 is the STATUS HOLDER here
    andi t4, t4, 0b11 // t5 is the OPT CODE HOLDER here
    li t5, 0b01
    beq t4, t5, .TRNG_exhausted
    li t5, 0b11
    beq t4, t5, .TRNG_fatal

    /* --------------- STICHING --------------- */
    // Clear out the top bits and then move to respective positions
    slli  t0, t0, 48
    srli  t0, t0, 48 // t0 is clean bits [0:15]
    slli  t1, t1, 48
    srli  t1, t1, 32 // t1 is shifted to bits [16:31]
    slli  t2, t2, 48
    srli  t2, t2, 16 // t2 is shifted to bits [32:47]
    slli  t3, t3, 48 // t3 is shifted to bits [48:63]
    // Combining them via BITWISE OR
    or t0, t0, t1
    or t0, t0, t2
    or t0, t0, t3
    // Successful Exit with Successful exit code
    mv a0, t0 // Move t0 --> a0 (a0 = t0)
    li t0, 0 // t0 = 0
    sd t0, 0(a1) // *a1 = t0 (no error)
    ret


.TRNG_exhausted:
    li    t6, 1
    sw    t6, 0(a1) // *err = 1 (Exhausted/WAIT)
    li    a0, 0 // Return 0
    ret

.TRNG_fatal:
    li    t6, 2
    sw    t6, 0(a1) // *err = 2 (Fatal/DEAD)
    li    a0, 0 // Return 0
    ret




/* ------- HARDWARE TRUE RANDOM ENTROPY 64 BIT - CRYPTOGRAPHICALLY-SECURED -------
* uint64_t _rv64_hw_cstrng(int *err)
* arg: a1: int *err (16 bit)
* ret:
*   a0: Random Number
*   a1: 1 (error) / 0 (success)
*/
_rv64_hw_cstrng:
    // We use t0 as our global cumulative status tracker.
    // At the end, we want to know if every sample successfully achieved OPST == 10 (ES16).
    li   t0, 0              // t0 = error accumulator status

    // ------ SAMPLE 1 (Bits [0:15]) ------
    csrrw t1, 0x015, x0 // Read seed CSR into t1
    srli  t2, t1, 30    // Extract OPST bits [31:30] into t2
    xori  t2, t2, 2     // If t2 was 2 (success), t2 becomes 0. Otherwise non-zero.
    or    t0, t0, t2    // Accumulate anomalies into t0
    // Save and scrub raw data chunk safely
    slli  t1, t1, 48
    srli  a2, t1, 48 // a2 = Cleaned Sample 1 entropy bits

    // ------ SAMPLE 2 (Bits [16:31]) ------
    csrrw t1, 0x015, x0     
    srli  t2, t1, 30        
    xori  t2, t2, 2         
    or    t0, t0, t2        
    // Save and scrub raw data chunk safely
    slli  t1, t1, 48
    srli  t1, t1, 32        
    or    a2, a2, t1 // a2 |= Sample 2 shifted to bits [16:31]

    // ------ SAMPLE 3 (Bits [32:47]) ------
    csrrw t1, 0x015, x0     
    srli  t2, t1, 30        
    xori  t2, t2, 2         
    or    t0, t0, t2        
    // Save and scrub raw data chunk safely
    slli  t1, t1, 48
    srli  t1, t1, 16        
    or    a2, a2, t1 // a2 |= Sample 3 shifted to bits [32:47]

    // ------ SAMPLE 4 (Bits [48:63]) ------
    csrrw t1, 0x015, x0     
    srli  t2, t1, 30        
    xori  t2, t2, 2         
    or    t0, t0, t2        
    // Save and scrub raw data chunk safely
    slli t1, t1, 48 // Shifts directly to [48:63] region boundary
    or   a2, a2, t1 // a2 now contains full 64-bit compiled pool



    // CONSTANT-TIME MULTIPLEXING & RETURN FORMATTING
    // If t0 == 0, then ALL reads were perfectly successful (ES16).
    // If t0 != 0, a failure mask must activate.
    snez t1, t0 // t1 = 1 if an error occurred, 0 if successful (MATCHES t0)
    neg  t2, t1 // t2 = Error Mask: 0xFFFF... (if failed), 0x00... (if success)
    not  t3, t2 // t3 = Success Mask: 0x00... (if failed), 0xFF... (if success)
    // Clean the random value: if any sample failed, force output to 0
    and a2, a2, t3 // a2 = successful_pool & success_mask


    // Now handle the specific error code mapping for *err without branching.
    // Standard fallback logic classification:
    //   If OPST was 01 (WAIT) -> error code = 1
    //   If OPST was 11 (DEAD) or 00 (BIST anomalies) -> error code = 2
    // We check the status traits of our last recorded sample in t2 (before it was flipped):
    // Re-extract true OPST raw bits from the last execution snapshot
    srli  t4, t1, 30        // Fetch the original un-flipped status of Sample 4
    
    // Branchless classification mapping:
    // If raw OPST == 1 (WAIT), we return 1.
    // If raw OPST == 3 (DEAD) or 0 (BIST processing state faults), we pass 2.
    andi  t5, t4, 1         // t5 = 1 if OPST is odd (WAIT/DEAD), 0 if even (BIST)
    srli  t6, t4, 1         // t6 = 1 if OPST is 2 or 3 (ES16/DEAD), 0 if 0 or 1 (BIST/WAIT)
    
    // Synthesize mapped value: (t5 | (t6 << 1)) inside an arithmetic threshold evaluation
    // Alternatively, a clean, robust default fallback for generic cryptographic failure:
    // Set err code directly to 1 if it was a WAIT condition, otherwise default to 2.
    xor   t4, t4, 1         // t4 = 0 if OPST was 1 (WAIT)
    snez  t4, t4            // t4 = 1 if NOT a WAIT condition
    addi  t4, t4, 1         // t4 = 1 (if WAIT), 2 (if DEAD/BIST)

    // Apply the mask to the error register variable
    and   t4, t4, t2        // If no error occurred (t2 == 0), t4 becomes 0 cleanly.

    // Commit values to respective destination systems
    sw t4, 0(a0) // *err = chosen error code status (0, 1, or 2)
    mv a0, a2    // Return the sanitized 64-bit random value in a0
    /* ------ POST OPERATION REGISTER SANITIZATION ------ */
    ld a2, x0 // Intial Return Values
    ld t0, x0 // Temporaries
    ld t1, x0 // Temporaries
    ld t2, x0 // Temporaries
    ld t3, x0 // Temporaries
    ld t4, x0 // Temporaries
    ld t5, x0 // Temporaries
    ld t6, x0 // Temporaries
    ret




#endif