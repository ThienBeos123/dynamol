#ifndef ____DNML_AARCH64_CONN
#define ____DNML_AARCH64_CONN

#if defined(__ASSEMBLER__) || defined(__ASSEMBLY__)
    #if defined(__aarch64__) || defined(__ARM64__) || defined(__arm64__)
        #define __ARCH_ARM64__ 1
    #else
        #define __ARCH_ARM64__ 0
    #endif
    #include <dnml_sys/asm/__asm_macros.h>
#else
    #include <dnml_sys/sys.h>
#endif

#if __ARCH_ARM64__
#include "__arm64_macros.h"
#include <_libdnml_config/numeric_config.h>

#ifndef __ASSEMBLER__
#ifdef __cplusplus
extern "C" {
#endif
/* ---------------------- Performance-based ---------------------- */
// _arm64_arith.S
extern uint64_t _arm64_add64c(uint64_t a, uint64_t b, uint8_t *carry); //* Also crt-secured
extern uint64_t _arm64_sub64b(uint64_t a, uint64_t b, uint8_t *borrow); //* Also crt-secured
extern uint64_t _arm64_wmul128(uint64_t a, uint64_t b, uint64_t *hi); //* Also crt-secured
extern uint64_t _arm64_wdiv128(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rhat, uint8_t *overflowed);
// _arm64_marith.S
extern uint64_t _arm64_modinv64(uint64_t x);
// _arm64_bitops.S
extern uint8_t _arm64_clz64(uint64_t x); //* Also crt-secured
extern uint8_t _arm64_ctz64(uint64_t x); //* Also crt-secured
extern uint64_t _arm64_bswap64(uint64_t x); //* Also crt-secured
extern uint8_t _arm64_pcnt64(uint64_t x); //* Also crt-secured
// _arm64_sec_.S
extern uint64_t _arm64_hw_drbg(int *err); //* Also crt-secured
extern uint64_t _arm64_hw_trng(int *err); //* Also crt-secured
extern void _arm64_full_halt(void); //* Also crt-secured
extern void _arm64_shallow_halt(void); //* Also crt-secured
/* ---------------------- Cryptograhy-based ---------------------- */
// _arm64_arith.S
extern uint64_t _arm64_crt_wdiv128(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rhat, uint8_t *ovf);
// _arm64_crt_u64cmp.S
uint8_t _arm64_crt_lt(uint64_t a, uint64_t b);
uint8_t _arm64_crt_gt(uint64_t a, uint64_t b);
uint8_t _arm64_crt_leq(uint64_t a, uint64_t b);
uint8_t _arm64_crt_geq(uint64_t a, uint64_t b);
// _arm64_crt_i64cmp.S
uint8_t _arm64_crt_lti64(uint64_t a, uint64_t b);
uint8_t _arm64_crt_gti64(uint64_t a, uint64_t b);
uint8_t _arm64_crt_leqi64(uint64_t a, uint64_t b);
uint8_t _arm64_crt_geqi64(uint64_t a, uint64_t b);
// _arm64_crt_equal.S
uint8_t _arm64_crt_ispos(uint64_t a);
uint8_t _arm64_crt_isneg(uint64_t a);
uint8_t _arm64_crt_eq(uint64_t a, uint64_t b);
uint8_t _arm64_crt_neq(uint64_t a, uint64_t b);
uint64_t __arm64_crt_select(uint8_t cond, uint64_t a, uint64_t b);
// _arm64_crt_alg.S


#ifdef __cplusplus
}
#endif
#endif

#endif
#endif