#ifndef ____DNML_SYS_CONN
#define ____DNML_SYS_CONN
#if __x86_64__


#include "__asm_macros.h"
#include "../__platform.h"

extern void __sysv_get_cpuid(unsigned int leaf, unsigned int subleaf, unsigned int registers[4]);
extern unsigned int __sysv_cpu_maxl(void);



#endif
#endif