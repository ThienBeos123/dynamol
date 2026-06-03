#include "__x86_conn__.h"
#if __ARCH_X86_64__
#if __ABI_X64_SYSV__


asm_text_section
.intel_syntax   noprefix
.globl	        _x86sv_hw_drbg
.globl          _x86sv_hw_trng
.globl          _x86sv_full_halt
.globl          _x86sv_shallow_halt

.type	        _x86sv_hw_drbg, %function
.type           _x86sv_hw_trng, %function
.type           _x86sv_full_halt, %function
.type           _x86sv_shallow_halt, %function
.p2align	    4, 0x90



; Uses hardware DRBG, in which it alos utilizes hardware entropy, 
; to seed entropy to the user swiftly
/* ----- HARDWRE DETERMINISTIC RANDOM BIT GENERATOR -----
* uint64_t _x86sv_hw_drbg(int *err)
* arg:
*   rdi: int *err (16 bit)
* ret:
*   rax: Random Number
*   rdi: 1 (error) / 0 (success)
*/
_x86sv_hw_drbg:
    rdrand rax ; Return with the random number
    setnc sil ; Set the error flag inversely of RDRAND error returns
    ; RDRAND Success --> CF = 1 and vice versa
    movzx rsi, sil ; Zero extends sil (1-byte) --> rsi (8-byte)
    mov [rdi], rsi ; *rdi = rsi
    mov rsi, 0 # Register clearance
    ret



; Harvests raw, unmanipulated True Entropy from hardware entropy pool
/* ----- HARDWRE TRUE RANDOM NUMBER GENERATOR -----
* uint64_t _x86sv_hw_trng(int *err)
* arg:
*   rdi: int *err (16 bit)
* ret:
*   rax: Random Number
*   rdi: 1 (error) / 0 (success)
*/
_x86sv_hw_trng:
    rdseed rax ; Return with the random number
    setnc sil ; Set the error flag inversely of RDRAND error returns
    ; RDRAND Success --> CF = 1 and vice versa
    movzx rsi, sil ; Zero extends sil (1-byte) --> rsi (8-byte)
    mov [rdi], rsi ; *rdi = rsi
    mov rsi, 0 # Register clearance
    ret


; Halt CPU Execution by a few cycles via the PAUSE instruction,
; Which does a system flush or whatever I'm sorry dawg
/* ----- HARDWRE CPU EXECUTION HALT - FULL SYSTEM FLUSH -----
* void _x86sv_full_halt(void)
* arg: NONE
* ret: NONE
*/
_x86sv_full_halt:
    pause
    ret


; Halt CPU Execution by a single cycles via the 9-byte NOP instruction.
; It basically does nothing, and is a really shallow NOP, in
; which it delays about at 1 CPU cycle / call
/* ----- HARDWRE CPU EXECUTION HALT - SHALLOW NOP HALTH -----
* void _x86sv_shallow_halt(void)
* arg: NONE
* ret: NONE
*/
_x86sv_shallow_halt:
    nop [eax + eax*1 + 00000000h]
    ret


#endif
#endif