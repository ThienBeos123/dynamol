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



#ifndef ___DNML_CAPS_H___
#define ___DNML_CAPS_H___


//* ----------- INCLUDES & MACROS ----------- *//
#include "__platform.h"
#include "__compiler.h"
#include "asm/__sys_conn.h"
#include <stdint.h>
#include <string.h>

#if __OS_LINUX__
    #include <sys/syscall.h>
#elif __OS_BSD__
    #include <sys/aux.h>
#elif __OS_WIN64__
    #include <intrin.h>
    #include <windows.h>
#endif

// Bit definitions for CPUID
#define dnml_bit_RDSEED (1 << 18)
#define dnml_bit_RDRAND (1 << 30)
#define dnml_bit_SSE42 (1 << 20)
#define dnml_bit_ABM (1 << 5)
#define dnml_bit_BMI1 (1 << 3)
// Platform Convenience Macro
#define ON_ANY_OTHER (defined(__OS_LINUX__) || defined(__OS_MACOS__) || defined(__OS_IOS__) || defined(__OS_BSD__))


//* ------- TYPE & OBJECT DECLARATIONS ------- *//
/* RISC-V 64 BIT HWCAPS STRUCT */
typedef struct {
    // RISC-V BIT EXTENSIONS
    uint8_t rv64_zbb, rv64_zba, rv64_zbs;
    // RISC-V Security Extensions
    uint8_t rv64_zkr;
    // RISC-V Hardware Interaction
    uint8_t rv64_zihintpause;
} _dnml_hwcaps_rv64;

/* x86_64 64 BIT HWCAPS STRUCT */
typedef struct {
    /* Bit Manipulation Extensions */
   uint8_t x86_abm; uint8_t x86_bmi1;
   /* SIMD / Vectorization Extensions */
   uint8_t x86_sse4_2;
   /* Security Extensions */
   uint8_t x86_rdrand; uint8_t x86_rdseed;
} _dnml_hwcaps_x64;

/* ARM64 64 BIT HWCAPS STRUCT */
typedef struct {
    /* Security Operations */
    uint8_t armv85_feat_rng;
} _dnml_hwcaps_arm64;

extern _dnml_hwcaps_rv64 libdnml_rv64_caps;
extern _dnml_hwcaps_x64 libdnml_x64_caps;
extern _dnml_hwcaps_arm64 libdnml_arm64_caps;

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