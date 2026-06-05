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



#include "__arm64_conn__.h"
#if __ARCH_ARM64__


asm_text_section
.globl      ASM_SYM(arm64_hw_drbg)
.globl      ASM_SYM(arm64_hw_trng)
.globl      ASM_SYM(arm64_full_halt)
.globl      ASM_SYM(arm64_shallow_halt)

FUNC_TYPE(ASM_SYM(arm64_hw_drbg))
FUNC_TYPE(ASM_SYM(arm64_hw_trng))
FUNC_TYPE(ASM_SYM(arm64_full_halt))
FUNC_TYPE(ASM_SYM(arm64_shallow_halt))


FUNC_SIZE(ASM_SYM(arm64_full_halt))
FUNC_SIZE(ASM_SYM(arm64_shallow_halt))
FUNC_SIZE(ASM_SYM(arm64_hw_drbg))
FUNC_SIZE(ASM_SYM(arm64_hw_trng))

.p2align    2


// Uses hardware DRBG, in which it alos utilizes hardware entropy,
// to seed entropy to the user swiftly
/* ----- HARDWRE DETERMINISTIC RANDOM BIT GENERATOR -----
* uint64_t _arm64_hw_drbg(int *err)
* arg: x1: int *err (16 bit)
* ret:
*   x0: Random Number
*   x1: 1 (error) / 0 (success)
*/
ASM_SYM(arm64_hw_drbg):
    mrs x0, s3_3_c2_c4_0 // Reading and returning the RNDR system register
    // Checking if RNDR threw an error (blockage)
    // NZCV is set to 0b0100 on failure, which corresponds to:
    // N = 0, Z = 1, C = 0, V = 0
    cset    x3, eq // Cflag = 1 --> x3 = 1 (error)
    str x3, [x1] // Store x3 into the adress of x1 (*x1 = x3)
    mov x3, #0
    ret



// Harvests raw, unmanipulated True Entropy from hardware entropy pool
/* ----- HARDWRE TRUE RANDOM NUMBER GENERATOR -----
* uint64_t _arm64_hw_trng(int *err)
* arg: x1: int *err (16 bit)
* ret:
*   x0: Random Number
*   x1: 1 (error) / 0 (success)
*/
ASM_SYM(arm64_hw_trng):
    mrs x0, s3_3_c2_c4_1 // Reading and returning the RNDRRS system register
    // Checking if RNDRRS threw an error (blockage)
    // NZCV is set to 0b0100 on failure, which corresponds to:
    // N = 0, Z = 1, C = 0, V = 0
    cset    x3, eq // Cflag = 1 --> x3 = 1 (error)
    str x3, [x1] // Store x3 into the adress of x1 (*x1 = x3)
    mov x3, #0
    ret


// Halt CPU Execution by a few cycles via the ISB instruction,
// in which the instruction flushes the CPU prefetch buffer
// (containing the prefetched data and instructions for faster access)
/* ----- HARDWRE CPU EXECUTION HALT - FULL SYSTEM FLUSH -----
* void _arm64_full_halt(void)
* arg: NONE
* ret: NONE
*/
ASM_SYM(arm64_full_halt):
    // This flushes the system's prefetch buffer state
    isb sy
    ret




// Halt CPU Execution by a single cycles via the YIELD instruction,
// in which we basically hint at the CPU of a NOP.
// This version is often times not preferred unless performance is
// critical over a _arm64_full_halt, as it doesn't delay enough
// time, and doesn't actually executes any tasks
/* ----- HARDWRE CPU EXECUTION HALT - SHALLOW NOP HALTH -----
* void _arm64_shallow_halt(void)
* arg: NONE
* ret: NONE
*/
ASM_SYM(arm64_shallow_halt):
    // Do a shallow NOP halt
    yield
    ret





#endif
