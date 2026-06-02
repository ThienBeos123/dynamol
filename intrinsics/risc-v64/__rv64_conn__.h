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
/* ---------------------- Performance-based ---------------------- */
// _rv64_arith.S
extern uint64_t _rv64_add64c(uint64_t a, uint64_t b, uint8_t *carry); //* Also crt-secured
extern uint64_t _rv64_sub64b(uint64_t a, uint64_t b, uint8_t *borrow); //* Also crt-secured
extern uint64_t _rv64_wmul128(uint64_t a, uint64_t b, uint64_t *hi); //* Also crt-secured
extern uint64_t _rv64_wdiv128(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rhat, uint8_t *overflowed);
// _rv64_marith.S
extern uint64_t _rv64_modinv64(uint64_t x);
// _rv64_bitops.S
extern uint8_t _rv64_clz64(uint64_t x); //* Also crt-secured
extern uint8_t _rv64_ctz64(uint64_t x); //* Also crt-secured
extern uint64_t _rv64_bswap64(uint64_t x); //* Also crt-secured
extern uint8_t _rv64_pcnt64(uint64_t x); //* Also crt-secured
// _rv64_port_bitops.S
extern uint8_t _rv64_clz64p(uint64_t x); // perf-based
extern uint8_t _rv64_ctz64p(uint64_t x); // perf-based
extern uint64_t _rv64_bswap64_port(uint64_t x); //* Also crt-secured
extern uint8_t _rv64_pcnt64_port(uint64_t x); //* Also crt-secured
// _rv64_sec.S
extern uint64_t _rv64_hw_trng(int *err);
// _rv64_hw.S
extern void _rv64_full_halt(void); //* Also crt-secured
extern void _rv64_shallow_halt(void); //* Also crt-secured
/* ---------------------- Cryptography-based ---------------------- */
// _rv64_arith.S
extern uint64_t _rv64_crt_wdiv128(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rhat, uint8_t *ovf);
// _rv64_port_bitops.S
extern uint8_t _rv64_clz64c(uint64_t x); // crypto-based
extern uint8_t _rv64_ctz64c(uint64_t x); // crypto-based
// _rv64_crt_u64cmp.S
extern uint8_t _rv64_crt_lt(uint64_t a, uint64_t b);
extern uint8_t _rv64_crt_gt(uint64_t a, uint64_t b);
extern uint8_t _rv64_crt_leq(uint64_t a, uint64_t b);
extern uint8_t _rv64_crt_geq(uint64_t a, uint64_t b);
// _rv64_crt_i64cmp.S
extern uint8_t _rv64_crt_lti64(int64_t a, int64_t b);
extern uint8_t _rv64_crt_gti64(int64_t a, int64_t b);
extern uint8_t _rv64_crt_leqi64(int64_t a, int64_t b);
extern uint8_t _rv64_crt_geqi64(int64_t a, int64_t b);
// _rv64_crt_equal.S
extern uint8_t _rv64_crt_ispos(int64_t a);
extern uint8_t _rv64_crt_isneg(int64_t a);
extern uint8_t _rv64_crt_eq(uint64_t a, uint64_t b);
extern uint8_t _rv64_crt_neq(uint64_t a, uint64_t b);
extern uint64_t _rv64_crt_select(uint8_t cond, uint64_t a, uint64_t b);
// _rv64_sec.S
extern uint64_t _rv64_hw_cstrng(int *err);
// _rv64_crt_alg.S

#ifdef __cplusplus
}
#endif
#endif

#endif
#endif
