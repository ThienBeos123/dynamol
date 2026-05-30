#include "__arm64_conn__.h"
#if __ARCH_ARM64__



asm_text_section
/* --- I64 COMPARISONS --- */
.globl     ASM_SYM(arm64_crt_lti64)
.globl     ASM_SYM(arm64_crt_gti64)
.globl     ASM_SYM(arm64_crt_leqi64)
.globl     ASM_SYM(arm64_crt_geqi64)

FUNC_TYPE(ASM_SYM(arm64_crt_lti64))
FUNC_TYPE(ASM_SYM(arm64_crt_gti64))
FUNC_TYPE(ASM_SYM(arm64_crt_leqi64))
FUNC_TYPE(ASM_SYM(arm64_crt_geqi64))

FUNC_SIZE(ASM_SYM(arm64_crt_lti64))
FUNC_SIZE(ASM_SYM(arm64_crt_gti64))
FUNC_SIZE(ASM_SYM(arm64_crt_leqi64))
FUNC_SIZE(ASM_SYM(arm64_crt_geqi64))
.p2align    2



/* ------- SIGNED 64 BIT LESS THAN -------
* uint8_t _arm64_crt_lti64(int64_t a, int64_t b)
* arg:
*   x0: int64_t a       (64 BIT)
*   x1: int64_t b       (64 BIT)
* ret: 1 (true) | 0 (false)
*/
ASM_SYM(arm64_crt_lti64):
    cmp x0, x1
    cset x0, lt
    mov x1, #0 // Register clearance
    ret


/* ------- SIGNED 64 BIT GREATER THAN -------
* uint8_t _arm64_crt_gti64(int64_t a, int64_t b)
* arg:
*   x0: int64_t a       (64 BIT)
*   x1: int64_t b       (64 BIT)
* ret: 1 (true) | 0 (false)
*/
ASM_SYM(arm64_crt_gti64):
    cmp x0, x1
    cset x0, gt
    mov x1, #0 // Register clearance
    ret



/* ------- SIGNED 64 BIT LESS THAN OR EQUAL -------
* uint8_t _arm64_crt_leqi64(int64_t a, int64_t b)
* arg:
*   x0: int64_t a       (64 BIT)
*   x1: int64_t b       (64 BIT)
* ret: 1 (true) | 0 (false)
*/
ASM_SYM(arm64_crt_leqi64):
    cmp x0, x1
    cset x0, le
    mov x1, #0 // Register clearance
    ret


/* ------- SIGNED 64 BIT LESS THAN OR EQUAL -------
* uint8_t _arm64_crt_geqi64(int64_t a, int64_t b)
* arg:
*   x0: int64_t a       (64 BIT)
*   x1: int64_t b       (64 BIT)
* ret: 1 (true) | 0 (false)
*/
ASM_SYM(arm64_crt_geqi64):
    cmp x0, x1
    cset x0, ge
    mov x1, #0 // Register clearance
    ret



#endif