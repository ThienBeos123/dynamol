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


asm_text_section
.intel_syntax   noprefix
.p2align	    4, 0x90
.globl          _x86sv_crt_lt
.globl          _x86sv_crt_gt
.globl          _x86sv_crt_leq
.globl          _x86sv_crt_geq

.type           _x86sv_crt_lt, %function
.type           _x86sv_crt_gt, %function
.type           _x86sv_crt_leq, %function
.type           _x86sv_crt_geq, %function


/* ------- UNSIGNED 64 BIT LESS THAN -------
* uint8_t _x86sv_crt_lt(uint64_t a, uint64_t b)
* arg:
*   rdi: uint64_t a       (64 BIT)
*   rsi: uint64_t b       (64 BIT)
* ret: rax -- 1 (true) | 0 (false)
*/
_x86sv_crt_lt:
    cmp rdi, rsi
    setb al
    movzx rax, al ; Zero extension
    mov rdi, 0 # Register Clearance
    mov rsi, 0 # Register Clearance
    ret


/* ------- UNSIGNED 64 BIT GREATER THAN -------
* uint8_t _x86sv_crt_gt(uint64_t a, uint64_t b)
* arg:
*   rdi: uint64_t a       (64 BIT)
*   rsi: uint64_t b       (64 BIT)
* ret: rax -- 1 (true) | 0 (false)
*/
_x86sv_crt_gt:
    cmp rdi, rsi
    seta al
    movzx rax, al ; Zero extension
    mov rdi, 0 # Register Clearance
    mov rsi, 0 # Register Clearance
    ret


/* ------- UNSIGNED 64 BIT LESS THAN OR EQUAL -------
* uint8_t _x86sv_crt_leq(uint64_t a, uint64_t b)
* arg:
*   rdi: uint64_t a       (64 BIT)
*   rsi: uint64_t b       (64 BIT)
* ret: rax -- 1 (true) | 0 (false)
*/
_x86sv_crt_leq:
    cmp rdi, rsi
    setbe al
    movzx rax, al ; Zero extension
    mov rdi, 0 # Register Clearance
    mov rsi, 0 # Register Clearance
    ret



/* ------- UNSIGNED 64 BIT LESS THAN OR EQUAL -------
* uint8_t _x86sv_crt_geq(uint64_t a, uint64_t b)
* arg:
*   rdi: uint64_t a       (64 BIT)
*   rsi: uint64_t b       (64 BIT)
* ret: rax -- 1 (true) | 0 (false)
*/
_x86sv_crt_geq:
    cmp rdi, rsi
    setae al
    movzx rax, al ; Zero extension
    mov rdi, 0 # Register Clearance
    mov rsi, 0 # Register Clearance
    ret


#endif
#endif
