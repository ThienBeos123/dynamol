#include "__sys_conn.h"

/* 
This file is reserved for:
+) x86_64 Architecture
+) System-V ABI (Unix-based + BSDs)
----> Exclude Win64 support (since it doesn't need to)
*/

asm_text_section
.intel_syntax   noprefix
.globl          __sysv_get_cpuid
.globl          __sysv_cpu_maxl
.p2align        4, 0x90

# void __sys_get_cpuid(
#    unsigned int leaf,         --> edi
#    unsigned int subleaf,      --> esi
#    unsigned int registers[4]  --> rdx
# )
__sysv_get_cpuid:
    push    rbx
    mov eax, edi
    mov ecx, esi
    cpuid
    mov [rdx],   eax
    mov [rdx+4], ebx
    mov [rdx+8], ecx
    mov [rdx+12], edx
    pop rbx
    ret

# unsigned int __sys_cpu_maxl(void)
# ---> ret: eax
__sysv_cpu_maxl:
    mov eax, 0
    mov ecx, 0
    cpuid
    ret