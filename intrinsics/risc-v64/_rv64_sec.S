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
    # Load the entropy from the seed register (0x015)
    # into a0 with no changes (hence x0 as the third arg)
    csrrw a0, 0x015, x0
    li t0, 0 # Load the immediate value 0 into t0
    sw t0, 0(a1) # *a1 = t0; (C-version)
    ret




#endif