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
.globl	        _x86sv_clz64e
.globl          _x86sv_clz64s
.globl          _x86sv_ctz64e
.globl          _x86sv_ctz64s
.globl          _x86sv_bswap64
.globl          _x86sv_pcnt64e
.globl          _x86sv_pcnt64s

.type	        _x86sv_clz64e, %function
.type           _x86sv_clz64s, %function
.type           _x86sv_ctz64e, %function
.type           _x86sv_ctz64s, %function
.type           _x86sv_bswap64, %function
.type           _x86sv_pcnt64e, %function
type            _x86sv_pcnt64s, %function
.p2align	    4, 0x90


/* ------- COUNT LEADING ZERO 64 BIT - ABM-EXTENDED -------
* uint8_t _x86sv_clz64e(uint64_t x)
* arg: rdi: uint64_t x --> (64 BIT)
* ret: rax: lzcnt(x) ------> (8  BIT)
*/
_x86sv_clz64e:
    lzcnt rax, rdi
    mov rdi, 0 # register clearance
    ret
/* ------- COUNT LEADING ZERO 64 BIT - 80386-STANDARD -------
* uint8_t _x86sv_clz64s(uint64_t x)
* arg: rdi: uint64_t x --> (64 BIT)
* ret: rax: bsr(x) ------> (8  BIT)
*/
_x86sv_clz64s:
    mov rcx, 64
    bsf rax, rdi
    cmovz rax, rcx
    mov rcx, 0 # register clearance
    mov rdi, 0 # register clearance
    ret




/* ------- COUNT TRAILING ZERO 64 BIT - BMI1-EXTENDED -------
* uint8_t _x86sv_ctz64e(uint64_t x)
* arg: rdi: uint64_t x --> (64 BIT)
* ret: rax: tzcnt(x) ------> (8  BIT)
*/
_x86sv_ctz64e:
    tzcnt rax, rdi
    mov rdi, 0 # register clearance
    ret
/* ------- COUNT TRAILING ZERO 64 BIT - 80386-STANDARD -------
* uint8_t _x86sv_ctz64s(uint64_t x)
* arg: rdi: uint64_t x --> (64 BIT)
* ret: rax: bsf(x) ------> (8  BIT)
*/
_x86sv_ctz64s:
    mov rcx, 64
    bsr rax, rdi
    cmovz rax, rcx
    mov rcx, 0 # register clearance
    mov rdi, 0 # register clearance
    ret


/* ------- BYTE SWAP 64 BIT -------
* uint64_t _x86sv_bswap64(uint64_t x)
* arg: rdi: uint64_t x ----> (64 BIT)
* ret: rax: bswap(x) ------> (8  BIT)
*/
_x86sv_bswap64:
    bswap   rdi
    mov rax, rdi
    mov rdi, 0 # register clearance
    ret



/* ------- POPULATION COUNT 64 BIT - SSE4.2-EXTENDED -------
* uint8_t _x86sv_pcnt64e(uint64_t x)
* arg: rdi: uint64_t x ----> (64 BIT)
* ret: rax: pcnt(x) -------> (8  BIT)
*/
_x86sv_pcnt64e:
    popcnt rax, rdi
    mov rdi, 0 # register clearance
    ret
/* ------- POPULATION COUNT 64 BIT - 80386-STANDARD -------
* uint8_t _x86sv_pcnt64s(uint64_t x)
* arg: rdi: uint64_t x ----> (64 BIT)
* ret: rax: pcnt(x) -------> (8  BIT)
*/
_x86sv_pcnt64s:
    ; --- Step 1: x = x - ((x >> 1) & 0x5555555555555555) ---
    mov rax, rdi
    shr rax, 1                      ; rax = x >> 1
    movabs rdx, 0x5555555555555555  ; Load mask 0x55...
    and rax, rdx                    ; rax = (x >> 1) & 0x55...
    sub rdi, rax                    ; rdi = x - rax (This updates our working x)

    ; --- Step 2: x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333) ---
    mov rax, rdi
    shr rax, 2                      ; rax = x >> 2
    movabs rdx, 0x3333333333333333  ; Load mask 0x33...
    and rdi, rdx                    ; rdi = x & 0x33...
    and rax, rdx                    ; rax = (x >> 2) & 0x33...
    add rdi, rax                    ; rdi = structural sum of pairs

    ; --- Step 3: x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0F ---
    mov rax, rdi
    shr rax, 4                      ; rax = x >> 4
    add rax, rdi                    ; rax = x + (x >> 4)
    movabs rdx, 0x0F0F0F0F0F0F0F0F  ; Load mask 0x0F...
    and rax, rdx                    ; rax = integrated byte totals

    ; --- Step 4 & 5: x = (x * 0x0101010101010101) >> 56 ---
    movabs rdx, 0x0101010101010101  ; Load multiplier mask
    imul rax, rdx                   ; rax = rax * 0x01... (sums all bytes into MSB)
    shr rax, 56                     ; Shift the accumulated total to lowest 8 bits
    ; ------ REGISTER CLEARANCE ---------
    mov rdi, 0
    mov rdx, 0
    ret



#endif
#endif