#ifndef ____DNML_VANILLC_CONN__
#define ____DNML_VANILLC_CONN__


#include <_libdnml_config/numeric_config.h>
#include <libdnml_types.h>
#include <dnml_sys/sys.h>
#include <include.h>


/* _vanillc_arith.c */
uint64_t _cintrin_add64c(uint64_t a, uint64_t b, uint8_t *carry);
uint64_t _cintrin_sub64b(uint64_t a, uint64_t b, uint8_t *borrow);
uint64_t _cintrin_wmul128(uint64_t a, uint64_t b, uint64_t *hi);
uint64_t _cintrin_wdiv128(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rhat, uint8_t *overflowed);
uint64_t _cintrin_modinv64(uint64_t x);

/* _vanillc_marith.c */
uint64_t _cintrin_modinv64(uint64_t x);

/* _vanillc_bitops.c */
uint8_t _cintrin_clz64(uint64_t x);
uint8_t _cintrin_ctz64(uint64_t x);
uint64_t _cintrin_bswap64(uint64_t x);
uint8_t _cintrin_pcnt64(uint64_t x);

/* _vanillc_misc.c */
uint64_t _cintrin_shallow_rng(int *err);
void _cintrin_nop_halt(void);


#endif