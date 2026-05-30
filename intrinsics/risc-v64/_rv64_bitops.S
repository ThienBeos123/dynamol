#include "___rv64_conn__.h"
#if __ARCH_RVI64__


asm_text_section
.option     nopic
.attribute  arch, "rv64gc_zbb"
.attribute  unaligned_access, 0
.attribute  stack_align, 16
.align      1

.globl      _rv64_clz64
.globl      _rv64_ctz64
.globl      _rv64_bswap64
.globl      _rv64_pcnt64

.type       _rv64_clz64, @function
.type       _rv64_ctz64, @function
.type       _rv64_bswap64, @function
.type       _rv64_pcnt64, @function


/* ------- COUNT LEADING ZERO 64 BIT -------
* uint8_t _rv64_clz64(uint64_t x)
* arg: a0: uint64_t x --> (64 BIT)
* ret: a0: clz(x) ------> (8  BIT)
*/
_rv64_clz64:
    clz a0, a0
    ret


/* ------- COUNT TRAILING ZERO 64 BIT -------
* uint8_t _rv64_ctz64(uint64_t x)
* arg: a0: uint64_t x --> (64 BIT)
* ret: a0: ctz(x) ------> (8  BIT)
*/
_rv64_ctz64:
    ctz a0, a0
    ret


/* ------- BYTE SWAP 64 BIT -------
* uint8_t _rv64_bswap64(uint64_t x)
* arg: a0: uint64_t x ----> (64 BIT)
* ret: a0: bswap(x) ------> (8  BIT)
*/
_rv64_bswap64:
    rev a0, a0
    ret


/* ------- POPULATION COUNT 64 BIT -------
* uint8_t _rv64_pcnt64(uint64_t x)
* arg: a0: uint64_t x ----> (64 BIT)
* ret: a0: pcnt(x) -------> (8  BIT)
*/
_rv64_pcnt64:
    pcnt a0, a0
    ret


#endif