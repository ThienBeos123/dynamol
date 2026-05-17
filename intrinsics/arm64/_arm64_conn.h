#ifndef ____DNML_AARCH64_CONN
#define ____DNML_AARCH64_CONN

#include <system/sys.h>

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


#endif