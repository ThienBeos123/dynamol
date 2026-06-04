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



#include "__sys_conn.h"
#if __x86_64__


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


#endif