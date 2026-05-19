#include "_arm64_conn.h"
#if __ARCH_ARM64__


asm_text_section
.globl     ASM_SYM(arm64_clz64)
.globl     ASM_SYM(arm64_ctz64)
.globl     ASM_SYM(arm64_bswap64)
.globl     ASM_SYM(arm64_pcnt64)

FUNC_TYPE(ASM_SYM(arm64_clz64))
FUNC_TYPE(ASM_SYM(arm64_ctz64))
FUNC_TYPE(ASM_SYM(arm64_bswap64))
FUNC_TYPE(ASM_SYM(arm64_pcnt64))

FUNC_SIZE(ASM_SYM(arm64_clz64))
FUNC_SIZE(ASM_SYM(arm64_ctz64))
FUNC_SIZE(ASM_SYM(arm64_bswap64))
FUNC_SIZE(ASM_SYM(arm64_pcnt64))

.p2align      2



/* ------- COUNT LEADING ZERO 64 BIT -------
* uint8_t _arm64_clz64(uint64_t x)
* arg: x0: uint64_t x --> (64 BIT)
* ret: x0: clz(x) ------> (8  BIT)
*/
ASM_SYM(arm64_clz64):
    clz x0, x0
    ret


/* ------- COUNT TRAILING ZERO 64 BIT -------
* uint8_t _arm64_ctz64(uint64_t x)
* arg: x0: uint64_t x --> (64 BIT)
* ret: x0: ctz(x) ------> (8  BIT)
*/
ASM_SYM(arm64_ctz64):
    rbit x0, x0
    clz  x0, x0
    ret


/* ------- BYTE SWAP 64 BIT -------
* uint64_t _arm64_bswap64(uint64_t x)
* arg: x0: uint64_t x ----> (64 BIT)
* ret: x0: bswap(x) ------> (8  BIT)
*/
ASM_SYM(arm64_bswap64):
    rev x0, x0
    ret


/* ------- POPULATION COUNT 64 BIT -------
* uint8_t _arm64_pcnt64(uint64_t x)
* arg: x0: uint64_t x --> (64 BIT)
* ret: x0: pcnt(x) -----> (8  BIT)
*/
ASM_SYM(arm64_pcnt64):
    fmov d0, x0
    cnt v0.8b, v0.8b
    addv b0, v0.8b
    umov w0, v0.b[0]
    ret

#endif