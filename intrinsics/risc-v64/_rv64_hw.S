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