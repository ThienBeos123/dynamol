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

asm_rodata_section
.align      3
_debruijin_clz:
    .byte 63, 62, 15, 61, 6,  14, 35, 60, 2,  5,  13, 21, 25, 34, 46, 59, 1
    .byte 8,  4,  27, 10, 12, 20, 41, 18, 24, 30, 33, 39, 45, 51, 58, 0
    .byte 16, 7,  36, 3,  22, 26, 47, 9,  28, 11, 42, 19, 31, 40, 52, 17 
    .byte 37, 23, 48, 29, 43, 32, 53, 38, 49, 44, 54, 50, 55, 56, 57

_debruijin_ctz:
    .byte 0,  1,  48, 2,  57, 49, 28, 3,  61, 58, 50, 42, 38, 29, 17, 4, 62
    .byte 55, 59, 36, 53, 51, 43, 22, 45, 39, 33, 30, 24, 18, 12, 5, 63 
    .byte 47, 56, 27, 60, 41, 37, 16, 54, 35, 52, 21, 44, 32, 23, 11, 46
    .byte 26, 40, 15, 34, 20, 31, 10, 25, 14, 19, 9,  13, 8,  7,  6



asm_text_section
.option     nopic
.attribute  arch, "rv64gc"
.attribute  unaligned_access, 0
.attribute  stack_align, 16
.align      1

.globl      _rv64_clz64p
.globl      _rv64_clz64c
.globl      _rv64_ctz64p
.globl      _rv64_ctz64c
.globl      _rv64_bswap64_port
.globl      _rv64_pcnt64_port

.type       _rv64_clz64p, @function
.type       _rv64_clz64c, @function
.type       _rv64_ctz64p, @function
.type       _rv64_ctz64c, @function
.type       _rv64_bswap64_port, @function
.type       _rv64_pcnt64_port, @function


/* ------- COUNT LEADING ZERO 64 BIT - PERFORMANCE-BASED -------
* uint8_t _rv64_clz64p(uint64_t x)
* arg: a0: uint64_t x --> (64 BIT)
* ret: a0: clz(x) ------> (8  BIT)
*/
_rv64_clz64p:
    beq  a0, zero, .all_zero // Safeguard check
    // Bit distribution sequence
    // x |= x >> (1 -> 2 -> 4 -> 8 -> 16 -> 32)
    srli t0, a0, 1
    or a0, a0, t0
    srli t0, a0, 2
    or a0, a0, t0
    srli t0, a0, 4
    or a0, a0, t0
    srli t0, a0, 8
    or a0, a0, t0
    srli t0, a0, 16
    or a0, a0, t0
    srli t0, a0, 32
    or a0, a0, t0
    // Getting the De-Brujin Table Adress
    // Full formula = (((x >> 1) + 1) * 0x03F79D71B4CB0A89) >> 58
    li t1, 0x03F79D71B4CB0A89 // Magic De-Brujin Constant
    srli a0, a0, 1
    addi a0, a0, 1
    mul  a0, a0, t1
    srli a0, a0, 58
    // Acessing the adress inside the _deburjin_clz table
    la t1, _debruijin_clz // Get the base adress of the table's page
    add t1, t1, a0 // adress = base_add + index
    ld a0, 0(t1) // No need for srli/slli alignments because we store them as individual bytes
    ret
/* ------- COUNT LEADING ZERO 64 BIT - CRYPTOGRAPHICALLY-SECURED -------
* uint8_t _rv64_clz64c(uint64_t x)
* arg: a0: uint64_t x --> (64 BIT)
* ret: a0: clz(x) ------> (8  BIT)
*/
_rv64_clz64c:
    /* REGISTER VARIABLES: t0 --> count, t1 --> is_zero */
    li t0, 0 // Set t0 = 0
    seqz t1, a0 // See if x == 0 (is_zero condition)
    /* Binary Search Iterations */
    // Iteration 1: 32 BITS
    andi t2, a0, 0xFFFFFFFF00000000 // x & 0xFFFFFFFF00000000
    seqz t2, t2 // !(x & 0xFFFFFFFF00000000)
    neg t2, t2 // If t2 == 1 (bit was 0) ---> t2 = UINT64_MAX
    andi t2, t2, 32 // If bit was 0 ---> t2 = 32 | else ---> t2 = 0
    add t0, t0, t2 // count += !(x & 0xFFFFFFFF00000000) ? 32 : 0;
    sll a0, a0, t2 // x <<= !(x & 0xFFFFFFFF00000000) ? 32 : 0
    // Iteration 2: 16 BITS
    andi t2, a0, 0xFFFF000000000000
    seqz t2, t2
    neg t2, t2
    andi t2, t2, 16
    add t0, t0, t2
    sll a0, a0, t2

    // Iteration 3: 8 BITS
    andi t2, a0, 0xFF00000000000000
    seqz t2, t2
    neg t2, t2
    andi t2, t2, 8
    add t0, t0, t2
    sll a0, a0, t2
    // Iteration 4: 4 BITS
    andi t2, a0, 0xF000000000000000
    seqz t2, t2
    neg t2, t2
    andi t2, t2, 4
    add t0, t0, t2
    sll a0, a0, t2

    // Iteration 5: 2 BITS
    andi t2, a0, 0xC000000000000000
    seqz t2, t2
    neg t2, t2
    andi t2, t2, 2
    add t0, t0, t2
    sll a0, a0, t2
    // Iteration 6: 1 BIT --- MSB
    andi t2, a0, 0x8000000000000000
    seqz t2, t2
    add t0, t0, t2

    // Final return Selection:
    // C-version: return _vanillc_crt_select(is_zero, 64, count);
    // Code is basically copy and pasted from "_rv64_crt_select"
    // to maintain leaf-function status + prevent exploitable function call overhead
    neg t1, t1 // Masking (1 --> 0xFFFF..., 0 --> 0)
    andi t1, t1, 64 // First choice (64)
    not t3, t1 // Inverting the mask (~mask)
    and t3, t3, t0 // Second choice (count)
    or a0, t1, t3 // Return t1 | t3 ((64 & mask) | (count & ~mask))
    /* ------- REGISTER CLEARANCE ------- */
    li t0, 0
    li t1, 0
    li t2, 0
    li t3, 0
    ret





/* ------- COUNT TRAILING ZERO 64 BIT - PERFORMANCE-BASED -------
* uint8_t _rv64_ctz64p(uint64_t x)
* arg: a0: uint64_t x --> (64 BIT)
* ret: a0: ctz(x) ------> (8  BIT)
*/
_rv64_ctz64p:
    beq a0, zero, .ctz_zero   // Safeguard check
    // Full Index formula: ((x & -x) * 0x03F79D71B4CB0A89) >> 58
    neg t0, a0 // -x
    and a0, a0, t0 // x & -x
    li t1, 0x03F79D71B4CB0A89
    mul a0, a0, t1
    srli a0, a0, 58
    // Acessing the adress inside the _deburjin_clz table
    la t1, _debruijin_ctz // Getting the base adress of the table's page
    add t1, t1, a0 // Adress = Base_Adress + Index
    ld a0, 0(t1) // Load into a0 (return) immeditely
    ret
/* ------- COUNT TRAILING ZERO 64 BIT - CRYPTOGRAPHICALLY-SECURED -------
* uint8_t _rv64_ctz64c(uint64_t x)
* arg: a0: uint64_t x --> (64 BIT)
* ret: a0: ctz(x) ------> (8  BIT)
*/
_rv64_ctz64c:
    /* REGISTER VARIABLES: t0 --> count, t1 --> is_zero */
    li t0, 0 // Set t0 = 0
    seqz t1, a0 // See if x == 0 (is_zero condition)
    /* Binary Search Iterations */
    // Iteration 1: 32 BITS
    andi t2, a0, 0x00000000FFFFFFFF // x & 0x00000000FFFFFFFF
    seqz t2, t2 // !(x & 0x00000000FFFFFFFF)
    neg t2, t2 // If t2 == 1 (bit was 0) ---> t2 = UINT64_MAX
    andi t2, t2, 32 // If bit was 0 ---> t2 = 32 | else ---> t2 = 0
    add t0, t0, t2 // count += !(x & 0x00000000FFFFFFFF) ? 32 : 0;
    srl a0, a0, t2 // x <<= !(x & 0x00000000FFFFFFFF) ? 32 : 0
    // Iteration 2: 16 BITS
    andi t2, a0, 0x000000000000FFFF
    seqz t2, t2
    neg t2, t2
    andi t2, t2, 16
    add t0, t0, t2
    srl a0, a0, t2

    // Iteration 3: 8 BITS
    andi t2, a0, 0x00000000000000FF
    seqz t2, t2
    neg t2, t2
    andi t2, t2, 8
    add t0, t0, t2
    srl a0, a0, t2
    // Iteration 4: 4 BITS
    andi t2, a0, 0x000000000000000F
    seqz t2, t2
    neg t2, t2
    andi t2, t2, 4
    add t0, t0, t2
    srl a0, a0, t2

    // Iteration 5: 2 BITS
    andi t2, a0, 0x0000000000000003
    seqz t2, t2
    neg t2, t2
    andi t2, t2, 2
    add t0, t0, t2
    srl a0, a0, t2
    // Iteration 6: 1 BIT --- MSB
    andi t2, a0, 0x0000000000000001
    seqz t2, t2
    add t0, t0, t2

    // Final return Selection:
    // C-version: return _vanillc_crt_select(is_zero, 64, count);
    // Code is basically copy and pasted from "_rv64_crt_select"
    // to maintain leaf-function status + prevent exploitable function call overhead
    neg t1, t1 // Masking (1 --> 0xFFFF..., 0 --> 0)
    andi t1, t1, 64 // First choice (64)
    not t3, t1 // Inverting the mask (~mask)
    and t3, t3, t0 // Second choice (count)
    or a0, t1, t3 // Return t1 | t3 ((64 & mask) | (count & ~mask))
    /* ------- REGISTER CLEARANCE ------- */
    li t0, 0
    li t1, 0
    li t2, 0
    li t3, 0
    ret






/* ------- BYTE SWAP 64 BIT -------
* uint8_t _rv64_bswap64_port(uint64_t x)
* arg: a0: uint64_t x ----> (64 BIT)
* ret: a0: bswap(x) ------> (8  BIT)
*/
_rv64_bswap64_port:
    // Step 1: 8-bit swaps
    ld   t2, .mask_b1 // 0x00FF00FF00FF00FF
    srli t0, a0, 8 // t0 = x >> 8
    and  t0, t0, t2 // t0 &= 0x00FF00FF00FF00FF
    and  t1, a0, t2 // t1 = x & 0x00FF00FF00FF00FF
    slli t1, t1, 8 // t1 <<= 8
    orr  a0, t0, t1

    // Step 2: 16-bit swaps
    ld   t3, .mask_b2 // 0x0000FFFF0000FFFF
    srli t0, a0, 16 // t0 = x >> 16
    and  t0, t0, t3 // t0 &= 0x0000FFFF0000FFFF
    and  t1, a0, t3 // t1 = x & 0x0000FFFF0000FFFF
    slli t1, t1, 16 // t1 <<= 16
    orr  a0, t0, t1

    // Step 3: 32-bit swap halves
    srli t0, a0, 32
    slli t1, a0, 32
    orr  a0, t0, t1
    // ----- Aggressive Temporary Registers Clearance -----
    ld t0, x0 // Zero out the temporary register t0
    ld t1, x0 // Zero out the temporary register t1
    ld t2, x0 // Zero out the temporary register t2
    ld t3, x0 // Zero out the temporary register t3
    ret






/* ------- POPULATION COUNT 64 BIT -------
* uint8_t _rv64_pcnt64_port(uint64_t x)
* arg: a0: uint64_t x ----> (64 BIT)
* ret: a0: pcnt(x) -------> (8  BIT)
*/
_rv64_pcnt64_port:
    // Step 1: x -= (x >> 1) & UINT64_C(0x5555555555555555)
    srli t0, a0, 1
    ld   t1, .mask_55
    and  t0, t0, t1
    sub  a0, a0, t0

    // Step 2: x = (x & UINT64_C(0x3333333333333333)) + ((x >> 2) & UINT64_C(0x3333333333333333));
    ld   t2, .mask_33
    and  t0, a0, t2 // t0 = x & 0x3333333333333333
    srli t1, a0, 2 // t1 = x >> 2
    and  t1, t1, t2 // t1 &= 0x3333333333333333
    add  a0, t0, t1 // x = t0 + t1

    // Step 3: x = (x + (x >> 4)) & UINT64_C(0x0F0F0F0F0F0F0F0F);
    srli t0, a0, 4 // t0 = x >> 4
    add  a0, a0, t0 // x += t0 (x >> 4)
    ld   t1, .mask_0F
    and  a0, a0, t1 // x &= 0x0F0F0F0F0F0F0F0F

    // x = x * UINT64_C(0x0101010101010101) >> 56;
    ld   t0, .mask_01
    mul  a0, a0, t0 // x *= 0x0101010101010101
    srli a0, a0, 56 // x >>= 56 --> return
    // ----- Aggressive Temporary Registers Clearance -----
    ld t0, x0 // Zero out the temporary register t0
    ld t1, x0 // Zero out the temporary register t1
    ld t2, x0 // Zero out the temporary register t2
    ret




/* ----- Miscallenous Helper Blocks ----- */
.all_zero:
    ld a0, 64
    ret
/* Bit-manipulation Constants */
.align 3
.mask_55: .quad 0x5555555555555555
.mask_33: .quad 0x3333333333333333
.mask_0F: .quad 0x0F0F0F0F0F0F0F0F
.mask_01: .quad 0x0101010101010101
.mask_b1: .quad 0x00FF00FF00FF00FF
.mask_b2: .quad 0x0000FFFF0000FFFF


#endif
