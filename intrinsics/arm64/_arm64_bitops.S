/*
Copyright (C) 2026 @ThienBeos123/@Poly-glon

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

  http://apache.org

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/



#include "__arm64_conn__.h"
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