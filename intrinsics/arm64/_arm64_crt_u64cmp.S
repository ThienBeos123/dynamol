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