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



#ifndef ____DNML_SYS_CONN
#define ____DNML_SYS_CONN
#if __x86_64__


#include "__asm_macros.h"
#include "../__platform.h"

extern void __sysv_get_cpuid(unsigned int leaf, unsigned int subleaf, unsigned int registers[4]);
extern unsigned int __sysv_cpu_maxl(void);



#endif
#endif