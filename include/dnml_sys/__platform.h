#ifndef ___DNML_PLATFORM_H___
#define ___DNML_PLATFORM_H___

//* ======================== 1. Architecture Detection ======================== *//
/* Detecting x86_64 Architecture */
#if defined(__x86_64__) || defined(_M_X64)
    #define __ARCH_X86_64__ 1
#else
    #define __ARCH_X86_64__ 0
#endif
/* Detecting AARCH64 / ARM64 Architecture */
#if defined(__aarch64__) || defined(_M_ARM64)
    #define __ARCH_ARM64__ 1
#else
    #define __ARCH_ARM64__ 0
#endif
/* Detecting RISC-V64 / RVI64  Architecture */
#if defined(__riscv) && (__riscv_xlen == 64)
    #define __ARCH_RVI64__ 1
#else
    #define __ARCH_RVI64__ 0
#endif

/* Endian Detection */
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    #define __LITTLE_ENDIAN__ 1
#else
    #define __LITTLE_ENDIAN__ 0
#endif
/* Alighment Adjustment */
#if __ARCH_X86_64__ || __ARCH_ARM64__
    #define __BIGINT_DEFAULT_ALIGNMENT__ 32
#else
    #define __BIGINT_DEFAULT_ALIGNMENT__ 8
#endif


//* ======================== 2. Operating System Detection ======================== *//
// APLE ECOSYSTEM DETECTIONS
#if defined(__APPLE__)
    #define __OS_LINUX__    0
    #define __OS_WIN64__    0
    #define __OS_BSD__      0
    #define __OS_SOLARIS__  0
    #define __OS_HPUX__     0
    #include <TargetConditionals.h>
    // MacOS detections
    #if defined(TARGET_OS_OSX) && TARGET_OS_OSX
        #define __OS_MACOS__    1
        #define __OS_IOS__      0
    // iOS detections
    #elif defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
        #define __OS_MACOS__    0
        #define __OS_IOS__      1
    #else // Extremely Legacy versions
        #define __OS_MACOS__    1
        #define __OS_IOS__      0
    #endif

// LINUX DETECTION
#elif defined(__linux__) || defined(__linux) || defined(linux)
    #define __OS_MACOS__    0
    #define __OS_IOS__      0
    #define __OS_LINUX__    1
    #define __OS_WIN64__    0
    #define __OS_BSD__      0
    #define __OS_SOLARIS__  0
    #define __OS_HPUX__     0

// WINDOWS DETECTION
#elif defined(_WIN32) || defined(_WIN64) || defined(WIN32)
    #define __OS_MACOS__    0
    #define __OS_IOS__      0
    #define __OS_LINUX__    0
    #define __OS_WIN64__    1
    #define __OS_BSD__      0
    #define __OS_SOLARIS__  0
    #define __OS_HPUX__     0

// BSD Detections
#elif ( \
    defined(__FreeBSD__) || defined(__NetBSD__) \
 || defined(__OpenBSD__) || defined(__DragonFly__) \
 || defined(__bsdi__) \
)
    #define __OS_MACOS__    0
    #define __OS_IOS__      0
    #define __OS_LINUX__    0
    #define __OS_WIN64__    0
    #define __OS_BSD__      1
    #define __OS_SOLARIS__  0
    #define __OS_HPUX__     0

// Solaris Detections
#elif defined(__sun) || defined(__sun__) || defined(__SVR4)
    #define __OS_MACOS__    0
    #define __OS_IOS__      0
    #define __OS_LINUX__    0
    #define __OS_WIN64__    0
    #define __OS_BSD__      0
    #define __OS_SOLARIS__  1
    #define __OS_HPUX__     0

// HP-UX Detections
#elif defined(__hpux) || defined(__hpux__) || defined(hpux)
    #define __OS_MACOS__    0
    #define __OS_IOS__      0
    #define __OS_LINUX__    0
    #define __OS_WIN64__    0
    #define __OS_BSD__      0
    #define __OS_SOLARIS__  0
    #define __OS_HPUX__     1

// Unknown OS
#else
    #define __OS_MACOS__    0
    #define __OS_IOS__      0
    #define __OS_LINUX__    0
    #define __OS_WIN64__    0
    #define __OS_BSD__      0
    #define __OS_SOLARIS__  0
    #define __OS_HPUX__     0
#endif

// System Family Detections;
#if defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    #define __FAMILY_UNIX__     1
    #define __FAMILY_WINDOWS__  0
#elif defined(_WIN32) || defined(_WIN64) || defined(WIN32)
    #define __FAMILY_UNIX__     0
    #define __FAMILY_WINDOWS__  1
#else
    #define __FAMILY_UNIX__     0
    #define __FAMILY_WINDOWS__  0
#endif

//* =============== 3. ABI (APPLICATION BINARY INTERFACE) Detection =============== *//
// ARM64/AArch64 ABI
#if __ARCH_ARM64__
    #define __ABI_AARCH64__ 1
#else
    #define __ABI_AARCH64__ 0
#endif
// X86_64 ABIs
#if __ARCH_X86_64__
    // Win64 ABI
    #if defined(_WIN64)
        #define __ABI_X64_WIN64__   1
        #define __ABI_X64_SYSV__    0
    // System-V ABI
    #elif __FAMILY_UNIX__
        #define __ABI_X64_WIN64__   0
        #define __ABI_X64_SYSV__    1
    // Unknown ABI - Fallback to Vanilla C
    #else
        #define __ABI_X64_WIN64__   0
        #define __ABI_X64_SYSV__    0
    #endif
#else
    #define __ABI_X64_WIN64__ 0
    #define __ABI_X64_SYSV__ 0
#endif

// RISC-V64 / RV64 ABI
#if __ARCH_RVI64__
    #define __ABI_LP64__ 1
#else
    #define __ABI_LP64__ 0
#endif


#endif