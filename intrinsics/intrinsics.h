#ifndef __DNML_INTRINSICS_H
#define __DNML_INTRINSICS_H



#include <math.h>
#include <stdint.h>
#include <system/sys.h>
#include "../sconfigs/settings/numeric_config.h"
#include "../sconfigs/settings/settings.h"

#include "arm64/_arm64_conn.h"
#include "x86_64/_x86_conn.h"
#include "risc-v64/_rv64_conn.h"
#include "zvanillc/_vanillc_conn.h"

#if __OS_MACOS__ || __OS_IOS__ || __OS_BSD__
    #include <sys/random.h>
#endif


#define min(x, y) ( ((x) < (y)) ? (x) : (y) )
#define max(x, y) ( ((x) > (y)) ? (x) : (y) )

// //* ---------------------------------------------------------------------------- *//
// //*                              IFUNC FUNCTION TABLES                           *//
// //* ---------------------------------------------------------------------------- *//
typedef struct {
    uint64_t     (*clz64)(uint64_t x);
    uint64_t     (*ctz64)(uint64_t x);
    uint64_t    (*bswap64)(uint64_t x);
    uint64_t     (*pcnt64)(uint64_t x);
} _BITOPS_FTABLE;
typedef struct {
    uint64_t (*add64c)(uint64_t a, uint64_t b, uint8_t *carry);
    uint64_t (*sub64b)(uint64_t a, uint64_t b, uint8_t *borrow);
    uint64_t (*wmul128)(uint64_t a, uint64_t b, uint64_t *hi);
    uint64_t (*wdiv128)(
        uint64_t lo, uint64_t hi, uint64_t div, 
        uint64_t *rhat
    );
} _ARITH_FTABLE;
typedef struct {
    uint64_t (*modinv64)(uint64_t x);
} _MARITH_FTABLE;
typedef struct {
} _ALG_FTABLE;

static _BITOPS_FTABLE _libdnml_gbitops_ftable;
static _ARITH_FTABLE _libdnml_garith_ftable;
static _MARITH_FTABLE _libdnml_gmarith_ftable;
static _ALG_FTABLE _libdnml_galg_ftable;

static inline void _libdnml_fill_gbitops(void) {
#if __ARCH_X86_64__
// CLZ - Detect ABM (Advanced Bit Manipulation)
_libdnml_gbitops_ftable.clz64 = (libdnml_caps.x86_abm) ? _x86_clz64e : _x86_clz64s;
// CTZ - Detect BMI1 (Bit Manipulation Instructions 1)
_libdnml_gbitops_ftable.clz64 = (libdnml_caps.x86_bmi1) ? _x86_ctz64e : _x86_ctz64s
// POPCNT - Detect SSE4.2
_libdnml_fill_gbitops.pcnt64 = (libdnml_caps.x86_sse4_2) ? _x86_pcnt64e : _cintrin_pcnt64;
_libdnml_gbitops_ftable.bswap64 = _x86_bswap64;
#elif __ARCH_ARM64__
_libdnml_gbitops_ftable.clz64 = _arm64_clz64;
_libdnml_gbitops_ftable.ctz64 = _arm64_ctz64;
_libdnml_gbitops_ftable.bswap64 = _arm64_bswap64;
_libdnml_gbitops_ftable.pcnt64 = _arm64_pcnt64;
#elif __ARCH_RVI64__
if (libdnml_caps.rv64_zbb) {
    _libdnml_gbitops_ftable.clz64 = _rv64_clz64;
    _libdnml_gbitops_ftable.ctz64 = _rv64_ctz64;
    _libdnml_gbitops_ftable.bswap64 = _rv64_bswap64;
    _libdnml_gbitops_ftable.pcnt64 = _rv64_pcnt64;
} else {
    _libdnml_gbitops_ftable.clz64 = _cintrin_clz64;
    _libdnml_gbitops_ftable.ctz64 = _cintrin_ctz64;
    _libdnml_gbitops_ftable.bswap64 = _cintrin_bswap64;
    _libdnml_gbitops_ftable.pcnt64 = _cintrin_pcnt64;
}
#else
_libdnml_gbitops_ftable.clz64 = _cintrin_clz64;
_libdnml_gbitops_ftable.ctz64 = _cintrin_ctz64;
_libdnml_gbitops_ftable.bswap64 = _cintrin_bswap64;
_libdnml_gbitops_ftable.pcnt64 = _cintrin_pcnt64;
#endif
}
static inline void _libdnml_fill_garith(void) {
#if __ARCH_X86_64__
_libdnml_garith_ftable.add64c = _x86_add64c;
_libdnml_garith_ftable.sub64b = _x86_sub64b;
_libdnml_garith_ftable.wmul128 = _x86_wmul128;
_libdnml_garith_ftable.wdiv128 = _x86_wdiv128;
#elif __ARCH_ARM64__
_libdnml_garith_ftable.add64c = _arm64_add64c;
_libdnml_garith_ftable.sub64b = _arm64_sub64b;
_libdnml_garith_ftable.wmul128 = _arm64_wmul128;
_libdnml_garith_ftable.wdiv128 = _cintrin_wdiv128;
#elif __ARCH_RVI64__
_libdnml_garith_ftable.add64c = _rv64_add64c;
_libdnml_garith_ftable.sub64b = _rv64_sub64b;
_libdnml_garith_ftable.wmul128 = _rv64_wmul128;
_libdnml_garith_ftable.wdiv128 = _cintrin_wdiv128;
#else
_libdnml_garith_ftable.add64c = _cintrin_add64c;
_libdnml_garith_ftable.sub64b = _cintrin_sub64b;
_libdnml_garith_ftable.wmul128 = _cintrin_wmul128;
_libdnml_garith_ftable.wdiv128 = _cintrin_wdiv128;
#endif
}
static inline void _libdnml_fill_gmarith(void) {
#if __ARCH_X86_64__
_libdnml_gmarith_ftable.modinv64 = _x86_modinv64;
#elif __ARCH_ARM64__
_libdnml_gmarith_ftable.modinv64 = _arm64_modinv64;
#elif __ARCH_RVI64__
_libdnml_gmarith_ftable.modinv64 = _rv64_modinv64;
#else
_libdnml_gmarith_ftable.modinv64 = _cintrin_modinv64;
#endif
}
static inline void _libdnml_fill_galg(void) {
#if __ARCH_X86_64__
#elif __ARCH_ARM64__
#elif __ARCH_RVI64__
#else
#endif
}


//* --------------------------------------------------------------------------------------- *//
//*                                    SINGLE-LIMB ARITHMETIC                               *//
//* --------------------------------------------------------------------------------------- *//
static inline uint64_t _cintrin_divwrap(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rhat) {
    *rhat = _cintrin_clz64(div);
    return _cintrin_wdiv128(lo, hi, div, rhat);
}
static inline uint64_t __ADD_UI64__(uint64_t a, uint64_t b, uint8_t *carry) {
    *carry = (*carry) ? 1 : 0;
    #if __compiler_clang // Clang --> Always used
        return __builtin_addcll(a, b, *carry, (unsigned long long*)carry);
    #elif __compiler_gcc // GCC --> Always used
        uint64_t sum;
        *carry = __builtin_uaddll_overflow(a, b, &sum);
        return sum;
    #elif __compiler_msvc // MSVC --> Only on x86_64
        uint64_t sum;
        *carry = _addcarry_u64((*carry) ? 1 : 0, a, b,  &sum)
        return sum;
    #else
        return _libdnml_garith_ftable.add64c(a, b, carry);
    #endif
}
static inline uint64_t __SUB_UI64__(uint64_t a, uint64_t b, uint8_t *borrow) {
    *borrow = (*borrow) ? 1 : 0;
    #if (__compiler_gcc || __compiler_clang) 
        // Clang / GCC --> Always used
        uint64_t diff;
        *borrow =  __builtin_sub_overflow(a, b, &diff);
        return diff;
    #elif __compiler_msvc // MSVC --> Only on x86_64
        uint64_t diff;
        *borrow = _subborrow_u64((*borrow) ? 1 : 0, a, b, &diff);
        return diff;
    #else
        return _libdnml_garith_ftable.sub64b(a, b, borrow);
    #endif
}
static inline uint64_t __MUL_UI64__(uint64_t a, uint64_t b, uint64_t *hi) {
    #if __HAS_int128__ // GCC / Clang --> ALWAYS USED
        uint128 res = ((uint128)a) * ((uint128)b);
        *hi = (uint64_t)(res >> BITS_IN_UINT64_T);
        return (uint64_t)res;
    #elif __compiler_msvc // MSVC - Only on x86/ARM64
        return _umul128(a, b, hi);
    #else
        return _libdnml_garith_ftable.wmul128(a, b, hi);
    #endif
}
static inline uint64_t __DIV_HELPER_UI64__(
    uint64_t lo, uint64_t hi, uint64_t div, 
    uint64_t *rhat
) {
    if (hi >= div) { 
        if (_DNML_DEBUG_MODE) { 
            fputs("Division Error - Can't contain full quotient in 64 bit", stderr);
            abort();
        } else { *rhat = 0; return 0; }
    } 
    #if __HAS_int128__ // GCC / Clang
        uint128 dividend = ((uint128)(hi) << BITS_IN_UINT64_T) | lo; 
        *rhat = (uint64_t)(dividend % div);
        return (uint64_t)(dividend / div);
    #elif __compiler_msvc // MSVC
        return _udiv128(hi, lo, div, rhat);
    #else // Unknown Compiler
        #if !(__ARCH_X86_64__)
            *rhat = _libdnml_gbitops_ftable.clz64(div);
        #endif
        return _libdnml_garith_ftable.wdiv128(lo, hi, div, rhat);
    #endif
}
static inline uint64_t __MODINV_UI64__(uint64_t x) { 
    if (!(x & 1)) return 0;
    return _libdnml_gmarith_ftable.modinv64(x);
}
static inline uint64_t __MODMUL_UI64__(uint64_t a, uint64_t b, uint64_t mod) {
    uint64_t hi, lo;
    lo = __MUL_UI64__(a, b, &hi);
    #if __HAS_int128__
        return (uint64_t)(((unsigned __int128)hi << 64 | lo) % mod);
    #else
        if (hi == 0) return lo % mod;
        uint64_t rem = hi % mod;
        for (uinit8_t i = 63; i >= 0; --i) {
            rem = (rem >= mod - rem) ?
                    rem - (mod - rem) :
                    rem + rem;
            if ((lo >> i) & 1) {
                ++rem;
                if (rem >= mod) rem -= mod;
            }
        }
    #endif
}
static inline uint64_t __MODEXP_UI64__(uint64_t base, uint64_t exp, uint64_t mod) {
    if (mod == 1) return 0;
    if (exp == 0) return 1;
    if (exp == 1) return base;
    base %= mod;
    uint64_t res = 1;
    while (exp > 0) {
        if (exp & 1) res = __MODMUL_UI64__(res, base, mod);
        base = __MODMUL_UI64__(base, base, mod);
        exp >>= 1;
    } return res;
}


//* --------------------------------------------------------------------------------------- *//
//*                                GENERAL MATHEMATICAL UTILITIES                           *//
//* --------------------------------------------------------------------------------------- *//
static inline uint8_t __SAFE_EXP__(uint64_t base, uint64_t exp) {
    if (exp == 0) return 1;
    if (exp == 1) return 1;
    if (exp == 2) return (base <= (1ULL << 32) - 1);
    return (double)exp * log2((double)base) < (double)(BITS_IN_UINT64_T);
}
static inline uint8_t __IS_2POW__(uint64_t x) { return (x) && !(x & (x - 1));  }
static inline uint8_t __CLZ_UI64__(uint64_t x) {
    if (!x) return BITS_IN_UINT64_T;
    // The actual code
    #if (__compiler_gcc || __compiler_clang)
        return __builtin_clzll(x);
    #elif __compiler_msvc
        return _CountLeadingZeros64(x);
    #else
        return _libdnml_gbitops_ftable.clz64(x);
    #endif
}
static inline uint8_t __CTZ_UI64__(uint64_t x) {
    if (!x) return BITS_IN_UINT64_T;
    // The actual code
    #if (__compiler_gcc || __compiler_clang) 
        return __builtin_ctzll(x);
    #elif __compilter_msvc
        return _CountTrailingZeros64(x);
    #else
        return _libdnml_gbitops_ftable.ctz64(x);
    #endif
}
static inline uint64_t __BSWAP_UI64__(uint64_t x) {
    if (!x || x == UINT64_MAX) return x;
    #if (__compiler_gcc || __compiler_clang)
        return __builtin_bswap64(x);
    #elif __compiler_msvc
        return _byteswap_uint64(x);
    #else
        return _libdnml_gbitops_ftable.bswap64(x);
    #endif
}
static inline uint64_t __PCNT_UI64__(uint64_t x) { 
    if (!x) return 0; 
    else if (x== UINT64_MAX) return BITS_IN_UINT64_T;
    #if (__compiler_gcc || __compiler_clang)
        return __builtin_popcountll(x);
    #elif __compiler_msvc
        return __popcnt64(x);
    #else
        return _libdnml_fill_gbitops.pcnt64(x);
    #endif
}


//* --------------------------------------------------------------------------------------- *//
//*                                   GENERAL MEMORY UTILITIES                              *//
//* --------------------------------------------------------------------------------------- *//
static inline __MEMWIPE_STD__(void *buf, size_t len) {}
static inline __MEMWIPE_STRICT__(void *buf, size_t len) {}




//* --------------------------------------------------------------------------------------- *//
//*                                  CRYPTOGRAPHICAL OPERATIONS                             *//
//* --------------------------------------------------------------------------------------- *//
// Cryptographical Helpers
static inline __GET_ENTROPY_UNBL(void *buf, size_t len) {}
static inline __CPU_RDSEED_RETRY(void *buf, size_t len) {}
static inline __jitter_harvest(void);
// Main Operations
static inline void __GET_ENTROPY_FAST(void* buf, size_t len) {
    // 1. Fast Hardware-accelerated Entropy collection
    if (libdnml_caps.x86_rdrand) // __CPU_RDRAND(buf, len);

    // 2. Non-blocking, OS Fall-back
    if (!__GET_ENTROPY_UNBL(buf, len)) return;

    // 3. Last Reosrt: Jitter Harvesting
    uint64_t* ptr = (uint64_t*)buf;
    for (size_t i = 0; i < (len >> 3); ++i) {
        ptr[i] = __jitter_harvest() ^ (uintptr_t)&ptr[i];
    }
}
static inline void __GET_ENTROPY_STD(void *buf, size_t len) {
    #if __OS_LINUX__
        getrandom(buf, len, 0);
    #elif __OS_WIN64__
        BCryptGenRandom(
            NULL, buf, len,
            BCRYPT_USE_SYSTE_PREFERRED_RNG
        );
    #else
        getentropy(buf, len);
    #endif
}
static inline void __GET_ENTROPY_PQC(void *buf, size_t len) {
    uint8_t pool_os[len];
    uint8_t pool_hw[len];

    __CPU_RDSEED_RETRY(pool_hw, len);
    __GET_ENTROPY_STD(pool_os, len);
    for (size_t i = 0; i < len; ++i) {
        ((uint8_t*)buf)[i] = pool_os[i] ^ pool_hw[i];
    } __MEMWIPE_STRICT__(pool_os, len);
    __MEMWIPE_STRICT__(pool_hw, len);
}


#endif
