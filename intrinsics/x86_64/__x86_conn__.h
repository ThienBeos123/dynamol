#ifndef ____DNML_X86_CONN
#define ____DNML_X86_CONN

#if defined(__ASSEMBLER__) || defined(__ASSEMBLY__)
    #if defined(__x86_64__) || defined(_M_X64)
        #define __ARCH_X86_64__ 1
    #else
        #define __ARCH_X86_64__ 0
    #endif
    #include <dnml_sys/asm/__asm_macros.h>
#else
    #include <dnml_sys/sys.h>
#endif

#if __ARCH_X86_64__

//* -------- SYSTEM-V ABI ---------- *//
#if __ABI_X64_SYSV__
#ifndef __ASSEMBLER__
#ifdef __cplusplus
extern "C" {
#endif
/* ---------------------- Performance-based ---------------------- */
// _x86_sysv_arith.S
extern uint64_t _x86sv_add64c(uint64_t a, uint64_t b, uint8_t *carry); //* Also crt-secured
extern uint64_t _x86sv_sub64b(uint64_t a, uint64_t b, uint8_t *borrow); //* Also crt-secured
extern uint64_t _x86sv_wmul128(uint64_t a, uint64_t b, uint64_t *hi); //* Also crt-secured
extern uint64_t _x86sv_wdiv128(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rem, uint8_t *overflowed);
#define _x86_add64c     _x86sv_add64c
#define _x86_sub64b     _x86sv_sub64b
#define _x86_wmul128    _x86sv_wmul128
#define _x86_wdiv128    _x86sv_wdiv128
// _x86_sysv_marith.S
extern uint64_t _x86sv_modinv64(uint64_t x);
#define _x86_modinv64   _x86sv_modinv64
// _x86_sysv_bitops.S
extern uint8_t _x86sv_clz64e(uint64_t x); //* Also crt-secured
extern uint8_t _x86sv_clz64s(uint64_t x); //* Also crt-secured 
extern uint8_t _x86sv_ctz64e(uint64_t x); //* Also crt-secured
extern uint8_t _x86sv_ctz64s(uint64_t x); //* Also crt-secured
extern uint64_t _x86sv_bswap64(uint64_t x); //* Also crt-secured
extern uint8_t _x86sv_pcnt64e(uint64_t x); //* Also crt-secured
extern uint8_t _x86sv_pcnt64s(uint64_t x); //* Also crt-secured
#define _x86_clz64e     _x86sv_clz64e
#define _x86_clz64s     _x86sv_clz64s 
#define _x86_ctz64e     _x86sv_ctz64e
#define _x86_ctz64s     _x86sv_ctz64s
#define _x86_bswap64    _x86sv_bswap64
#define _x86_pcnt64e    _x86sv_pcnt64e
#define _x86_pcnt64s    _x86sv_pcnt64s
// _x86_sysv_sec.S
extern uint64_t _x86sv_hw_drbg(int *err) //* Also crt-secured
extern uint64_t _x86sv_hw_trng(int *err) //* Also crt-secured
extern void _x86sv_full_halt(void) //* Also crt-secured
extern void _x86sv_shallow_halt(void)  //* Also crt-secured
#define _x86_hw_drbg    _x86sv_hw_drbg
#define _x86_hw_trng    _x86sv_hw_trng
#define _x86_full_halt  _x86sv_full_halt
#define _x86_shallow_halt _x86sv_shallow_halt
/* ---------------------- Cryptography-based ---------------------- */
// _x86_sysv_arith.S
extern uint64_t _x86sv_crt_wdiv128(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rhat, uint8_t *ovf);
#define _x86_crt_wdiv128    _x86sv_crt_wdiv128
// _x86_sysv_crt_u64cmp.S
uint8_t _x86sv_crt_lt(uint64_t a, uint64_t b);
uint8_t _x86sv_crt_gt(uint64_t a, uint64_t b);
uint8_t _x86sv_crt_leq(uint64_t a, uint64_t b);
uint8_t _x86sv_crt_geq(uint64_t a, uint64_t b);
#define _x86_crt_lt     _x86sv_crt_lt
#define _x86_crt_gt     _x86sv_crt_gt
#define _x86_crt_leq    _x86sv_crt_leq
#define _x86_crt_geq    _x86sv_crt_geq
// _x86_sysv_crt_i64cmp.S
uint8_t _x86sv_crt_lti64(uint64_t a, uint64_t b);
uint8_t _x86sv_crt_gti64(uint64_t a, uint64_t b);
uint8_t _x86sv_crt_leqi64(uint64_t a, uint64_t b);
uint8_t _x86sv_crt_geqi64(uint64_t a, uint64_t b);
#define _x86_crt_lti64     _x86sv_crt_lti64
#define _x86_crt_gti64     _x86sv_crt_gti64
#define _x86_crt_leqi64    _x86sv_crt_leqi64
#define _x86_crt_geqi64    _x86sv_crt_geqi64
// _x86_sysv_crt_equal.S
uint8_t _x86sv_crt_ispos(uint64_t a);
uint8_t _x86sv_crt_isneg(uint64_t a);
uint8_t _x86sv_crt_eq(uint64_t a, uint64_t b);
uint8_t _x86sv_crt_neq(uint64_t a, uint64_t b);
uint64_t _x86sv_crt_select(uint8_t cond, uint64_t a, uint64_t b);
#define _x86_crt_ispos      _x86sv_crt_ispos
#define _x86_crt_isneg      _x86sv_crt_isneg
#define _x86_crt_eq         _x86sv_crt_eq
#define _x86_crt_neq        _x86sv_crt_neq
#define _x86_crt_select     _x86sv_crt_select
// _x86_sysv_crt_alg.S

#ifdef __cplusplus
}
#endif
#endif


//* -------- WINDOW-64 ABI ---------- *//
#elif __ABI_X64_WIN64__
#ifndef __ASSEMBLER__
#ifdef __cplusplus
extern "C" {
#endif
/* ---------------------- Performance-based ---------------------- */
// _x86_win64_arith.S
extern uint64_t _x86w64_add64c(uint64_t a, uint64_t b, uint8_t *carry); //* Also crt-secured
extern uint64_t _x86w64_sub64b(uint64_t a, uint64_t b, uint8_t *borrow); //* Also crt-secured
extern uint64_t _x86w64_wmul128(uint64_t a, uint64_t b, uint64_t *hi); //* Also crt-secured
extern uint64_t _x86w64_wdiv128(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rem, uint8_t *overflowed);
#define _x86_add64c     _x86w64_add64c
#define _x86_sub64b     _x86w64_sub64b
#define _x86_wmul128    _x86w64_wmul128
#define _x86_wdiv128    _x86w64_wdiv128
// _x86_win64_marith.S
extern uint64_t _x86w64_modinv64(uint64_t x);
#define _x86_modinv64   _x86w64_modinv64
// _x86_win64_bitops.S
extern uint8_t _x86w64_clz64e(uint64_t x); //* Also crt-secured
extern uint8_t _x86w64_clz64s(uint64_t x); //* Also crt-secured
extern uint8_t _x86w64_ctz64e(uint64_t x); //* Also crt-secured
extern uint8_t _x86w64_ctz64s(uint64_t x); //* Also crt-secured
extern uint64_t _x86w64_bswap64(uint64_t x); //* Also crt-secured
extern uint8_t _x86w64_pcnt64e(uint64_t x); //* Also crt-secured
extern uint8_t _x86w64_pcnt64s(uint64_t x); //* Also crt-secured
#define _x86_clz64e     _x86w64_clz64e
#define _x86_clz64s     _x86w64_clz64s  
#define _x86_ctz64e     _x86w64_ctz64e
#define _x86_ctz64s     _x86w64_ctz64s
#define _x86_bswap64    _x86w64_bswap64
#define _x86_pcnt64e    _x86w64_pcnt64e
#define _x86_pcnt64s    _x86w64_pcnt64s
// _x86_win64_sec.S
extern uint64_t _x86w64_hw_drbg(int *err) //* Also crt-secured
extern uint64_t _x86w64_hw_trng(int *err) //* Also crt-secured
extern void _x86w64_full_halt(void) //* Also crt-secured
extern void _x86w64_shallow_halt(void) //* Also crt-secured
#define _x86_hw_drbg    _x86w64_hw_drbg
#define _x86_hw_trng    _x86w64_hw_trng
#define _x86_full_halt  _x86w64_full_halt
#define _x86_shallow_halt _x86w64_shallow_halt
/* ---------------------- Cryptography-based ---------------------- */
// _x86_win64_arith.S
extern uint64_t _x86w64_crt_wdiv128(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rhat, uint8_t *ovf);
#define _x86_crt_wdiv128    _x86w64_crt_wdiv128
// _x86_win64_crt_u64cmp.S
uint8_t _x86w64_crt_lt(uint64_t a, uint64_t b);
uint8_t _x86w64_crt_gt(uint64_t a, uint64_t b);
uint8_t _x86w64_crt_leq(uint64_t a, uint64_t b);
uint8_t _x86w64_crt_geq(uint64_t a, uint64_t b);
#define _x86_crt_lt     _x86w64_crt_lt
#define _x86_crt_gt     _x86w64_crt_gt
#define _x86_crt_leq    _x86w64_crt_leq
#define _x86_crt_geq    _x86w64_crt_geq
// _x86_win64_crt_i64cmp.S
uint8_t _x86w64_crt_lti64(uint64_t a, uint64_t b);
uint8_t _x86w64_crt_gti64(uint64_t a, uint64_t b);
uint8_t _x86w64_crt_leqi64(uint64_t a, uint64_t b);
uint8_t _x86w64_crt_geqi64(uint64_t a, uint64_t b);
#define _x86_crt_lti64     _x86w64_crt_lti64
#define _x86_crt_gti64     _x86w64_crt_gti64
#define _x86_crt_leqi64    _x86w64_crt_leqi64
#define _x86_crt_geqi64    _x86w64_crt_geqi64
// _x86_win64_crt_equal.S
uint8_t _x86w64_crt_ispos(uint64_t a);
uint8_t _x86w64_crt_isneg(uint64_t a);
uint8_t _x86w64_crt_eq(uint64_t a, uint64_t b);
uint8_t _x86w64_crt_neq(uint64_t a, uint64_t b);
uint64_t _x86w64_crt_select(uint8_t cond, uint64_t a, uint64_t b);
#define _x86_crt_ispos      _x86w64_crt_ispos
#define _x86_crt_isneg      _x86w64_crt_isneg
#define _x86_crt_eq         _x86w64_crt_eq
#define _x86_crt_neq        _x86w64_crt_neq
#define _x86_crt_select     _x86w64_crt_select
// _x86_win64_crt_alg.S


#ifdef __cplusplus
}
#endif
#endif


#endif
#endif
#endif