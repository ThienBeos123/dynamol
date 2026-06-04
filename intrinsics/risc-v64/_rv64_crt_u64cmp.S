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
/* --- U64 COMPARISONS --- */
.globl      _rv64_crt_lt
.globl      _rv64_crt_gt
.globl      _rv64_crt_leq
.globl      _rv64_crt_geq

.type       _rv64_crt_lt, @function
.type       _rv64_crt_gt, @function
.type       _rv64_crt_leq, @function
.type       _rv64_crt_geq, @function
.p2align    2



/* ------- UNSIGNED 64 BIT LESS THAN -------
* uint8_t _rv64_crt_lt(uint64_t a, uint64_t b)
* arg:
*   a0: uint64_t a       (64 BIT)
*   a1: uint64_t b       (64 BIT)
* ret: 1 (true) | 0 (false)
*/
_rv64_crt_lt:
    sltu a0, a0, a1
    ret


/* ------- UNSIGNED 64 BIT GREATER THAN -------
* uint8_t _rv64_crt_gt(uint64_t a, uint64_t b)
* arg:
*   a0: uint64_t a       (64 BIT)
*   a1: uint64_t b       (64 BIT)
* ret: 1 (true) | 0 (false)
*/
_rv64_crt_gt:
    sltu a0, a1, a0
    ret



/* ------- UNSIGNED 64 BIT LESS THAN OR EQUAL -------
* uint8_t _rv64_crt_leq(uint64_t a, uint64_t b)
* arg:
*   a0: uint64_t a       (64 BIT)
*   a1: uint64_t b       (64 BIT)
* ret: 1 (true) | 0 (false)
*/
_rv64_crt_leq:
    sltu t0, a1, a0 // a <= b is the logical NOT of a > b (b < a)
    xori a0, t0, 1 // Invert it (logical NOT)
    ld t0, 0 // Register clearance
    ld a1, 0 // Register clearance
    ret


/* ------- UNSIGNED 64 BIT LESS THAN OR EQUAL -------
* uint8_t _rv64_crt_geq(uint64_t a, uint64_t b)
* arg:
*   a0: uint64_t a       (64 BIT)
*   a1: uint64_t b       (64 BIT)
* ret: 1 (true) | 0 (false)
*/
_rv64_crt_geq:
    sltu t0, a0, a1 // a >= b is the logical NOT of a < b
    xori a0, t0, 1 // Invert it (logical NOT)
    ld t0, 0 // Register clearance
    ld a1, 0 // Register clearance
    ret




#endif