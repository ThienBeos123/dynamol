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



#include "__rv64_conn__.h"
#if __ARCH_RVI64__


asm_text_section
.option     nopic
.attribute  arch, "rv64gc"
.attribute  unaligned_access, 0
.attribute  stack_align, 16
.align      1
/* --- EQUALITY AND GENERAL --- */
.globl      _rv64_crt_ispos
.globl      _rv64_crt_isneg
.globl      _rv64_crt_eq
.globl      _rv64_crt_neq
.globl      _rv64_crt_select

.type       _rv64_crt_ispos, @function
.type       _rv64_crt_isneg, @function
.type       _rv64_crt_eq, @function
.type       _rv64_crt_neq, @function
.type       _rv64_crt_select, @function
.p2align    2



/* ------- SIGNED 64 BIT POSITIVITY CHECK -------
* uint8_t _rv64_crt_ispos(int64_t a)
* arg: a0: int64_t a    (64 BIT)
* ret: a0: 1 (true) | 0 (false)
*/
_rv64_crt_ispos:
    srli a0, a0, 63
    xori a0, a0, 1
    snez t0, a0
    and a0, t0
    li t0, 0 // Register clearance
    ret


/* ------- UNSIGNED 64 BIT NEGATIVITY CHECK  -------
* uint8_t _rv64_crt_isneg(int64_t b)
* arg: a0: int64_t a    (64 BIT)
* ret: a0: 1 (true) | 0 (false)
*/
_rv64_crt_isneg:
    srli a0, a0, 63
    ret



/* ------- 64 BIT EQUALITY -------
* uint8_t _rv64_crt_eq(uint64_t a, uint64_t b)
* arg:
*   a0: uint64_t a       (64 BIT)
*   a1: uint64_t b       (64 BIT)
* ret: 1 (true) | 0 (false)
*/
_rv64_crt_eq:
    xor t0, a0, a1
    sseqz a0, t0
    ld t0, 0 // Register clearance
    ld a1, 0 // Register clearance
    ret


/* ------- 64 BIT INEQUALITY -------
* uint8_t _rv64_crt_neq(uint64_t a, uint64_t b)
* arg:
*   a0: uint64_t a       (64 BIT)
*   a1: uint64_t b       (64 BIT)
* ret: 1 (true) | 0 (false)
*/
_rv64_crt_neq:
    xor t0, a0, a1
    snez a0, t0
    ld t0, 0 // Register clearance
    ld a1, 0 // Register clearance
    ret


/* ------- 64 BIT SELECTION -------
* uint8_t _rv64_crt_select(uint8_t cond, uint64_t a, uint64_t b)
* arg:
*   a0: uint8_t cond     (8 BIT -- 1 | 0)
*   a1: uint64_t a       (64 BIT)
*   a2: uint64_t b       (64 BIT)
* ret: EITHER a1 OR a2
*/
_rv64_crt_select:
    snez a0, a0 // !!(a0)
    neg t0, a0 // Masking (1 --> 0xFFFF..., 0 --> 0)
    and t1, a1, t0 // First choice (a & mask)
    not t2, t0 // Inverting the mask (~mask)
    and t3, a2, t2 // Second choice (b & ~mask)
    or a0, t1, t3 // Return t1 | t3 ((a & mask) | (b & ~mask))
    /* ----- Register clearance ----- */
    ld a1, 0 // Clearing out general-purpose/parameter register a1
    ld a2, 0 // Clearing out general-purpose/parameter register a2
    ld t0, 0 // Clearing out general-purpose/parameter register t0
    ld t1, 0 // Clearing out general-purpose/parameter register t1
    ld t2, 0 // Clearing out general-purpose/parameter register t2
    ld t3, 0 // Clearing out general-purpose/parameter register t3
    ret


#endif
