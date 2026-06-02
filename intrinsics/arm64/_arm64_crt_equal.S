#include "__arm64_conn__.h"
#if __ARCH_ARM64__


asm_text_section
/* ----- GENERAL EQUALITIES ----- */
.globl     ASM_SYM(arm64_crt_ispos)
.globl     ASM_SYM(arm64_crt_isneg)
.globl     ASM_SYM(arm64_crt_eq)
.globl     ASM_SYM(arm64_crt_neq)
.globl     ASM_SYM(arm64_crt_select)

FUNC_TYPE(ASM_SYM(arm64_crt_ispos))
FUNC_TYPE(ASM_SYM(arm64_crt_isneg))
FUNC_TYPE(ASM_SYM(arm64_crt_eq))
FUNC_TYPE(ASM_SYM(arm64_crt_neq))
FUNC_TYPE(ASM_SYM(arm64_crt_select))

FUNC_SIZE(ASM_SYM(arm64_crt_ispos))
FUNC_SIZE(ASM_SYM(arm64_crt_isneg))
FUNC_SIZE(ASM_SYM(arm64_crt_eq))
FUNC_SIZE(ASM_SYM(arm64_crt_neq))
FUNC_SIZE(ASM_SYM(arm64_crt_select))
.p2align    2



/* ------- SIGNED 64 BIT POSITIVITY CHECK -------
* uint8_t _arm64_crt_ispos(int64_t a)
* arg: x0: int64_t a    (64 BIT)
* ret: x0: 1 (true) | 0 (false)
*/
ASM_SYM(arm64_crt_ispos):
    lsr x0, x0, #63
    eor x0, x0, #1 
    ret


/* ------- UNSIGNED 64 BIT NEGATIVITIY CHECK -------
* uint8_t _arm64_crt_isneg(int64_t a)
* arg: x0: int64_t a    (64 BIT)
* ret: x0: 1 (true) | 0 (false)
*/
ASM_SYM(arm64_crt_isneg):
    lsr x0, x0, #63
    ret



/* ------- 64 BIT EQUALITY -------
* uint8_t _arm64_crt_eq(uint64_t a, uint64_t b)
* arg:
*   x0: uint64_t a       (64 BIT)
*   x1: uint64_t b       (64 BIT)
* ret: 1 (true) | 0 (false)
*/
ASM_SYM(arm64_crt_eq):
    cmp x0, x1
    cset x0, eq
    mov x1, #0 // Register clearance
    ret


/* ------- 64 BIT INEQUALITY -------
* uint8_t _arm64_crt_neq(uint64_t a, uint64_t b)
* arg:
*   x0: uint64_t a       (64 BIT)
*   x1: uint64_t b       (64 BIT)
* ret: 1 (true) | 0 (false)
*/
ASM_SYM(arm64_crt_neq):
    cmp x0, x1
    cset x0, ne
    mov x1, #0 // Register clearance
    ret



/* ------- 64 BIT SELECTION -------
* uint8_t _arm64_crt_select(uint8_t cond, uint64_t a, uint64_t b)
* arg:
*   x0: uint8_t cond     (8 BIT -- 1 | 0)
*   x1: uint64_t a       (64 BIT)
*   x2: uint64_t b       (64 BIT)
* ret: EITHER x1 OR x2
*/
ASM_SYM(arm64_crt_select):
    cmp x0, xzr
    csel x0, x1, x2, ne
    mov x1, #0 // Register clearance
    mov x2, #0 // Register clearance
    ret





#endif