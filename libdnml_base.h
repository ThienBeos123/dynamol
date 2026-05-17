#ifndef _libdnml_base
#define _libdnml_base


#include "system/__arch.h"
#include "system/__compiler.h"
#include "system/__hwcaps.h"
#include "sconfigs/settings.h"
#include "intrinsics/intrinsics.h"
#include <stdint.h>

static volatile uint8_t _libinit = 0;

//* ---------- LIBRARY INITIALIZATION FUNCTIONS ---------- *//
// Library Initialization
static inline void _libdnml_init(void) {
#if __compiler_clang || __compiler_gcc || __compiler_msvc
    // Every "hardware-specific features"
    // are already supported on Clang/GCC/MSVC
    _libinit = 1;
    return;
#else
    // Unknown compiler
    // ---> Initialize DNML's handwritten intrinsics
    if (_libinit) return;
    _dnml_hwcaps libdnml_caps = {0};
    _libdnml_detect_hwcaps();
    _libdnml_fill_galg();
    _libdnml_fill_garith();
    _libdnml_fill_gbitops();
    _libdnml_fill_gmarith();
    _libinit = 1;
#endif
}
static inline uint8_t _libdnml_cinit(void) { return _libinit; }
static inline void _libdnml_cleanup(void) {}



//* ---------- AUTOMATIC CHECK MACROS ---------- *//
// Library Initialization Auto-Check
#if __compiler_clang || __compiler_gcc || __compiler_msvc
    #define _LIBDNML_AUTOCHECK()
#elif _DNML_DEBUG_MODE
    #define _LIBDNML_AUTOCHECK() \
        do { if (!_libdnml_cinit()) { \
            fprintf(stderr, "[libdnml] dnml_init() not called\n"); \
            abort(); \
        }} while(0) 
#else
    #define _LIBDNML_AUTOCHECK()
#endif

// Library Automatic Initialization - Compiler Support only
#if __compiler_clang || __compiler_gcc
    __attribute__((constructor))
    static void _libdnml_gcc_ainit(void) { _libdnml_init(); }
#elif __compiler_msvc
    static void _libdnml_msvc_ainit(void);
    #pragma section(".CRT$XCU", read)
    __declspec(allocate(".CRT$XCU"))
    static void(*_pdnml_init)(void) = _libdnml_msvc_ainit;
    static void _libdnml_msvc_ainit(void) { _libdnml_init(); }
#endif



#endif