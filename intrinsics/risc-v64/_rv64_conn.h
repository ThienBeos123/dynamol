#ifndef ____DNML_RVI64_CONN
#define ____DNML_RVI64_CONN

#include <system/sys.h>


// _rvi64_arith.S
extern uint64_t _rv64_add64c(uint64_t a, uint64_t b, uint8_t *carry);
extern uint64_t _rv64_sub64b(uint64_t a, uint64_t b, uint8_t *borrow);
extern uint64_t _rv64_wmul128(uint64_t a, uint64_t b, uint64_t *hi);
// _rvi64_marith.S
extern uint64_t _rv64_modinv64(uint64_t x);
// _rvi64_alg.S
// _rvi64_bitops.S
extern uint8_t _rv64_clz64(uint64_t x);
extern uint8_t _rv64_ctz64(uint64_t x);
extern uint64_t _rv64_bswap64(uint64_t x);
extern uint8_t _rv64_pcnt64(uint64_t x);



#endif