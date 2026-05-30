#include "___rv64_conn__.h"
#if __ARCH_RVI64__


asm_rodata_section
inv8_lookup:
    .byte 0x01, 0xAB, 0xCD, 0xB7, 0x39, 0xA3, 0xC5, 0xEF
    .byte 0xF1, 0x1B, 0x3D, 0xA7, 0x29, 0x13, 0x35, 0xDF
    .byte 0xE1, 0x8B, 0xAD, 0x97, 0x19, 0x83, 0xA5, 0xCF
    .byte 0xD1, 0xFB, 0x1D, 0x87, 0x09, 0xF3, 0x15, 0xBF
    .byte 0xC1, 0x6B, 0x8D, 0x77, 0xF9, 0x63, 0x85, 0xAF
    .byte 0xB1, 0xDB, 0xFD, 0x67, 0xE9, 0xD3, 0xF5, 0x9F
    .byte 0xA1, 0x4B, 0x6D, 0x57, 0xD9, 0x43, 0x65, 0x8F
    .byte 0x91, 0xBB, 0xDD, 0x47, 0xC9, 0xB3, 0xD5, 0x7F
    .byte 0x81, 0x2B, 0x4D, 0x37, 0xB9, 0x23, 0x45, 0x6F
    .byte 0x71, 0x9B, 0xBD, 0x27, 0xA9, 0x93, 0xB5, 0x5F
    .byte 0x61, 0x0B, 0x2D, 0x17, 0x99, 0x03, 0x25, 0x4F
    .byte 0x51, 0x7B, 0x9D, 0x07, 0x89, 0x73, 0x95, 0x3F
    .byte 0x41, 0xEB, 0x0D, 0xF7, 0x79, 0xE3, 0x05, 0x2F
    .byte 0x31, 0x5B, 0x7D, 0xE7, 0x69, 0x53, 0x75, 0x1F
    .byte 0x21, 0xCB, 0xED, 0xD7, 0x59, 0xC3, 0xE5, 0x0F
    .byte 0x11, 0x3B, 0x5D, 0xC7, 0x49, 0x33, 0x55, 0xFF


asm_text_section
.option     nopic
.attribute  arch, "rv64gc"
.attribute  unaligned_access, 0
.attribute  stack_align, 16
.align      1

.globl      _rv64_modinv64
.type       _rv64_modinv64, @function

/* ------- 64 BIT MODULAR INVERSE OF MODULO 2^64 -------
* uint64_t _rv64_modinv64(uint64_t x) 
* arg: a0: uint64_t x   (64 BIT)
* ret: a0: x^-1 mod(2^64)
*/
_rv64_modinv64:
    # Index calculation
    addi t0, a0, 0 # t0 = a0
    srli t0, t0, 1 # t0 = x >> 1
    andi t0, t0, 0x7F # t0 = (x >> 1) & 0x7F

    # Table adress loading
    lui t1, %hi(inv8_lookup) # Loading upper 32 bits
    addi    t2, t1, %lo(inv8_lookup) # Loading lower 64 bits
    # ----> Full 64 bit adress of table's base (t2)
    # element_adress = arr_base + (index * byte_size)
    add t0, t2, t0 # --> res_adress = arr_base + index
    ld t1, 0(t0) # res = *res_adress

    # General Formula: res *= 2 - x * res
    # Iteration 1: 
    mv t0, 2 # t0 = 2
    mv t2, t1 # t2 = t1/res
    mul t2, t2, a0 # t2 = res * x
    sub t0, t0, t2 # t0 = 2 - (res * x)
    mul t1, t1, t0 # t1 = res * (2 - (res * x)
    # ---> res *= 2 - (x * res)

    # Iteration 2: 
    mv t0, 2 # t0 = 2
    mv t2, t1 # t2 = t1/res
    mul t2, t2, a0 # t2 = res * x
    sub t0, t0, t2 # t0 = 2 - (res * x)
    mul t1, t1, t0 # t1 = res * (2 - (res * x)
    # ---> res *= 2 - (x * res)

    # Iteration 3: 
    mv t0, 2 # t0 = 2
    mv t2, t1 # t2 = t1/res
    mul t2, t2, a0 # t2 = res * x
    sub t0, t0, t2 # t0 = 2 - (res * x)
    mul t1, t1, t0 # t1 = res * (2 - (res * x)
    # ---> res *= 2 - (x * res)
    mv a0, t1 # return res;
    ret




#endif