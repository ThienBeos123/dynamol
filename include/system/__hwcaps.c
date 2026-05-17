#include "__hwcaps.h"

// THIS FILE MAY NOT BE INCLUDED OR USED
// BY ANY OTHER FILE IN THE PROJECT LIB-DNML,
// WITH THE SOLE EXCEPTION BEING __HWCAPS.H itself

_dnml_hwcaps libdnml_caps;

//* --------- EXTENSION DETECTION FUNCTIONS --------- *//
// RV64 -- ZBB + ZBA + ZBS detections
static void ___rv64_LINUX_detcaps(void) {
#if __ARCH_RVI64__ && __OS_LINUX__
    struct riscv_hwprobe pairs[] = {
        { .key = RISCV_HWPROBE_KEY_IMA_EXIT_0 }
    }; long ret = syscall(__NR_riscv_hwprobe, pairs, 1, 0, NULL, 0);
    if (!ret) {
        uint64_t ext = pairs[0].value;
        libdnml_caps.rv64_zbb = (ext & RISCV_HWPROBE_EXT_ZBB) != 0;
        libdnml_caps.rv64_zba = (ext & RISCV_HWPROBE_EXT_ZBA) != 0;
        return;
    }
#endif
}
static void ___rv64_legLINUX_detcaps(void) {
#if __ARCH_RVI64__ && __OS_LINUX__
    FILE *f = fopen("/proc/cpuinfo", "r");
    if (f) {
        char curr_block[256];
        while (fgets(curr_block, sizeof(curr_block), f)) {
            if (strncmp(curr_block, "isa", 3) == 0) {
                libdnml_caps.rv64_zbb = 
                    strstr(curr_block, "_zbb") != NULL ||
                    strstr(curr_block, "zbb")  != NULL;
                libdnml_caps.rv64_zba = strstr(curr_block, "_zba") != NULL;
                break;
            }
        }
        fclose(f);
    }
#endif
}
static inline void ___rv64_BSD_detcaps(void) {
#if __ARCH_RVI64__ && __OS_BSD__
    unsigned long hwcap = 0;
    elf_aux_info(AT_HWCAP, &hwcap, sizeof(hwcap));
    libdnml_caps.rv64_zbb = (hwcap & HWCAP_ISA_B) != 0;
#endif
}
void __DNML_DETRV64_HWCAPS(void) {
#if __ARCH_RVI64__
#if __OS_LINUX__
    // Modern Linux detection
    #if defined(__NR_riscv_hwprobe)
        ___rv64_LINUX_detcaps(); return;
    #endif // Legacy Linux detection
    ___rv64_legLINUX_detcaps();
#elif __OS_BSD__
    ___rv64_BSD_detcaps();
#else // Bare metal, Embedded systems
    // Users are forced to manually set 
    // the _DNMLEMB_RISCV_ZBB flag
    libdnml_caps.rv64_zbb = _DNML_BARE_RISCV_ZBB;
    libdnml_caps.rv64_zba = _DNML_BARE_RISCV_ZBA;
    libdnml_caps.rv64_zbs = _DNML_BARE_RISCV_ZBS;
#endif
#endif
}


// x86_64 -- ABM + BMI1 detections
static inline void ___x86_WIN64_detcaps(void) {
#if __ARCH_X86_64__ && ON_WINDOWS
    int cpuInfo[4] = {0};
    // BMI1 Checking
    __cpuid(cpuInfo, 7) // Scan Page 0x00000007
    libdnml_caps.x86_bmi1 = (cpuInfo[1] & bit_BMI1) != 0;
    // ABM Checking
    __cpuid(cpuInfo, 0x80000001);
    libdnml_caps.x86_abm = (cpuInfo[2] & bit_ABM) != 0;
    libdnml_caps.x86_sse4_2 = IsProcessorFeaturePresent(
        PF_SSE4_2_INSTRUCTIONS_AVAILABLE
    ); __cpuid(cpuInfo, 1);
    libdnml_caps.x86_rdrand = (cpuInfo[2] & bit_RDRAND) != 0;
#endif
}
static inline void ___x86_Pdetcaps_secex(void) {
#if __ARCH_X86_64__ && (ON_ANY_OTHER)
    unsigned int det_registers[4] = {0};
    // SSE4.2 Detection
    __sysv_get_cpuid(1, 0, det_registers);
    libdnml_caps.x86_rdrand = (det_registers[2] & bit_RDRAND) != 0;
#endif
}
static inline void ___x86_Pdetcaps_simdex(void) {
#if __ARCH_X86_64__ && (ON_ANY_OTHER)
    unsigned int det_registers[4] = {0};
    // SSE4.2 Detection
    __sysv_get_cpuid(1, 0, det_registers);
    libdnml_caps.x86_sse4_2 = (det_registers[2] & bit_SSE42) != 0;
#endif
}
static inline void ___x86_Pdetcaps_bitex(void) {
#if __ARCH_X86_64__ && (ON_ANY_OTHER)
    unsigned int det_registers[4] = {0};
    unsigned int max_instruct = 0;
    max_instruct = __sysv_cpu_maxl();
    // ABM Detection
    if (max_instruct >= 0x80000001) {
        __sysv_get_cpuid(0x80000001, 0, det_registers);
        libdnml_caps.x86_abm = (det_registers[2] & bit_ABM) != 0;
    } else libdnml_caps.x86_abm = 0;
    if (max_instruct >= 7) {
        __sysv_get_cpuid(7, 0, det_registers);
        libdnml_caps.x86_bmi1 = (det_registers[1] & bit_BMI1) != 0;
    } else libdnml_caps.x86_bmi1 = 0;
#endif
}
void __DNML_DETX64_HWCAPS(void) {
// COMPILER INTRINSICS --- GCC / Clang
#if __ARCH_X86_64__
#if __compiler_clang || __compiler_gcc
    libdnml_caps.x86_sse4_2 = __builtin_cpu_supports("sse4.2");
    libdnml_caps.x86_bmi1 = __builtin_cpu_supports("bmi");
    libdnml_caps.x86_abm = __builtin_cpu_supports("abm");
#elif __compiler_msvc || defined(_WIN32)
    ___x86_WIN64_detcaps();
#elif __ABI_X64_SYSV__ // System-V - Any other OS
    ___x86_Pdetcaps_bitex();
    ___x86_Pdetcaps_secex();
    ___x86_Pdetcaps_simdex();
#else // BARE METAL - Unknown ABI
    libdnml_caps.x86_abm = _DNML_BARE_X86_ABM;
    libdnml_caps.x86_bmi1 = _DNML_BARE_X86_BMI1;
    libdnml_caps.x86_sse4_2 = _DNML_BARE_X86_SSE4_2;
#endif
#endif
}


// ARM64 -- detections
void __DNML_DETARM64_HWCAPS(void) {}

// Detection main + dispatch
void _libdnml_detect_hwcaps(void) {
    #if __ARCH_X86_64__
        __DNML_DETX64_HWCAPS();
    #elif __ARCH_ARM64__
        __DNML_DETARM64_HWCAPS();
    #elif __ARCH_RVI64__
        __DNML_DETRV64_HWCAPS();
    #endif
}