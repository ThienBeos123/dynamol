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
.align      12
.globl _debruijin_clz:
    .quad 63,  0, 58,  1, 59, 47, 53,  2, 60, 39, 48, 27, 54, 33, 42,  3
    .quad 61, 51, 37, 40, 49, 18, 28, 20, 55, 30, 34, 11, 43, 14, 22,  4
    .quad 62, 57, 46, 52, 38, 26, 32, 41, 50, 36, 17, 19, 29, 10, 13, 21
    .quad 56, 45, 25, 31, 35, 16,  9, 12, 44, 24, 15,  8, 23,  7,  6,  5
.globl _debruijin_ctz:
    .quad 0,  1,  2, 53,  3,  7, 54, 27,  4, 38, 41,  8, 34, 55, 48, 28
    .quad 62,  5, 39, 46, 44, 42, 22,  9, 24, 35, 59, 56, 49, 18, 29, 11
    .quad 63, 52,  6, 26, 37, 40, 33, 47, 61, 45, 43, 21, 23, 58, 17, 10
    .quad 51, 25, 36, 32, 60, 44, 57, 16, 50, 31, 15, 14, 13, 12, 50, 30



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
    beqz a0, .all_zero // if (!x) return 64;
    // Smear bits to the right to fill all positions bellow highest bit;
    // x |= x >> 1;
    srli t0, a0, 1
    or a0, a0, t0
    // x |= x >> 2
    srli t0, a0, 2
    or a0, a0, t0

    // x |= x >> 4;
    srli t0, a0, 4
    or a0, a0, t0
    // x |= x >> 8
    srli t0, a0, 8
    or a0, a0, t0

    // x |= x >> 16;
    srli t0, a0, 16
    or a0, a0, t0
    // x |= x >> 32
    srli t0, a0, 32
    or a0, a0, t0

    // Calculating the Magic De-brujin Index
    ld t0, 0x07C4ACDDFFB40F2D
    mul a0, a0, t0
    srli a0, a0, 58

    // Accessing the De Brujin's Table constant
    la t1, _debruijin_clz
    slli a0, a0, 3 // Mul by 8 to match the 64-bit alignment
    add t0, t1, a0 // t0 = base_adress + (index * 8)
    ld t1, 0(t0) // t1 = *t0
    ld a0, 0(t0) // return *t0 (a0 = *t0)
    ret
/* ------- COUNT LEADING ZERO 64 BIT - CRYPTOGRAPHICALLY-SECURED -------
* uint8_t _rv64_clz64c(uint64_t x)
* arg: a0: uint64_t x --> (64 BIT)
* ret: a0: clz(x) ------> (8  BIT)
*/
_rv64_clz64c:
    sub a1, x0, a0 // a1 = (-x)
    or a1, a0, a1 // a1 |= x (a0)
    xori a1, a1, -1 // ~a10 (== a10 ^ 0xFFFFFFFFFFFFFFFF)
    srli a1, a1, 63 // a1 >>= 63 63
    ld a2, 0 // Set a2 to 0 for clearance
    /* Variables register list from now on:
        - U64 a0: x             -- (THE INPUT)
        - U64 a1: is_zero_mask  -- ~(x | (-x)) >> 63;
        - U64 a2: n             -- (temporary result holder)
        - U64 a3: mask          -- DEFINED LATER
    */

    // Step 1: mask = -((int64_t)!(x >> 32)); n += mask & 32; x <<= mask & 32;
    srli a3, a0, 32 // a3 = x >> 32
    seqz a3, a3 // !(a3) --> !(x >> 32)
    sub a3, x0, a3 // -((int64_t)(...a3...))
    andi a3, a3, 32 // mask &= 32
    add a2, a2, a3 // n += mask & 32
    sll a0, a0, a3 // x <<= mask & 32

    // Step 2: mask = -((int64_t)!(x >> 48)); n += mask & 16; x <<= mask & 16;
    srli a3, a0, 48 // a3 = x >> 48
    seqz a3, a3 // !(a3) --> !(x >> 48)
    sub a3, x0, a3 // -((int64_t)(...a3...))
    andi a3, a3, 16 // mask &= 16
    add a2, a2, a3 // n += mask & 16
    sll a0, a0, a3 // x <<= mask & 16

    // Step 3: mask = -((int64_t)!(x >> 56)); n += mask & 8;  x <<= mask & 8;
    srli a3, a0, 56 // a3 = x >> 56
    seqz a3, a3 // !(a3) --> !(x >> 56)
    sub a3, x0, a3 // -((int64_t)(...a3...))
    andi a3, a3, 8 // mask &= 8
    add a2, a2, a3 // n += mask & 8
    sll a0, a0, a3 // x <<= mask & 8

    // Step 4: mask = -((int64_t)!(x >> 60)); n += mask & 4;  x <<= mask & 4;
    srli a3, a0, 60 // a3 = x >> 60
    seqz a3, a3 // !(a3) --> !(x >> 60)
    sub a3, x0, a3 // -((int64_t)(...a3...))
    andi a3, a3, 4 // mask &= 4
    add a2, a2, a3 // n += mask & 4
    sll a0, a0, a3 // x <<= mask & 4

    // Step 5: mask = -((int64_t)!(x >> 62)); n += mask & 2;  x <<= mask & 2;
    srli a3, a0, 62 // a3 = x >> 62
    seqz a3, a3 // !(a3) --> !(x >> 62)
    sub a3, x0, a3 // -((int64_t)(...a3...))
    andi a3, a3, 2 // mask &= 2
    add a2, a2, a3 // n += mask & 2
    sll a0, a0, a3 // x <<= mask & 2

    // Step 6: mask = -((int64_t)!(x >> 63)); n += mask & 1;
    srli a3, a0, 63 // a3 = x >> 63
    seqz a3, a3 // !(a3) --> !(x >> 63)
    sub a3, x0, a3 // -((int64_t)(...a3...))
    andi a3, a3, 1 // mask &= 1
    add a2, a2, a3 // n += mask & 1
    // Step 7: return (uint8_t)(n ^ (is_zero_mask & (n ^ U64_BITS)));
    xori t0, a2, 64 // t0 = n ^ U64_BITS
    and t0, a1, t0 // t0 = is_zero_mask & (n ^ U64_BITS)
    xor t0, a2, t0 // t0 = n ^ (is_zero_mask & (n ^ U64_BITS));
    mv a0, t0 // return t0 via a0 = t0
    // ----- Aggressive Temporary Registers Clearance -----
    ld a1, x0 // Zero out the general-purpose register a1
    ld a2, x0 // Zero out the general-purpose register a2
    ld a3, x0 // Zero out the general-purpose register a3
    ld t0, x0 // Zero out the temporary register t0
    ret






/* ------- COUNT TRAILING ZERO 64 BIT - PERFORMANCE-BASED -------
* uint8_t _rv64_ctz64p(uint64_t x)
* arg: a0: uint64_t x --> (64 BIT)
* ret: a0: ctz(x) ------> (8  BIT)
*/
_rv64_ctz64p:
    // Calculating the Magic De-brujin Index
    // index = (x & -x) * 0x022FDD63CC95386DULL >> 58;
    sub t0, x0, a0 // t0 = (-x)
    and a0, a0, t0
    ld t0, 0x022FDD63CC95386D
    mul a0, a0, t0
    srli a0, a0, 58

    // Accessing the De Brujin's Table constant
    la t1, _debruijin_ctz
    slli a0, a0, 3 // Mul by 8 to match the 64-bit alignment
    add t0, t1, a0 // t0 = base_adress + (index * 8)
    ld a0, 0(t0) // return *t0 (a0 = *t0)
    ret
/* ------- COUNT TRAILING ZERO 64 BIT - CRYPTOGRAPHICALLY-SECURED -------
* uint8_t _rv64_ctz64c(uint64_t x)
* arg: a0: uint64_t x --> (64 BIT)
* ret: a0: ctz(x) ------> (8  BIT)
*/
_rv64_ctz64c:
    sub a1, x0, a0 // a1 = (-x)
    or a1, a0, a1 // a1 |= x (a0)
    xori a1, a1, -1 // ~a10 (== a10 ^ 0xFFFFFFFFFFFFFFFF)
    srli a1, a1, 63 // a1 >>= 63
    ld a2, 0 // Set a2 to 0 for clearance
    sub a4, x0, a0 // a4 = (-x)
    and a4, a0, a4 // a4 &= x --> (x & (-x))
    /* Variables register list from now on:
        - U64 a0: x             -- (THE INPUT)
        - U64 a1: is_zero_mask  -- ~(x | (-x)) >> 63;
        - U64 a2: n             -- (temporary result holder)
        - U64 a3: mask          -- DEFINED LATER
        - U64 a4: lowest_bit    -- (x & -x)
    */

    // Step 1: mask = -((int64_t)!(lowest_bit >> 32)); n += mask & 32; lowest_bit <<= mask & 32;
    srli a3, a4, 32 // a3 = lowest_bit >> 32
    seqz a3, a3 // !(a3) --> !(x >> 32)
    sub a3, x0, a3 // -((int64_t)(...a3...))
    andi a3, a3, 32 // mask &= 32
    add a2, a2, a3 // n += mask & 32
    sll a4, a4, a3 // lowest_bit <<= mask & 32

    // Step 2: mask = -((int64_t)!(lowest_bit >> 48)); n += mask & 16; lowest_bit <<= mask & 16;
    srli a3, a4, 48 // a3 = lowest_bit >> 48
    seqz a3, a3 // !(a3) --> !(x >> 48)
    sub a3, x0, a3 // -((int64_t)(...a3...))
    andi a3, a3, 16 // mask &= 16
    add a2, a2, a3 // n += mask & 16
    sll a4, a4, a3 // lowest_bit <<= mask & 16

    // Step 3: mask = -((int64_t)!(x >> 56)); n += mask & 8;  x <<= mask & 8;
    srli a3, a4, 56 // a3 = lowest_bit >> 56
    seqz a3, a3 // !(a3) --> !(x >> 56)
    sub a3, x0, a3 // -((int64_t)(...a3...))
    andi a3, a3, 8 // mask &= 8
    add a2, a2, a3 // n += mask & 8
    sll a4, a4, a3 // lowest_bit <<= mask & 8

    // Step 4: mask = -((int64_t)!(x >> 60)); n += mask & 4;  x <<= mask & 4;
    srli a3, a4, 60 // a3 = lowest_bit >> 60
    seqz a3, a3 // !(a3) --> !(x >> 60)
    sub a3, x0, a3 // -((int64_t)(...a3...))
    andi a3, a3, 4 // mask &= 4
    add a2, a2, a3 // n += mask & 4
    sll a4, a4, a3 // lowest_bit <<= mask & 4

    // Step 5: mask = -((int64_t)!(x >> 62)); n += mask & 2;  x <<= mask & 2;
    srli a3, a4, 62 // a3 = lowest_bit >> 62
    seqz a3, a3 // !(a3) --> !(x >> 62)
    sub a3, x0, a3 // -((int64_t)(...a3...))
    andi a3, a3, 2 // mask &= 2
    add a2, a2, a3 // n += mask & 2
    sll a4, a4, a3 // lowest_bit <<= mask & 2

    // Step 7: return (uint8_t)(ctz_val ^ (is_zero_mask & (ctz_val ^ U64_BITS)));
    ld t0, 63
    sub a2, t0, a2 // uint64_t ctz_val = 63 - n
    xori t0, a2, 64 // t0 = ctz_val ^ U64_BITS
    and t0, a1, t0 // t0 = is_zero_mask & (ctz_val ^ U64_BITS)
    xor t0, a2, t0 // t0 = ctz_val ^ (is_zero_mask & (ctz_val ^ U64_BITS));
    mv a0, t0 // return t0 via a0 = t0
    // ----- Aggressive Temporary Registers Clearance -----
    ld a1, x0 // Zero out the general-purpose register a1
    ld a2, x0 // Zero out the general-purpose register a2
    ld a3, x0 // Zero out the general-purpose register a3
    ld a4, x0 // Zero out the general-purpose register a4
    ld t0, x0 // Zero out the temporary register t0
    ret

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
