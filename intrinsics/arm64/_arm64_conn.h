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

#ifndef __ASSEMBLER__
#ifdef __cplusplus
extern "C" {
#endif
// _arm64_arith.S
extern uint64_t _arm64_add64c(uint64_t a, uint64_t b, uint8_t *carry);
extern uint64_t _arm64_sub64b(uint64_t a, uint64_t b, uint8_t *borrow);
extern uint64_t _arm64_wmul128(uint64_t a, uint64_t b, uint64_t *hi);
// _arm64_marith.S
extern uint64_t _arm64_modinv64(uint64_t x);
// _arm64_alg.S
// _arm64_bitops.S
extern uint8_t _arm64_clz64(uint64_t x);
extern uint8_t _arm64_ctz64(uint64_t x);
extern uint64_t _arm64_bswap64(uint64_t x);
extern uint8_t _arm64_pcnt64(uint64_t x);
// _arm64_sec_.S
extern uint64_t _arm64_hw_drbg(int *err);
extern uint64_t _arm64_hw_trng(int *err);
// _arm64_hw_.S
extern void _arm64_full_halt(void);
extern void _arm64_shallow_halt(void);
#ifdef __cplusplus
}
#endif
#endif

#endif
#endif