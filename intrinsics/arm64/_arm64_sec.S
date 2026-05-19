#include "_arm64_conn.h"
#if __ARCH_ARM64__


asm_text_section
.globl      ASM_SYM(arm64_hw_drbg)
.globl      ASM_SYM(arm64_hw_trng)

FUNC_TYPE(ASM_SYM(arm64_hw_drbg))
FUNC_TYPE(ASM_SYM(arm64_hw_trng))

FUNC_SIZE(ASM_SYM(arm64_hw_drbg))
FUNC_SIZE(ASM_SYM(arm64_hw_trng))

.p2align    2


// Uses hardware DRBG, in which it alos utilizes hardware entropy, 
// to seed entropy to the user swiftly
/* ----- HARDWRE DETERMINISTIC RANDOM BIT GENERATOR -----
* uint64_t _arm64_hw_drbg(int *err)
* arg:
*   x1: int *err (16 bit)
* ret:
*   x0: Random Number
*   x1: 1 (error) / 0 (success)
*/
ASM_SYM(arm64_hw_drbg):
    mrs x0, s3_3_c2_c4_0 // Reading and returning the RNDR system register
    // Checking if RNDR threw an error (blockage)
    // NZCV is set to 0b0100 on failure, which corresponds to:
    // N = 0, Z = 1, C = 0, V = 0
    cset    x3, eq // Cflag = 1 --> x3 = 1 (error)
    str x3, [x1] // Store x3 into the adress of x1 (*x1 = x3)
    ret



// Harvests raw, unmanipulated True Entropy from hardware entropy pool
/* ----- HARDWRE TRUE RANDOM NUMBER GENERATOR -----
* uint64_t _arm64_hw_trng(int *err)
* arg:
*   x1: int *err (16 bit)
* ret:
*   x0: Random Number
*   x1: 1 (error) / 0 (success)
*/
ASM_SYM(arm64_hw_trng):
    mrs x0, s3_3_c2_c4_1 // Reading and returning the RNDRRS system register
    // Checking if RNDRRS threw an error (blockage)
    // NZCV is set to 0b0100 on failure, which corresponds to:
    // N = 0, Z = 1, C = 0, V = 0
    cset    x3, eq // Cflag = 1 --> x3 = 1 (error)
    str x3, [x1] // Store x3 into the adress of x1 (*x1 = x3)
    ret





#endif