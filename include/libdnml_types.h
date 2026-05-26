#ifndef ____DNML_TYPES_H
#define ____DNML_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <include.h>
#include "dnml_sys/sys.h"
#include "_libdnml_config/settings.h"

//* ======================================= DYNAMOL DEFINITION ========================================= *//
#define BIGINT_LIMBS_BITS 64
#define DASI_OVERFLOW_THRES10                                                  \
  1844677407370955161 // 1,844,674,407,370,955,161 (UINT64_MAX / 10)
typedef uint64_t limb_t;
typedef struct {
    size_t n; size_t cap;
    limb_t *limbs; int8_t sign;
} bigInt;

typedef struct __424947666C6F6174__ {
  bigInt mantissa;
  uint32_t exponent;
  // RAM Space needed to calculate the maxmimum exponent value as a power of 2
  // (2^(2^32 - 1))
  // ------> Approximately 550MB
  // ------> Doable, but extremely heavy for computers
} bigFloat;



//* ======================================= DYNAMOL DEFINITION ========================================= *//
#define CRYPTINT_LIMBS_BITS 64
typedef struct {
    size_t n; size_t cap;
    limb_t *limbs; int8_t sign;
    bool poisoned; /* 
        This field is not to be taken the same as crint_validate and its variants.
        crint_validate and its variants check and assert the validity of a cryptInt's
        internal state, following CRYPT_NUM_CONTRACT.txt. This field is specifically
        reserved for special case in which it violates mathematical rule and is undefined
    */
} crint;





#ifdef __cplusplus
}
#endif

#endif