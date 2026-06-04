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



#include "__x86_conn__.h"
#if __ARCH_X86_64__
#if __ABI_X64_SYSV__


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
.intel_syntax   noprefix
.globl	        _x86sv_modinv64
.type           _x86sv_modinv64, %function
.p2align	    4, 0x90


/* ------- 64 BIT MODULAR INVERSE OF MODULO 2^64 -------
* uint64_t _x86sv_modinv64(uint64_t x) 
* arg: rdi: uint64_t x   (64 BIT)
* ret: rax: x^-1 mod(2^64)
*/
_x86sv_modinv64:
    ; Calculating table's offset
    mov rax, rdi ; rax = rdi
    shr rax, 1 ; rax >> 1
    and rax, 0x7F ; rax & 0x7F
    ; rax = (rdi >> 1) & 0x7F/0b1111111
    movzx   rax, byte ptr [rip + inv8_lookup + rax] ; rax = inv8_lookup[rax]

    ; General formula: res *= 2 - x * res
    ; Iteration 1:
    mov rsi, 2
    mov rdx, rax ; rdx = rax/res
    imul    rdx, rdi ; res * x
    sub rsi, rdx ; 2 - (res * x)
    imul    rax, rsi ; res *= 2 - (res * x)

    ; Iteration 2:
    mov rsi, 2
    mov rdx, rax ; rdx = rax / res
    imul    rdx, rdi ; res * x
    sub rsi, rdx ; 2 - (res * x)
    imul    rax, rsi ; res *= 2 - (res * x)

    ; Iteration 3:
    mov rsi, 2
    mov rdx, rax ; rdx = rax / res
    imul    rdx, rdi ; res * x
    sub rsi, rdx ; 2 - (res * x)
    imul    rax, rsi ; res *= 2 - (res * x)
    ret


#endif
#endif