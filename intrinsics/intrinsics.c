#include "intrinsics.h"



//* --------------------------------------------------------------------------------------- *//
//*                                    SINGLE-LIMB ARITHMETIC                               *//
//* --------------------------------------------------------------------------------------- *//
uint64_t __ADD_UI64__(uint64_t a, uint64_t b, uint8_t *carry) {
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
        return (*_libdnml_garith_ftable.add64c)(a, b, carry);
    #endif
}
uint64_t __SUB_UI64__(uint64_t a, uint64_t b, uint8_t *borrow) {
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
        return (*_libdnml_garith_ftable.sub64b)(a, b, borrow);
    #endif
}
uint64_t __MUL_UI64__(uint64_t a, uint64_t b, uint64_t *hi) {
    #if __HAS_int128__ // GCC / Clang --> ALWAYS USED
        uint128 res = ((uint128)a) * ((uint128)b);
        *hi = (uint64_t)(res >> U64_BITS);
        return (uint64_t)res;
    #elif __compiler_msvc // MSVC - Only on x86/ARM64
        return _umul128(a, b, hi);
    #else
        return (*_libdnml_garith_ftable.wmul128)(a, b, hi);
    #endif
}
uint64_t __DIV_HELPER_UI64__(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rhat, uint8_t *overflowed) {
    if (hi >= div) { 
        if (_DNML_DEBUG_MODE) { 
            fputs("Division Error - Can't contain full quotient in 64 bit", stderr);
            abort();
        } else { *rhat = 0; return 0; }
    } 
    #if __HAS_int128__ // GCC / Clang
        uint128 dividend = ((uint128)(hi) << U64_BITS) | lo; 
        *rhat = (uint64_t)(dividend % div);
        return (uint64_t)(dividend / div);
    #elif __compiler_msvc // MSVC
        return _udiv128(hi, lo, div, rhat);
    #else // Unknown Compiler
        #if !(__ARCH_X86_64__)
            *rhat = _libdnml_gbitops_ftable.clz64(div);
        #endif
        return (*_libdnml_garith_ftable.wdiv128)(lo, hi, div, rhat, overflowed);
    #endif
}
uint64_t __MODINV_UI64__(uint64_t x) { 
    if (!(x & 1)) return 0;
    return (*_libdnml_gmarith_ftable.modinv64)(x);
}
uint64_t __MODMUL_UI64__(uint64_t a, uint64_t b, uint64_t mod) {
    uint64_t hi, lo;
    lo = __MUL_UI64__(a, b, &hi);
    #if __HAS_int128__
        return (uint64_t)(((unsigned __int128)hi << 64 | lo) % mod);
    #else
        if (hi == 0) return lo % mod;
        uint64_t rem = hi % mod;
        for (uinit8_t i = 63; i != -1; --i) {
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
uint64_t __MODEXP_UI64__(uint64_t base, uint64_t exp, uint64_t mod) {
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
uint8_t __SAFE_EXP__(uint64_t base, uint64_t exp) {
    if (exp == 0) return 1;
    if (exp == 1) return 1;
    if (exp == 2) return (base <= (1ULL << 32) - 1);
    return (double)exp * log2((double)base) < (double)(U64_BITS);
}
uint8_t __IS_2POW__(uint64_t x) { return (x) && !(x & (x - 1));  }
uint8_t __CLZ_UI64__(uint64_t x) {
    if (!x) return U64_BITS;
    // The actual code
    #if (__compiler_gcc || __compiler_clang)
        return __builtin_clzll(x);
    #elif __compiler_msvc
        return _CountLeadingZeros64(x);
    #else
        return (*_libdnml_gbitops_ftable.clz64)(x);
    #endif
}
uint8_t __CTZ_UI64__(uint64_t x) {
    if (!x) return U64_BITS;
    // The actual code
    #if (__compiler_gcc || __compiler_clang) 
        return __builtin_ctzll(x);
    #elif __compilter_msvc
        return _CountTrailingZeros64(x);
    #else
        return (*_libdnml_gbitops_ftable.ctz64)(x);
    #endif
}
uint64_t __BSWAP_UI64__(uint64_t x) {
    if (!x || x == UINT64_MAX) return x;
    #if (__compiler_gcc || __compiler_clang)
        return __builtin_bswap64(x);
    #elif __compiler_msvc
        return _byteswap_uint64(x);
    #else
        return (*_libdnml_gbitops_ftable.bswap64)(x);
    #endif
}
uint8_t __PCNT_UI64__(uint64_t x) { 
    if (!x) return 0; 
    else if (x== UINT64_MAX) return U64_BITS;
    #if (__compiler_gcc || __compiler_clang)
        return __builtin_popcountll(x);
    #elif __compiler_msvc
        return __popcnt64(x);
    #else
        return (*_libdnml_gbitops_ftable.pcnt64)(x);
    #endif
}




//* --------------------------------------------------------------------------------------- *//
//*                                HARDWARE INTERACTION UTILITIES                           *//
//* --------------------------------------------------------------------------------------- *//
static void __MEMCPY_STRICT__(void *buf, const void *src,  size_t len) {}
static void __MEMWIPE_STRICT__(void *buf, size_t len) {}
/* Security-Extension Hardware Functionalities */
int __CPU_DBRG_SEED__(void *buf, size_t len, int retry_max, bool crypt, size_t *written) {
    if (_libdnml_ghw_ftable.hw_drbg == _cintrin_shallow_rng) { *written = 0; return -1; }
    unsigned char *p = (unsigned char *)buf;
    size_t rem = len; int retry_cnt = 0;
    // Filling in 64-bit chunks / 8-byte chunks
    while (rem >= 8) {
        uint64_t block; int err = 0;
        while (1) {
            block = (*_libdnml_ghw_ftable.hw_drbg)(&err);
            if (err == 2) return -2; // FATAL ERROR IN HARDWARE (for RV64)
            if (retry_cnt > retry_max) return -1;
            if (!err) { *(uint64_t*)p = block; p += 8; rem -= 8; break; } 
            ++retry_cnt; (*_libdnml_ghw_ftable.hw_shalt)();
        } (*written) += 8; // 8 bytes filled
    }
    // Handle the remaining odd blocks
    if (rem) {
        uint64_t block; int err = 0;
        while (1) {
            block = (*_libdnml_ghw_ftable.hw_drbg)(&err);
            if (err == 2) return -2; // FATAL ERROR IN HARDWARE (for RV64)
            if (retry_cnt > retry_max) return -1;
            if (!err) { 
                if (!crypt) memcpy(p, &block, rem);
                else __MEMCPY_STRICT__(p, &block, rem);
                break; 
            } ++retry_cnt; (*_libdnml_ghw_ftable.hw_shalt)();
        } (*written) += rem; // The remaining are filled in
    } return 0;
}
int __CPU_TRNG_SEED__(void *buf, size_t len, int retry_max, bool crypt, size_t *written) {
    if (_libdnml_ghw_ftable.hw_trng == _cintrin_shallow_rng) { *written = 0; return -1; }
    unsigned char *p = (unsigned char *)buf;
    size_t rem = len; int retry_cnt = 0;
    // Filling in 64-bit chunks / 8-byte chunks
    while (rem >= 8) { 
        uint64_t block; int err = 0;
        while (1) {
            block = (*_libdnml_ghw_ftable.hw_trng)(&err);
            if (err == 2) return -2; // FATAL ERROR IN HARDWARE (for RV64)
            if (retry_cnt > retry_max) return -1;
            if (!err) { *(uint64_t*)p = block; p += 8; rem -= 8; break; } 
            ++retry_cnt; (*_libdnml_ghw_ftable.hw_halt)();
        } (*written) += 8; // 8 bytes filled 
    }
    // Handle the remaining odd blocks
    if (rem) {
        uint64_t block; int err = 0;
        while (1) {
            block = (*_libdnml_ghw_ftable.hw_trng)(&err);
            if (err == 2) return -2; // FATAL ERROR IN HARDWARE (for RV64)
            if (retry_cnt > retry_max) return -1;
            if (!err) {
                if (!crypt) memcpy(p, &block, rem);
                else __MEMCPY_STRICT__(p, &block, rem);
                break;
            } ++retry_cnt; (*_libdnml_ghw_ftable.hw_halt)();
        } (*written) += rem; // The remaining are filled in
    } return 0;
}
/* Hardware-Interactive Functionalities */
void __CPU_FULL_HALT__(void) { (*_libdnml_ghw_ftable.hw_halt); }
void __CPU_SHALLOW_HALT__(void) { (*_libdnml_ghw_ftable.hw_shalt); }



//* --------------------------------------------------------------------------------------- *//
//*                                  CRYPTOGRAPHICAL OPERATIONS                             *//
//* --------------------------------------------------------------------------------------- *//
// Helper functions
static uint64_t ___get_time_stamp(void) {
#if __ARCH_X86_64__
    return _rdtsc();
#else
    struct timespec ts; clock_gettime(CLOCK_MONOTONIC, &ts);
    return ((uint64_t)ts.tv_sec * 1000000000ULL) + ts.tv_nsec;
#endif
}
// Platform-specific Helpers
static int ___ENTROPY_URANDOM(void *buf, size_t len, int retry_max, size_t *written) { // Unix Portable fallback
#if __OS_LINUX__ || __OS_BSD__ || __OS_MACOS__ || __OS_IOS__
    int fd, retry_cnt = 0; ssize_t readed = 0;
    unsigned char *p = (unsigned char*)buf;
    fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) return -1;
    while (readed < (ssize_t)len) {
        ssize_t n = read(fd, p + readed, len - readed);
        if (n < 0) {
            // Signal interruption --> Retry
            if (errno = EINTR) {
                if (retry_cnt > retry_max) return -1;
                ++retry_cnt; continue;
            }
            // True hard fail
            int saved_errno = errno;
            close(fd); errno = saved_errno;
            return -1;
        } if (!n) { close(fd); errno = EIO; return -1; }
        readed += n; (*written) += n;
    } close(fd);
    return 0;
#else
    return -1;
#endif
}
static int ___ENTROPY_LINUX(void *buf, size_t len, int retry_max, size_t *written) { // Linux
#if __OS_LINUX__
    unsigned char *cbuf = (unsigned char*)buf;
    size_t remaining = len; int retry_cnt = 0;

    while (remaining) {
        ssize_t ret = getrandom(cbuf, remaining, 0);
        if (ret < 0) {
            if (errno == ENOSYS) return ___ENTROPY_URANDOM(buf, len, retry_max, *written);
            if (errno == EINTR) {
                if (retry_cnt > retry_max) return -1;
                ++retry_cnt; continue; // Signal blocking - Retry
            }
            return -1; // Another error
        }
        if (!ret) { errno = EIO; return -1; }
        cbuf += ret; (*written) += ret; remaining -= ret;
    }
    return 0;
#else
    return -1;
#endif
}
static int ___ENTROPY_DARWIN(void *buf, size_t len, int retry_max, size_t *written) { // MacOS and BSDs (Open, Net, Free)
#if __OS_MACOS__ || __OS_BSD__
    unsigned char *ucbuf = (unsigned char *)buf;
    size_t remain = len; int fallbacked = 0;

    while (remain) {
        // Process in chunks (getentropy has 256 byte limit)
        size_t n = (remain > 256) ? 256 : remain;
        int ret = getentropy(ucbuf, n);
        // Success --> Continue read
        if (!ret) { ucbuf += n; (*written) += n; remain -= n; continue; }
        // Failed
        if (fallbacked) return -1; // Fallbacked already, mostly for safety
        if (errno == ENOSYS || errno == EPERM || errno == EACCES) {
            // ENOSYS: getentropy() not availabled (older versions)
            // EPERM: Permssion denied (Sandbox restrictions)
            // EACCES: Access denied (Sandbox restrictions)
            return ___ENTROPY_URANDOM(buf, len, retry_max, written);
        } 
        return -1;
    }
    return 0;
#else
    return -1;
#endif
}
static int ___ENTROPY_IOS(void *buf, size_t len, int retry_max, size_t *written) { // iOS
#if __OS_IOS__
    unsigned char *ucbuf = (unsigned char*)buf;
    size_t rem = len;
    while (rem) {
        size_t n = (rem > 256) ? 256 : rem;
        int ret = getentropy(ucbuf, n);
        if (ret) return -1; // getentropy() failed --> unexpected error
        ucbuf += n; (*written) += n; rem -= n;
    }
    return 0;
#else
    return -1;
#endif
}
static int ___ENTROPY_WIN64(void *buf, size_t len, size_t *written) { // Windows (64 bit)
#if __OS_WIN64__
    NTSTATUS status;
    BCRYPT_ALG_HANDLE alg_handle = NULL;

    // Open RNG Algorithm Provider
    status = BCryptOpenAlgorithmProvider(
        &alg_handle, BCRYPT_RNG_ALGORITHM, // Uses RNG Algorithms
        NULL, 0 // Default provider + No flags
    ); if (!BCRYPT_SUCCESS(status)) { errno = EIO; return -1; }

    // Generate Random bytes
    status = BCryptGenRandom(
        alg_handle, // Algorithm Handle
        (unsigned char*)buf, // Output buffer
        (ULONG)len, 0 // Number of bytes to fill + No flags
    );

    // Cleanup
    BCryptCloseAlgorithmProvider(alg_handle, 0); // Close provider
    if (!BCRYPT_SUCCESS(status)) { errno = EIO; return -1; } // Check for failure
    *written = len; return 0; // Success (can't truly track BCryptGenRandom write)

#endif
}
// Cryptographical Helpers
static int __GET_ENTROPY_UNBL(void *buf, size_t len, int retry_max, size_t *written) {
#if __OS_LINUX__
    return ___ENTROPY_LINUX(buf, len, retry_max, written);
#elif __OS_IOS__
    return ___ENTROPY_IOS(buf, len, retry_max, written);
#elif __OS_MACOS__ || __OS_BSD__
    return ___ENTROPY_DARWIN(buf, len, retry_max, written);
#elif __OS_WIN64__
    return ___ENTROPY_WIN64(buf, len, retry_max, written);
#else
    return 1; // or return -1;
#endif
}
static uint64_t __jitter_harvest(int loop_count) {
    uint64_t entropy_pool = 0;
    uint64_t previous_time = ___get_time_stamp();
    // Looping/Iterating 128 times for fast and solid entropy
    for (int i = 0; i < loop_count; ++i) {
        uint64_t dummy = previous_time ^ i;
        dummy = (dummy << 7) | (dummy >> 57);

        uint64_t curr_time = ___get_time_stamp();
        uint64_t delta = curr_time - previous_time;
        previous_time = curr_time;

        entropy_pool ^= delta;
        entropy_pool *= UINT64_C(0xff51afd7ed558ccd);
        entropy_pool = (entropy_pool << 31) | (entropy_pool >> 33);
    }
    return entropy_pool;
}
// Main Operations
void __GET_ENTROPY_FAST(void* buf, size_t len) {
    // 1. Fast Hardware-accelerated Entropy collection
    size_t tmp = 0; // Not important
    if (!__CPU_DBRG_SEED__(buf, len, 32, false, &tmp)) return;
    if (!__GET_ENTROPY_UNBL(buf, len, 3, &tmp)) return; // 2. Non-blocking, OS Fall-back
    // 3. Last Reosrt: Light Jitter Harvesting
    uint64_t* ptr = (uint64_t*)buf;
    for (size_t i = 0; i < (len >> 3); ++i) {
        ptr[i] = __jitter_harvest(128) ^ (uintptr_t)&ptr[i];
    }
}
void __GET_ENTROPY_STD(void *buf, size_t len) {}

