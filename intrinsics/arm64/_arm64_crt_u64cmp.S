#include "__arm64_conn__.h"
#if __ARCH_ARM64__




asm_text_section
/* --- U64 COMPARISONS --- */
.globl     ASM_SYM(arm64_crt_lt)
.globl     ASM_SYM(arm64_crt_gt)
.globl     ASM_SYM(arm64_crt_leq)
.globl     ASM_SYM(arm64_crt_geq)

FUNC_TYPE(ASM_SYM(arm64_crt_lt))
FUNC_TYPE(ASM_SYM(arm64_crt_gt))
FUNC_TYPE(ASM_SYM(arm64_crt_leq))
FUNC_TYPE(ASM_SYM(arm64_crt_geq))

FUNC_SIZE(ASM_SYM(arm64_crt_lt))
FUNC_SIZE(ASM_SYM(arm64_crt_gt))
FUNC_SIZE(ASM_SYM(arm64_crt_leq))
FUNC_SIZE(ASM_SYM(arm64_crt_geq))
.p2align    2



/* ------- UNSIGNED 64 BIT LESS THAN -------
* uint8_t _arm64_crt_lt(uint64_t a, uint64_t b)
* arg:
*   x0: uint64_t a       (64 BIT)
*   x1: uint64_t b       (64 BIT)
* ret: 1 (true) | 0 (false)
*/
ASM_SYM(arm64_crt_lt):
    cmp x0, x1
    cset x0, lo
    mov x1, #0 // Register clearance
    ret


/* ------- UNSIGNED 64 BIT GREATER THAN -------
* uint8_t _arm64_crt_gt(uint64_t a, uint64_t b)
* arg:
*   x0: uint64_t a       (64 BIT)
*   x1: uint64_t b       (64 BIT)
* ret: 1 (true) | 0 (false)
*/
ASM_SYM(arm64_crt_gt):
    cmp x0, x1
    cset x0, hi
    mov x1, #0 // Register clearance
    ret



/* ------- UNSIGNED 64 BIT LESS THAN OR EQUAL -------
* uint8_t _arm64_crt_leq(uint64_t a, uint64_t b)
* arg:
*   x0: uint64_t a       (64 BIT)
*   x1: uint64_t b       (64 BIT)
* ret: 1 (true) | 0 (false)
*/
ASM_SYM(arm64_crt_leq):
    cmp x0, x1
    cset x0, ls
    mov x1, #0 // Register clearance
    ret


/* ------- UNSIGNED 64 BIT LESS THAN OR EQUAL -------
* uint8_t _arm64_crt_geq(uint64_t a, uint64_t b)
* arg:
*   x0: uint64_t a       (64 BIT)
*   x1: uint64_t b       (64 BIT)
* ret: 1 (true) | 0 (false)
*/
ASM_SYM(arm64_crt_geq):
    cmp x0, x1
    cset x0, hs
    mov x1, #0 // Register clearance
    ret






#endif