#include "_x86_conn.h"
#if __ARCH_X86_64__
#if __ABI_X64_SYSV__


asm_text_section
.intel_syntax   noprefix
.globl	        _x86sv_hw_drbg
.globl          _x86sv_hw_trng

.type	        _x86sv_hw_drbg, %function
.type           _x86sv_hw_trng, %function
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
    ret




#endif
#endif