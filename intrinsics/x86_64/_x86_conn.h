#ifndef ____DNML_X86_CONN
#define ____DNML_X86_CONN

#if defined(__ASSEMBLER__) || defined(__ASSEMBLY__)
    #if defined(__x86_64__) || defined(_M_X64)
        #define __ARCH_X86_64__ 1
    #else
        #define __ARCH_X86_64__ 0
    #endif
    #include <system/asm/__asm_macros.h>
#else
    #include <system/sys.h>
#endif

#if __ARCH_X86_64__

//* -------- SYSTEM-V ABI ---------- *//
#if __ABI_X64_SYSV__
#ifndef __ASSEMBLER__
// _x86_sysv_arith.S
extern uint64_t _x86sv_add64c(uint64_t a, uint64_t b, uint8_t *carry);
extern uint64_t _x86sv_sub64b(uint64_t a, uint64_t b, uint8_t *borrow);
extern uint64_t _x86sv_wmul128(uint64_t a, uint64_t b, uint64_t *hi);
extern uint64_t _x86sv_wdiv128(
    uint64_t lo, uint64_t hi,
    uint64_t div, uint64_t *rem
);
#define _x86_add64c     _x86sv_add64c
#define _x86_sub64b     _x86sv_sub64b
#define _x86_wmul128    _x86sv_wmul128
#define _x86_wdiv128    _x86sv_wdiv128
// _x86_sysv_marith.S
extern uint64_t _x86sv_modinv64(uint64_t x);
#define _x86_modinv64   _x86sv_modinv64
// _x86_sysv_alg.S
// _x86_sysv_bitops.S
extern uint64_t _x86sv_clz64e(uint64_t x);
extern uint64_t _x86sv_clz64s(uint64_t x);
extern uint64_t _x86sv_ctz64e(uint64_t x);
extern uint64_t _x86sv_ctz64s(uint64_t x);
extern uint64_t _x86sv_bswap64(uint64_t x);
#define _x86_clz64e     _x86sv_clz64e
#define _x86_clz64s     _x86sv_clz64s 
#define _x86_ctz64e     _x86sv_ctz64e
#define _x86_ctz64s     _x86sv_ctz64s
#define _x86_bswap64    _x86sv_bswap64
#define _x86_pcnt64e    _x86sv_pcnt64e
// _x86_sysv_sec.S
extern uint64_t _x86sv_hw_drbg(int *err)
extern uint64_t _x86sv_hw_trng(int *err)
#define _x86_hw_drbg    _x86sv_hw_drbg
#define _x86_hw_trng    _x86sv_hw_trng
// _x86_sysv_hw.S
extern void _x86sv_full_halt(void)
extern void _x86sv_shallow_halt(void)
#define _x86_full_halt  _x86sv_full_halt
#define _x86_shallow_halt _x86sv_shallow_halt
#endif


//* -------- WINDOW-64 ABI ---------- *//
#elif __ABI_X64_WIN64__
#ifndef __ASSEMBLER__
// _x86_win64_arith.S
extern uint64_t _x86w64_add64c(uint64_t a, uint64_t b, uint8_t *carry);
extern uint64_t _x86w64_sub64b(uint64_t a, uint64_t b, uint8_t *borrow);
extern uint64_t _x86w64_wmul128(uint64_t a, uint64_t b, uint64_t *hi);
extern uint64_t _x86w64_wdiv128(
    uint64_t lo, uint64_t hi,
    uint64_t div, uint64_t *rem
);
#define _x86_add64c     _x86w64_add64c
#define _x86_sub64b     _x86w64_sub64b
#define _x86_wmul128    _x86w64_wmul128
#define _x86_wdiv128    _x86w64_wdiv128
// _x86_win64_marith.S
extern uint64_t _x86w64_modinv64(uint64_t x);
#define _x86_modinv64   _x86w64_modinv64
// _x86_win64_alg.S
// _x86_win64_bitops.S
extern uint8_t _x86w64_clz64e(uint64_t x);
extern uint8_t _x86w64_clz64s(uint64_t x);
extern uint8_t _x86w64_ctz64e(uint64_t x);
extern uint8_t _x86w64_ctz64s(uint64_t x);
extern uint64_t _x86w64_bswap64(uint64_t x);
extern uint8_t
#define _x86_clz64e     _x86w64_clz64e
#define _x86_clz64s     _x86w64_clz64s  
#define _x86_ctz64e     _x86w64_ctz64e
#define _x86_ctz64s     _x86w64_ctz64s
#define _x86_bswap64    _x86w64_bswap64
#define _x86_pcnt64e    _x86w64_pcnt64
// _x86_win64_sec.S
extern uint64_t _x86w64_hw_drbg(int *err)
extern uint64_t _x86w64_hw_trng(int *err)
#define _x86_hw_drbg    _x86w64_hw_drbg
#define _x86_hw_trng    _x86w64_hw_trng
// _x86_win64_hw.S
extern void _x86w64_full_halt(void)
extern void _x86w64_shallow_halt(void)
#define _x86_full_halt  _x86w64_full_halt
#define _x86_shallow_halt _x86w64_shallow_halt
#endif


//* -------- UNKNOWN ABI ---------- *//
#else
#include "../zvanillc/_vanillc_conn.h"
// Arithmetic
#define _x86_add64c     _cintrin_add64c
#define _x86_sub64b     _cintrin_sub64b
#define _x86_wmul128    _cintrin_wmul128
#define _x86_wdiv128    _cintrin_wdiv128
// Modular Arithmetic
#define _x86_modinv64   _cintrin_modinv64
// Bitwise Operations
#define _x86_clz64e     _cintrin_clz64
#define _x86_clz64s     _cintrin_clz64
#define _x86_ctz64e     _cintrin_ctz64
#define _x86_ctz64s     _cintrin_ctz64
#define _x86_bswap64    _cintrin_bswap64
#define _x86_pcnt64e    _cintrin_pcnt64
// SecOPS + Hardware Interaction
#define _x86_hw_drbg        _cintrin_shallow_rng // Meaningless for Cryptography
#define _x86_hw_trng        _cintrin_shallow_rng // Meaningless for Cryptography
#define _x86_full_halt      _cintrin_nop_halt // Meaningless halt
#define _x86_shallow_halt   _cintrin_nop_halt // Meaningless halt


#endif
#endif
#endif