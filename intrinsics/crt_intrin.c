#include "intrinsics.h"





//* --------------------------------------------------------------------------------------- *//
//*                                    SINGLE-LIMB ARITHMETIC                               *//
//* --------------------------------------------------------------------------------------- *//
uint64_t __CRT_ADD_U64__(uint64_t a, uint64_t b, uint8_t *carry) {
    *carry = !!(*carry);
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
        return (*_libdnml_crt_garith_ftable.add64c)(a, b, carry);
    #endif
}
uint64_t __CRT_SUB_U64__(uint64_t a, uint64_t b, uint8_t *borrow) {
    *borrow = !!(*borrow);
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
        return (*_libdnml_crt_garith_ftable.sub64b)(a, b, borrow);
    #endif
}
uint64_t __CRT_MUL_U64__(uint64_t a, uint64_t b, uint64_t *hi) {
    #if __HAS_int128__ // GCC / Clang --> ALWAYS USED
        uint128 res = ((uint128)a) * ((uint128)b);
        *hi = (uint64_t)(res >> U64_BITS);
        return (uint64_t)res;
    #elif __compiler_msvc // MSVC - Only on x86/ARM64
        return _umul128(a, b, hi);
    #else
        return (*_libdnml_crt_garith_ftable.wmul128)(a, b, hi);
    #endif
}
uint64_t __CRT_DIV_U128__(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rhat, uint8_t *overflowed) {
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
            *rhat = _libdnml_crt_gbitops_ftable.clz64(div);
        #endif
        return (*_libdnml_crt_garith_ftable.wdiv128)(lo, hi, div, rhat, overflowed);
    #endif
}






//* ----------------------------------------------------------------------------------- *//
//*                                    BITWISE OPERATIONS                               *//
//* ----------------------------------------------------------------------------------- *//
uint8_t __CRT_CLZ_UI64__(uint64_t x) {
    // The actual code
    #if (__compiler_gcc || __compiler_clang)
        return __builtin_clzll(x);
    #elif __compiler_msvc
        return _CountLeadingZeros64(x);
    #else
        return (*_libdnml_crt_gbitops_ftable.clz64)(x);
    #endif
}
uint8_t __CRT_CTZ_UI64__(uint64_t x) {
    // The actual code
    #if (__compiler_gcc || __compiler_clang) 
        return __builtin_ctzll(x);
    #elif __compilter_msvc
        return _CountTrailingZeros64(x);
    #else
        return (*_libdnml_crt_gbitops_ftable.ctz64)(x);
    #endif
}
uint64_t __CRT_BSWAP_UI64__(uint64_t x) {
    if (!x || !(x ^ UINT64_MAX)) return x;
    #if (__compiler_gcc || __compiler_clang)
        return __builtin_bswap64(x);
    #elif __compiler_msvc
        return _byteswap_uint64(x);
    #else
        return (*_libdnml_crt_gbitops_ftable.bswap64)(x);
    #endif
}
uint8_t __CRT_PCNT_UI64__(uint64_t x) { 
    if (!x) return 0; 
    else if (!(x ^ UINT64_MAX)) return U64_BITS;
    #if (__compiler_gcc || __compiler_clang)
        return __builtin_popcountll(x);
    #elif __compiler_msvc
        return __popcnt64(x);
    #else
        return (*_libdnml_crt_gbitops_ftable.pcnt64)(x);
    #endif
}




//* --------------------------------------------------------------------------------------- *//
//*                                        COMPARISON ARIT                                  *//
//* --------------------------------------------------------------------------------------- *//
// Standard U64 Comparisons
uint8_t _lib_crt_lt(uint64_t x, uint64_t y) {
    return (*_libdnml_crt_cmp_ftable.lt_func)(x, y);
}
uint8_t _lib_crt_gt(uint64_t x, uint64_t y) {
    return (*_libdnml_crt_cmp_ftable.gt_func)(x, y);
}
uint8_t _lib_crt_leq(uint64_t x, uint64_t y) {
    return (*_libdnml_crt_cmp_ftable.leq_func)(x, y);
}
uint8_t _lib_crt_geq(uint64_t x, uint64_t y) {
    return (*_libdnml_crt_cmp_ftable.geq_func)(x, y);
}
// SIGNED I64 Comparisons
uint8_t _lib_crt_lti64(int64_t x, int64_t y) {
    return (*_libdnml_crt_cmp_ftable.lti64_func)(x, y);
}
uint8_t _lib_crt_gti64(int64_t x, int64_t y) {
    return (*_libdnml_crt_cmp_ftable.gti64_func)(x, y);
}
uint8_t _lib_crt_leqi64(int64_t x, int64_t y) {
    return (*_libdnml_crt_cmp_ftable.leqi64_func)(x, y);
}
uint8_t _lib_crt_geqi64(int64_t x, int64_t y) {
    return (*_libdnml_crt_cmp_ftable.geqi64_func)(x, y);
}
// Equality
uint8_t _lib_crt_ispos(int64_t x) {
    return (*_libdnml_crt_cmp_ftable.is_pos)(x);
}
uint8_t _lib_crt_isneg(int64_t x) {
    return (*_libdnml_crt_cmp_ftable.is_neg)(x);
}
uint8_t _lib_crt_neq(uint64_t x, uint64_t y) {
    return (*_libdnml_crt_cmp_ftable.eq_func)(x, y);
}
uint8_t _lib_crt_eq(uint64_t x, uint64_t y) {
    return (*_libdnml_crt_cmp_ftable.neq_func)(x, y);
}
uint64_t _lib_crt_select(uint8_t cond, uint64_t a, uint64_t b) {
    #if __compiler_clang
        return __builtin_ct_select(cond, a, b);
    #else
        return (*_libdnml_crt_cmp_ftable.select_fn)(cond, a, b);
    #endif
}





//* ----------------------------------------------------------------------------------------- *//
//*                             GENERAL HARDWARE INTERACTION UTILITIES                        *//
//* ----------------------------------------------------------------------------------------- *//
typedef uint64_t (*rng_func_t)(int*);
typedef void (*halt_func_t)(void);
static int __internal_write_seed(
    void *buf, size_t len, int retry_max, 
    rng_func_t hw_rng, halt_func_t hw_halt, 
    size_t *written
) {
    uint8_t* p = (uint8_t*)buf;
    size_t rem = len, sum_len = (len + 7) / 8;
    size_t loop_limit = sum_len + (size_t)retry_max;
    int retry_cnt = 0; uint64_t dummy_sink = 0;

    // Global status tracks errors without branches:
    //  0 = Success
    // -1 = Retry limit exceeded or initialization failure
    // -2 = Fatal hardware DEAD fault triggered
    int global_status = 0; *written = 0;
    for (size_t i = 0; _lib_crt_lt(i, loop_limit); ++i) {
        int err = 0; uint64_t block = (hw_rng)(&err);
        // ------------ ERROR EVALUATION ------------
        uint8_t is_fatal = (_lib_crt_eq(err, 2));
        uint8_t is_wait = (_lib_crt_eq(err, 1));
        uint8_t is_ok = (_lib_crt_eq(err, 0));
        // If a fatal error occurs anywhere, permanently latch global_status to -2
        global_status = (global_status & ~is_fatal) | (-2 & is_fatal);
        

        // ------------ RETRIES TRACKING ------------
        // If retry_exceed && global_status != -2, latch to -1
        retry_cnt += (int)is_wait; uint8_t retry_exceed = (_lib_crt_gt(retry_cnt, retry_max));
        int is_not_fatal = (_lib_crt_neq(global_status, -2));
        global_status = global_status + ((-1 - global_status) & (retry_exceed & is_not_fatal));


        // ------------ WRITE EVALUATION & TRACKING ------------
        // Condition: we can still WRITE + the current read is CLEAN + no ERRORS
        uint64_t has_space = (rem); // rem > 0
        uint64_t exec_valid = (!global_status); // global_status == 0
        uint64_t write_enabled = has_space & is_ok & exec_valid;
        // Writing (if write disabled ---> fake write into dummy sink)
        size_t write_amounts = _lib_crt_select((rem >= 8), 8, rem);
        size_t active_bytes = write_amounts & (-(size_t)write_enabled);


        // ------------ FAULT MASKING & HIDING ------------
        // 1. Copy data into a local staging variable to guarantee 100% uniform code tracking
        // 2. We execute a full 8-byte transfer into a temporary buffer, then conditionally 
        //    commit bytes into the real destination stream to eliminate memcpy timing variances.
        uint64_t staging_val = block;
        uint8_t staging_bytes[8]; __INTERNAL_MEMCPY_STRICT__(staging_bytes, &staging_val, 8);
        // Iterate through an unconditional 8-byte lane loop to write bytes sequentially
        // This fully replaces variable-time memcpy calls or dynamic odd-block evaluations.
        for (size_t b = 0; _lib_crt_lt(b, 8); ++b) {
            uint8_t byte_active = (_lib_crt_lt(b, 8));
            uint8_t *dst_ptr = (uint8_t*)_lib_crt_select(
                byte_active, (uintptr_t)(p), (uintptr_t)(&dummy_sink)
            );
            *dst_ptr = staging_bytes[b];
            p += (1 & byte_active); // Advance if byte_active == TRUE
            /* ==== MANDATORY "PER-ITERATION" VARIABLE CLEANUP ==== */ // clang-format off
            byte_active = 0; dst_ptr = 0; // clang-format on
        }
        rem -= active_bytes; *written += active_bytes;
        (hw_halt)(); // ALWAYS DO A HALT TO ENSURE NO SIGNIFICANT TIMING VARIANCE
        /* ==== MANDATORY "PER-ITERATION" VARIABLE CLEANUP ==== */ // clang-format off
        err = 0; block = 0; is_fatal = 0; is_wait = 0; is_ok = 0; retry_exceed = 0; 
        has_space = 0; exec_valid = 0; write_enabled = 0; write_amounts = 0; active_bytes = 0; 
        staging_val = 0; __INTERNAL_MEMWIPE_STRICT__(staging_bytes, 8); // clang-format on
    }
    /* ==== MANDATORY "POST-OPERATION" VARIABLE CLEANUP ==== */ // clang-format off
    p = 0; rem = 0; sum_len = 0; loop_limit = 0;
    retry_cnt = 0; dummy_sink = 0; return global_status; // clang-format on
}
int __CPU_CSDBRG_SEED__(void *buf, size_t len, int retry_max, size_t *written) {
    if (_libdnml_crt_sec_ftable.hw_drbg == _cintrin_shallow_rng) { *written = 0; return -1; }
    return __internal_write_seed(
        buf, len, retry_max, _libdnml_crt_sec_ftable.hw_drbg,
        _libdnml_crt_sec_ftable.hw_halt, written
    );
}
int __CPU_CSTRNG_SEED__(void *buf, size_t len, int retry_max, size_t *written) {
    if (_libdnml_crt_sec_ftable.hw_trng == _cintrin_shallow_rng) { *written = 0; return -1; }
    return __internal_write_seed(
        buf, len, retry_max, _libdnml_crt_sec_ftable.hw_trng,
        _libdnml_crt_sec_ftable.hw_halt, written
    );
}




//* ----------------------------------------------------------------------------------------- *//
//*                                  RANDOM-GENERATION OPERATIONS                             *//
//* ----------------------------------------------------------------------------------------- *//
// Helper functions
static void __INTERNAL_MEMCPY_STRICT__(void *buf, const void *src, size_t len) {}
static void __INTERNAL_MEMWIPE_STRICT__(void *buf, size_t len) {}
static inline uint64_t ___get_time_stamp(void) {
#if __ARCH_X86_64__
    return _rdtsc();
#else
    struct timespec ts; clock_gettime(CLOCK_MONOTONIC, &ts);
    return ((uint64_t)ts.tv_sec * 1000000000ULL) + ts.tv_nsec;
#endif
}
// Platform-specific Helpers
static int ___ENTROPY_CSURANDOM(void *buf, size_t len, int retry_max, size_t *written) { // Unix Portable fallback
#if __OS_LINUX__ || __OS_BSD__ || __OS_MACOS__ || __OS_IOS__
    int retry_cnt = 0; ssize_t readed = 0;
    unsigned char *p = (unsigned char*)buf;
    uint64_t dummy_sink = 0; *written = 0;
    // 1. Open the file descriptor unconditionally
    int fd = open("/dev/urandom", O_RDONLY);
    int fd_failed = _lib_crt_isneg(fd);
    int global_status = _lib_crt_select(fd_failed, -1, 0);

    // 2. Define a rigid, deterministic loop bound to prevent infinite execution paths
    size_t loop_limit = len + (size_t)retry_max;
    for (size_t i = 0; _lib_crt_lt(i, loop_limit); ++i) {
        // Condition evaluation
        int processing_complete = _lib_crt_geqi64(readed, (ssize_t)len);
        int state_healthy = !global_status;
        int perform_real_read = state_healthy & !processing_complete;

        // 3. Address and Parameter Multiplexing (Branchless Selection)
        // If we shouldn't read, dynamically swap the target address to our safe dummy pad
        unsigned char *target_p = (unsigned char *)_lib_crt_select(
            perform_real_read, (uintptr_t)(p + readed), (uintptr_t)&dummy_sink
        ); size_t bytes_requested  = _lib_crt_select(perform_real_read, len - (size_t)readed, 0);
        ssize_t n = read(fd, target_p, bytes_requested);

        // 4. Constant-Time Error Evaluation
        int is_read_neg = _lib_crt_isneg(n);
        int is_read_zero = _lib_crt_eq(n, 0);
        int is_eintr = _lib_crt_eq(errno, EINTR);
        // Distinguish between soft signals (EINTR) and catastrophic faults (EIO, EBADF, etc.)
        int signal_interrupt = is_read_neg & is_eintr;
        int hard_fault = (is_read_neg & !is_eintr) | (is_read_zero & perform_real_read);

        // 5. Variable Metadata Updating
        // Status variable updates
        retry_cnt += _lib_crt_select(signal_interrupt & perform_real_read, 1, 0);
        int retry_exceeded = _lib_crt_gti64(retry_cnt, retry_max);
        int error_trigger = hard_fault | (signal_interrupt & retry_exceeded);
        global_status = _lib_crt_select(error_trigger & state_healthy, -1, global_status);
        // Byte write/read variable updates
        ssize_t active_bytes_read = _lib_crt_select(perform_real_read & !is_read_neg, n, 0);
        readed += active_bytes_read; (*written) += (size_t)active_bytes_read;
    }
    int safe_fd = _lib_crt_select(fd_failed, -1, fd);
    close(safe_fd); return global_status;
#else
    return -1;
#endif
}
static int ___ENTROPY_CSLINUX(void *buf, size_t len, int retry_max, size_t *written) { //todo UNFINISHED
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
static int ___ENTROPY_CSDARWIN(void *buf, size_t len, int retry_max, size_t *written) { //todo UNFINISHED
#if __OS_MACOS__ || __OS_BSD__
    unsigned char *ucbuf = (unsigned char *)buf;
    size_t remain = len, loop_limit = len + (size_t)retry_max;
    int global_status = 0;
    for (size_t i = 0; _lib_crt_lt(i, loop_limit); ++i) {
        // Process in chunks (getentropy has 256 byte limit)
        size_t n = _lib_crt_select((remain > 256), 256, remain);
        int ret = getentropy(ucbuf, n); global_status |= (ret); 
        n &= (-!(ret)); // ret == 0 --> n STAYS
        ucbuf += n; (*written) += n; remain -= n;
    }
    return 0;
#else
    return -1;
#endif
}
static int ___ENTROPY_CSIOS(void *buf, size_t len, int retry_max, size_t *written) { //todo UNFINISHED
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
static int ___ENTROPY_CSWIN64(void *buf, size_t len, size_t *written) { //todo UNFINISHED
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
static int __GET_ENTROPY_CSUNBL(void *buf, size_t len, int retry_max, size_t *written) {
#if __OS_LINUX__
    return ___ENTROPY_CSLINUX(buf, len, retry_max, written);
#elif __OS_IOS__
    return ___ENTROPY_CSIOS(buf, len, retry_max, written);
#elif __OS_MACOS__ || __OS_BSD__
    return ___ENTROPY_CSDARWIN(buf, len, retry_max, written);
#elif __OS_WIN64__
    return ___ENTROPY_CSWIN64(buf, len, retry_max, written);
#else
    return 1; // or return -1;
#endif
}
static uint64_t __jitter_harvest_cs(int loop_count) { //todo UNFINISHED
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
// Main Functions
int __GET_ENTROPY_PQC(void *buf, size_t len) { //todo UNFINISHED
    // Chunk based approach - Safer
    uint8_t pool_os[512], pool_hw[512];
    size_t processed = 0, os_write = 0, hw_write = 0;
    while (processed < len) {
        size_t chunk = (len - processed > 512) ? 512 : (len - processed);
        // Gathers into pool_hw via CPU instruction for Hardware pool
        int trng_ret = __CPU_CSTRNG_SEED__(pool_hw, chunk, 512, &hw_write);
        if (trng_ret == -2) return -1; // FATAL ERROR IN HARDWARE POOL
        if (trng_ret == -1) {
            int drbg_ret = __CPU_CSDBRG_SEED__(&pool_hw[hw_write], chunk - hw_write, 512, &hw_write);
            if (drbg_ret == -2) return -1; // FATAL ERROR IN HARDWARE DRBG
            if (drbg_ret == -1) {
                uint64_t *jitter_ptr = (uint64_t *)(&pool_hw[hw_write]);
                for (size_t i = 0; i < ((chunk - hw_write) >> 3); ++i) jitter_ptr[i] = __jitter_harvest_cs(4096);
                size_t tail = (chunk - hw_write) & (3 - 1); // chunk % 8
                if (tail) { // Filling in the remaining spaces
                    uint64_t last_jit = __jitter_harvest_cs(4096);
                    __INTERNAL_MEMCPY_STRICT__(pool_os + ((chunk - hw_write) - tail), &last_jit, tail);
                    last_jit = 0;
                } jitter_ptr = 0; tail = 0;
            }
        }
        // Gathers into pool_os via Kernel Entropy Pool
        int os_ret = __GET_ENTROPY_CSUNBL(pool_os, chunk, 512, &os_write);
        if (os_ret) {
            uint64_t *jitter_ptr = (uint64_t *)(&pool_os[os_write]);
            for (size_t i = 0; i < ((chunk - os_write) >> 3); ++i) jitter_ptr[i] = __jitter_harvest_cs(4096);
            size_t tail = (chunk - os_write) & (3 - 1); // chunk % 8
            if (tail) { // Filling in the remaining spaces
                uint64_t last_jit = __jitter_harvest_cs(4096);
                __INTERNAL_MEMCPY_STRICT__(pool_os + ((chunk - os_write) - tail), &last_jit, tail);
                last_jit = 0;
            } jitter_ptr = 0; tail = 0;
        }
        // Prevention of structural bias via XORing + Multiplying raw entropy
        for (size_t i = 0; i < chunk; ++i) {
            ((uint8_t*)buf)[processed + i] = pool_os[i] ^ pool_hw[i]; /* XORing */
            ((uint8_t*)buf)[processed + i] *= (pool_hw[i] & 1) ? pool_hw[i] : pool_hw [i] - 1; // Mul by odd
        } processed += chunk; hw_write = 0; os_write = 0; chunk = 0;
    } __INTERNAL_MEMWIPE_STRICT__(pool_os, 512); __INTERNAL_MEMWIPE_STRICT__(pool_hw, 512);
    processed = 0; os_write = 0; os_write = 0; hw_write = 0;
}