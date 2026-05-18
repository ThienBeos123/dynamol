#include "_arm64_conn.h"
#if __ARCH_ARM64__


asm_text_section
.globl      ASM_SYM(arm64_full_halt)
.globl      ASM_SYM(arm64_shallow_halt)


FUNC_TYPE(ASM_SYM(arm64_full_halt))
FUNC_TYPE(ASM_SYM(arm64_shallow_halt))


FUNC_SIZE(ASM_SYM(arm64_full_halt))
FUNC_TYPE(ASM_SYM(arm64_shallow_halt))

.p2align    2



# Halt CPU Execution by a few cycles via the ISB instruction,
# in which the instruction flushes the CPU prefetch buffer
# (containing the prefetched data and instructions for faster access)
/* ----- HARDWRE CPU EXECUTION HALT - FULL SYSTEM FLUSH -----
* void _arm64_full_halt(void)
* arg: NONE
* ret: NONE
*/
ASM_SYM(arm64_full_halt):
    # This flushes the system's prefetch buffer state
    isb sy, #15
    ret




# Halt CPU Execution by a single cycles via the YIELD instruction,
# in which we basically hint at the CPU of a NOP.
# This version is often times not preferred unless performance is
# critical over a _arm64_full_halt, as it doesn't delay enough
# time, and doesn't actually executes any tasks
/* ----- HARDWRE CPU EXECUTION HALT - SHALLOW NOP HALTH -----
* void _arm64_shallow_halt(void)
* arg: NONE
* ret: NONE
*/
ASM_SYM(arm64_shallow_halt):
    # Do a shallow NOP halt
    yield
    ret





#endif