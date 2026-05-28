#include "intrinsics.h"

//* =================== PERF-BASED IFUNC TABLE UNIT FUNCTIONS =================== *//
_BITOPS_FTABLE _libdnml_gbitops_ftable;
_ARITH_FTABLE _libdnml_garith_ftable;
_MARITH_FTABLE _libdnml_gmarith_ftable;
_HW_FTABLE _libdnml_ghw_ftable;

void _libdnml_fill_gbitops(void) {
#if __ARCH_X86_64__
// CLZ - Detect ABM (Advanced Bit Manipulation)
_libdnml_gbitops_ftable.clz64 = (libdnml_x64_caps.x86_abm) ? _x86_clz64e : _x86_clz64s;
// CTZ - Detect BMI1 (Bit Manipulation Instructions 1)
_libdnml_gbitops_ftable.clz64 = (libdnml_x64_caps.x86_bmi1) ? _x86_ctz64e : _x86_ctz64s
// POPCNT - Detect SSE4.2
_libdnml_fill_gbitops.pcnt64 = (libdnml_x64_caps.x86_sse4_2) ? _x86_pcnt64e : _cintrin_pcnt64;
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
void _libdnml_fill_garith(void) {
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
void _libdnml_fill_gmarith(void) {
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
void _libdnml_fill_galg(void) {
#if __ARCH_X86_64__
#elif __ARCH_ARM64__
#elif __ARCH_RVI64__
#else
#endif
}
void _libdnml_fill_ghw(void) {
#if __ARCH_X86_64__
_libdnml_ghw_ftable.hw_drbg = (libdnml_x64_caps.x86_rdrand) ? _x86_hw_drbg : _cintrin_shallow_rng;
_libdnml_ghw_ftable.hw_trng = (libdnml_x64_caps.x86_rdseed) ? _x86_hw_trng : _cintrin_shallow_rng;
_libdnml_ghw_ftable.hw_halt = _x86_full_halt;
_libdnml_ghw_ftable.hw_shalt = _x86_shallow_halt;
#elif __ARCH_ARM64__
_libdnml_ghw_ftable.hw_drbg = (libdnml_arm64_caps.armv85_feat_rng) ? _arm64_hw_drbg : _cintrin_shallow_rng;
_libdnml_ghw_ftable.hw_trng = (libdnml_arm64_caps.armv85_feat_rng) ? _arm64_hw_trng : _cintrin_shallow_rng;
_libdnml_ghw_ftable.hw_halt = _arm64_full_halt;
_libdnml_ghw_ftable.hw_shalt = _arm64_shallow_halt;
#elif __ARCH_RVI64__
_libdnml_ghw_ftable.hw_drbg = (libdnml_x64_caps.rv64_zkr) ? _rv64_hw_trng : _cintrin_shallow_rng;
_libdnml_ghw_ftable.hw_trng = (libdnml_x64_caps.rv64_zkr) ? _rv64_hw_trng : _cintrin_shallow_rng;
_libdnml_ghw_ftable.hw_halt = (libdnml_x64_caps.rv64_zihintpause) ? _rv64_full_halt : _cintrin_nop_halt;
_libdnml_ghw_ftable.hw_shalt = (libdnml_x64_caps.rv64_zihintpause) ? _rv64_shallow_halt : _cintrin_nop_halt;
#else
_libdnml_ghw_ftable.hw_drbg = _cintrin_shallow_rng;
_libdnml_ghw_ftable.hw_trng = _cintrin_shallow_rng;
_libdnml_ghw_ftable.hw_halt = _cintrin_nop_halt;
_libdnml_ghw_ftable.hw_shalt = _cintrin_nop_halt;
#endif
}


//* =================== CRT-BASED IFUNC TABLE UNIT FUNCTIONS =================== *//
_CRT_BITOPS_FTABLE _libdnml_crt_gbitops_ftable;
_CRT_ARITH_FTABLE _libdnml_crt_garith_ftable;
_CRT_ALG_FTABLE _libdnml_crt_galg_ftable;
_CRT_SEC_FTABLE _libdnml_crt_sec_ftable;

void _libdnml_fill_crt_gbitops(void) {}
void _libdnml_fill_crt_garith(void) {}
void _libdnml_fill_crt_galg(void) {}
void _libdnml_fill_crt_gsec(void) {}