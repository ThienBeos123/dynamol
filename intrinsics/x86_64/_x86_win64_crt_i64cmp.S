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
#if __ABI_X64_WIN64__


asm_text_section
.intel_syntax   noprefix
.p2align	    4, 0x90
.globl          _x86w64_crt_lti64
.globl          _x86w64_crt_gti64
.globl          _x86w64_crt_leqi64
.globl          _x86w64_crt_geqi64

.type           _x86w64_crt_lti64, %function
.type           _x86w64_crt_gti64, %function
.type           _x86w64_crt_leqi64, %function
.type           _x86w64_crt_geqi64, %function


/* ------- UNSIGNED 64 BIT LESS THAN -------
* uint8_t _x86w64_crt_lti64(int64_t a, int64_t b)
* arg:
*   rcx: int64_t a       (64 BIT)
*   rdx: int64_t b       (64 BIT)
* ret: rax -- 1 (true) | 0 (false)
*/
_x86w64_crt_lti64:
    cmp rcx, rdx
    setl al
    movzx rax, al
    mov rcx, 0 ; register clearance
    mov rdx, 0 ; register clearance
    ret


/* ------- UNSIGNED 64 BIT GREATER THAN -------
* uint8_t _x86w64_crt_gti64(int64_t a, int64_t b)
* arg:
*   rcx: int64_t a       (64 BIT)
*   rdx: int64_t b       (64 BIT)
* ret: rax -- 1 (true) | 0 (false)
*/
_x86w64_crt_gti64:
    cmp rcx, rdx
    setg al
    movzx rax, al
    mov rcx, 0 ; register clearance
    mov rdx, 0 ; register clearance
    ret


/* ------- UNSIGNED 64 BIT LESS THAN OR EQUAL -------
* uint8_t _x86w64_crt_leqi64(int64_t a, int64_t b)
* arg:
*   rcx: int64_t a       (64 BIT)
*   rdx: int64_t b       (64 BIT)
* ret: rax -- 1 (true) | 0 (false)
*/
_x86w64_crt_leqi64:
    cmp rcx, rdx
    setle al
    movzx rax, al
    mov rcx, 0 ; register clearance
    mov rdx, 0 ; register clearance
    ret



/* ------- UNSIGNED 64 BIT LESS THAN OR EQUAL -------
* uint8_t _x86w64_crt_geqi64(int64_t a, int64_t b)
* arg:
*   rcx: int64_t a       (64 BIT)
*   rdx: int64_t b       (64 BIT)
* ret: rax -- 1 (true) | 0 (false)
*/
_x86w64_crt_geqi64:
    cmp rcx, rdx
    setge al
    movzx rax, al
    mov rcx, 0 ; register clearance
    mov rdx, 0 ; register clearance
    ret



#endif
#endif
