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
.globl	        _x86w64_hw_drbg
.globl          _x86w64_hw_trng
.globl          _x86w64_full_halt
.globl          _x86w64_shallow_halt

.type	        _x86w64_hw_drbg, %function
.type           _x86w64_hw_trng, %function
.type           _x86w64_full_halt, %function
.type           _x86w64_shallow_halt, %function
.p2align	    4, 0x90



; Uses hardware DRBG, in which it alos utilizes hardware entropy, 
; to seed entropy to the user swiftly
/* ----- HARDWRE DETERMINISTIC RANDOM BIT GENERATOR -----
* uint64_t _x86w64_hw_drbg(int *err)
* arg:
*   rcx: int *err (16 bit)
* ret:
*   rax: Random Number
*   rcx: 1 (error) / 0 (success)
*/
_x86w64_hw_drbg:
    rdrand rax ; Return with the random number
    setnc sil ; Set the error flag inversely of RDRAND error returns
    ; RDRAND Success --> CF = 1 and vice versa
    movzx rsi, sil ; Zero extends sil (1-byte) --> rsi (8-byte)
    mov [rcx], rsi ; *rcx = rsi
    mov rsi, 0 ; register clearance
    ret



; Harvests raw, unmanipulated True Entropy from hardware entropy pool
/* ----- HARDWRE TRUE RANDOM NUMBER GENERATOR -----
* uint64_t _x86w64_hw_trng(int *err)
* arg:
*   rcx: int *err (16 bit)
* ret:
*   rax: Random Number
*   rcx: 1 (error) / 0 (success)
*/
_x86w64_hw_trng:
    rdseed rax ; Return with the random number
    setnc sil ; Set the error flag inversely of RDRAND error returns
    ; RDRAND Success --> CF = 1 and vice versa
    movzx rsi, sil ; Zero extends sil (1-byte) --> rsi (8-byte)
    mov [rcx], rsi ; *rcx = rsi
    mov rsi, 0 ; register clearance
    ret



; Halt CPU Execution by a few cycles via the PAUSE instruction,
; Which does a system flush or whatever I'm sorry dawg
/* ----- HARDWRE CPU EXECUTION HALT - FULL SYSTEM FLUSH -----
* void _x86w64_full_halt(void)
* arg: NONE
* ret: NONE
*/
_x86w64_full_halt:
    pause
    ret


; Halt CPU Execution by a single cycles via the 9-byte NOP instruction.
; It basically does nothing, and is a really shallow NOP, in
; which it delays about at 1 CPU cycle / call
/* ----- HARDWRE CPU EXECUTION HALT - SHALLOW NOP HALTH -----
* void _arm64_shallow_halt(void)
* arg: NONE
* ret: NONE
*/
_x86w64_shallow_halt:
    nop [eax + eax*1 + 00000000h]
    ret




#endif
#endif