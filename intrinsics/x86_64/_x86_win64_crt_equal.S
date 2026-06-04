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
.globl          _x86w64_crt_ispos
.globl          _x86w64_crt_isneg
.globl          _x86w64_crt_eq
.globl          _x86w64_crt_neq
.globl          _x86w64_crt_select

.type           _x86w64_crt_ispos, %function
.type           _x86w64_crt_isneg, %function
.type           _x86w64_crt_eq, %function
.type           _x86w64_crt_neq, %function
.type           _x86w64_crt_select, %function


/* ------- 64 BIT POSITIVITY CHECK -------
* uint8_t _x86w64_crt_ispos(int64_t a)
* arg: rdx -- int64_t a    (64 BIT)
* ret: rax -- 1 (true) | 0 (false)
*/
_x86w64_crt_ispos:
    test rdx, rdx ; Test if positive
    sets al ; Set sign
    movzx rax, al
    mov rdx, 0 ; register clearance
    ret


/* ------- 64 BIT NEGATIVITY CHECK -------
* uint8_t _x86w64_crt_isneg(int64_t a)
* arg: rdx -- int64_t a    (64 BIT)
* ret: rax -- 1 (true) | 0 (false)
*/
_x86w64_crt_isneg:
    test rdx, rdx ; Test if negative
    setns al ; Set if NEGATIVE sign
    movzx rax, al
    mov rdx, 0 ; register clearance
    ret


/* ------- 64 BIT EQUALITY -------
* uint8_t _x86w64_crt_eq(uint64_t a, uint64_t b)
* arg:
*   rdx: uint64_t a       (64 BIT)
*   rcx: uint64_t b       (64 BIT)
* ret: rax -- 1 (true) | 0 (false)
*/
_x86w64_crt_eq:
    cmp rdx, rcx
    sete al
    movzx rax, al
    mov rcx, 0 ; register clearance
    mov rdx, 0 ; register clearance
    ret



/* ------- 64 BIT INEQUALITY -------
* uint8_t _x86w64_crt_neq(uint64_t a, uint64_t b)
* arg:
*   rdx: uint64_t a       (64 BIT)
*   rcx: uint64_t b       (64 BIT)
* ret: rax -- 1 (true) | 0 (false)
*/
_x86w64_crt_neq:
    cmp rdx, rcx
    setne al
    movzx rax, al
    mov rcx, 0 ; register clearance
    mov rdx, 0 ; register clearance
    ret


/* ------- 64 BIT SELECTION -------
* uint8_t _x86w64_crt_select(uint8_t cond, uint64_t a, uint64_t b)
* arg:
*   rdx: uint8_t cond     (8 BIT -- 1 | 0)
*   rcx: uint64_t a       (64 BIT)
*   r8:  uint64_t b       (64 BIT)
* ret: EITHER x1 OR x2
*/
_x86w64_crt_select:
    test rdx, rdx ; Test the condition register
    mov rax, r8 ; Assume the condition is false ---> rax = b
    cmovnz rax, rcx
    ; If test evaluates to TRUE ---> rax = a
    ; ELSE (test evaluates to FALSE) ---> cmovnz is IGNORED
    mov rcx, 0 ; register clearance
    mov rdx, 0 ; register clearance
    mov r8, 0  ; register clearance
    ret



#endif
#endif
