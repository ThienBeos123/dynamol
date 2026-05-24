#ifndef ____DNML_RVI64_CONN
#define ____DNML_RVI64_CONN

#if defined(__ASSEMBLER__) || defined(__ASSEMBLY__)
    #if defined(__riscv) && (__riscv_xlen == 64)
        #define __ARCH_RVI64__ 1
    #else
        #define __ARCH_RVI64__ 0
    #endif
    #include <dnml_sys/asm/__asm_macros.h>
#else
    #include <dnml_sys/sys.h>
#endif

#if __ARCH_RVI64__

#ifndef __ASSEMBLER__
#ifdef __cplusplus
extern "C" {
#endif
// _rv64_arith.S
extern uint64_t _rv64_add64c(uint64_t a, uint64_t b, uint8_t *carry);
extern uint64_t _rv64_sub64b(uint64_t a, uint64_t b, uint8_t *borrow);
extern uint64_t _rv64_wmul128(uint64_t a, uint64_t b, uint64_t *hi);
// _rv64_marith.S
extern uint64_t _rv64_modinv64(uint64_t x);
// _rv64_alg.S
// _rv64_bitops.S
extern uint8_t _rv64_clz64(uint64_t x);
extern uint8_t _rv64_ctz64(uint64_t x);
extern uint64_t _rv64_bswap64(uint64_t x);
extern uint8_t _rv64_pcnt64(uint64_t x);
// _rv64_sec.S
extern uint64_t _rv64_hw_trng(int *err);
// _rv64_hw.S
extern void _rv64_full_halt(void);
extern void _rv64_shallow_halt(void);
#ifdef __cplusplus
}
#endif
#endif

#endif
#endif