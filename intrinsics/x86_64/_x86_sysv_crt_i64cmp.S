#include "__x86_conn__.h"
#if __ARCH_X86_64__
#if __ABI_X64_SYSV__


asm_text_section
.intel_syntax   noprefix
.p2align	    4, 0x90
.globl          _x86sv_crt_lti64
.globl          _x86sv_crt_gti64
.globl          _x86sv_crt_leqi64
.globl          _x86sv_crt_geqi64

.type           _x86sv_crt_lti64, %function
.type           _x86sv_crt_gti64, %function
.type           _x86sv_crt_leqi64, %function
.type           _x86sv_crt_geqi64, %function


/* ------- UNSIGNED 64 BIT LESS THAN -------
* uint8_t _x86sv_crt_lti64(int64_t a, int64_t b)
* arg:
*   rdi: int64_t a       (64 BIT)
*   rsi: int64_t b       (64 BIT)
* ret: rax -- 1 (true) | 0 (false)
*/
_x86sv_crt_lti64:
    cmp rdi, rsi
    setl al
    movzx rax, al ; Zero extension
    mov rdi, 0 # Register Clearance
    mov rsi, 0 # Register Clearance
    ret


/* ------- UNSIGNED 64 BIT GREATER THAN -------
* uint8_t _x86sv_crt_gti64(int64_t a, int64_t b)
* arg:
*   rdi: int64_t a       (64 BIT)
*   rsi: int64_t b       (64 BIT)
* ret: rax -- 1 (true) | 0 (false)
*/
_x86sv_crt_gti64:
    cmp rdi, rsi
    setg al
    movzx rax, al ; Zero extension
    mov rdi, 0 # Register Clearance
    mov rsi, 0 # Register Clearance
    ret


/* ------- UNSIGNED 64 BIT LESS THAN OR EQUAL -------
* uint8_t _x86sv_crt_leqi64(int64_t a, int64_t b)
* arg:
*   rdi: int64_t a       (64 BIT)
*   rsi: int64_t b       (64 BIT)
* ret: rax -- 1 (true) | 0 (false)
*/
_x86sv_crt_leqi64:
    cmp rdi, rsi
    setle al
    movzx rax, al ; Zero extension
    mov rdi, 0 # Register Clearance
    mov rsi, 0 # Register Clearance
    ret



/* ------- UNSIGNED 64 BIT LESS THAN OR EQUAL -------
* uint8_t _x86sv_crt_geqi64(int64_t a, int64_t b)
* arg:
*   rdi: int64_t a       (64 BIT)
*   rsi: int64_t b       (64 BIT)
* ret: rax -- 1 (true) | 0 (false)
*/
_x86sv_crt_geqi64:
    cmp rdi, rsi
    setge al
    movzx rax, al ; Zero extension
    mov rdi, 0 # Register Clearance
    mov rsi, 0 # Register Clearance
    ret


#endif
#endif