#ifndef _libdnml_base
#define _libdnml_base


#include <include.h>
#include <dnml_sys/sys.h>
#include "intrinsics/intrinsics.h"
#include "dynamol/bigint/bigInt_func.h"
#include "drypto/crypt_int/cryptInt_func.h"

static volatile uint8_t _libinit = 0;

//* ---------- LIBRARY INITIALIZATION FUNCTIONS ---------- *//
// Library Initialization
static inline dnml_status _libdnml_init(void) {
    if (_libinit) return DNML_LIB_INISUCCESS;
    _libdnml_detect_hwcaps();
#if __compiler_clang || __compiler_gcc || __compiler_msvc
    // Compiler built-ins cover most arithmetic/bitwise optimizations.
    // Hardware RNG and halt hooks still require explicit initialization.
    _libinit = 1;
    return DNML_LIB_INISUCCESS;
#else
    // Unknown compiler
    // --- Performance-based Intrinsics Dispatch ---
    _libdnml_fill_garith(); _libdnml_fill_gbitops();
    _libdnml_fill_gmarith(); _libdnml_fill_ghw();
    // --- Cryptography-based Intrinsics Dispatch ---
    _libdnml_fill_crt_gcmp(); _libdnml_fill_crt_gsec();
    _libdnml_fill_crt_gbitops(); _libdnml_fill_crt_garith();
    _libdnml_fill_crt_galg();
    // --------- MODULES INITIALIZATION ---------
    if (_init_dynamol_bigint() == DNML_ALLOC_OOM) return DNML_ALLOC_OOM; // dynamol/bigint
    if (_init_drypto_crint() == DNML_ALLOC_OOM) return DNML_ALLOC_OOM; // drypto/crypt_int
    _libinit = 1; return DNML_LIB_INISUCCESS;
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