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



asm_text_section
.option     nopic
.attribute  arch, "rv64gc_zihintpause"
.attribute  unaligned_access, 0
.attribute  stack_align, 16
.align      1

.globl      _rv64_shallow_halt
.globl      _rv64_full_halt


.type       _rv64_shallow_halt, @function
.type       _rv64_full_halt, @function




// Halt CPU Execution by a few cycles via the ISB instruction,
/* ----- HARDWRE CPU EXECUTION HALT - FULL SYSTEM FLUSH -----
* void _rv64_full_halt(void)
* arg: NONE
* ret: NONE
*/
_rv64_full_halt:
    fence w, 0
    ret



// Halt CPU Execution by a single cycles via the NOP addi instruction,
// Add 0 into x0 (zero register) and basically is useless
/* ----- HARDWRE CPU EXECUTION HALT - SHALLOW NOP HALTH -----
* void _rv64_shallow_halt(void)
* arg: NONE
* ret: NONE
*/
_rv64_shallow_halt:
    addi x0, x0, 0
    ret



#endif