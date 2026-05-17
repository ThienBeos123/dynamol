#ifndef ___DNML_CAPS_H___
#define ___DNML_CAPS_H___


//* ----------- INCLUDES & MACROS ----------- *//
#include "__platform.h"
#include "__compiler.h"
#include "asm/__sys_conn.h"
#include <stdint.h>
#include <string.h>

#if defined(__linux__) || defined(__linux) 
    #include <sys/syscall.h>
#elif defined(__FreeBSD__) || defined(__OpenBSD__)
    #include <sys/aux.h>
#elif __compiler_msvc || defined(_WIN32)
    #include <intrin.h>
    #include <windows.h>
#endif

// Bit definitions for CPUID
#define bit_RDRAND (1 << 30)
#define bit_SSE42 (1 << 20)
#define bit_ABM (1 << 5)
#define bit_BMI1 (1 << 3)
// Platform Convenience Macro
#define ON_WINDOWS (defined(_WIN32) || __compiler_msvc)
#define ON_ANY_OTHER (__OS_LINUX__ || __OS_MACOS__ || __OS_IOS__ || __OS_BSD__)


//* ------- TYPE & OBJECT DECLARATIONS ------- *//
typedef struct {
    // RISC-V EXTENSIONS
    uint8_t rv64_zbb;
    uint8_t rv64_zba;
    uint8_t rv64_zbs;
    // X86_64 EXTENSIONS
    uint8_t x86_abm; uint8_t x86_bmi1;
    uint8_t x86_sse4_2; uint8_t x86_rdrand;
    // AARCH64 EXTENSIONS
} _dnml_hwcaps;
extern _dnml_hwcaps libdnml_caps;

//* --------- BARE-METAL MANUAL FLAGS --------- *//
// RISC-V 64 bit Flags
#define _DNML_BARE_RISCV_ZBB 0
#define _DNML_BARE_RISCV_ZBA 0
#define _DNML_BARE_RISCV_ZBS 0
// x86_64 Flags
#define _DNML_BARE_X86_ABM 0
#define _DNML_BARE_X86_BMI1 0
#define _DNML_BARE_X86_SSE4_2 0
// ARM64/AARCH64 Flags


//* --------- EXTENSION DETECTION FUNCTIONS --------- *//
void __DNML_DETRV64_HWCAPS(void);
void __DNML_DETX64_HWCAPS(void);
void __DNML_DETARM64_HWCAPS(void);
void _libdnml_detect_hwcaps(void);





#endif