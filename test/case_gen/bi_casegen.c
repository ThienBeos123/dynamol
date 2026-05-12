#include "case_gen.h"

/* BigInt Random Generation Requirements
*   - Note 1 - Repetition of TEST_CONDUCT.md:
*       +) Key component configuration entirely dictates
*          the numerical amounts configuration, or, more precisely,
*          numerical amounts configuration is automatically not set
*          and MUST NOT be touched if "key components configuration" 
*          of such components are FALSE / NOT set to TRUE
*       
*       +) Any early implementation may accept the use of simpler &
*          less cryptographically-secured combination of srand(time(NULL))
*          and rand() by ANSI-C <stdlib.h>, though TRNG/HWRNG-seeded entropy
*          are preferred for more unbiased source of entropy for xoshiro256++
*
*       +) Regarding entropy collection via TRNG/HWRNG, it is generally
*          preferred to use lib-dnml's OS-dispatched entropy collector family -
*          _GET_ENTROPY_*() - for cross-platform compatibility and
*          stability of usage from the library's standardization. 
*          However, early prototypes may use OS-specific entropy
*          harvest ONLY for prototyping of functionality and reliability,
*          BUT final implementation must use the general function like _GET_ENTROPY_FAST() 
*          or another specialized entropy-collection function that supports
*          at least the dispatching of MacOS, Linux, AND Windows (64 bit).
*
*   - Note 2 - bi_casegen.c SPECIFIC
*       +) It is expected that any usage of bi_casegen.c (subsequently through case_gen.h) 
*          must first use the function strgen_init_sesh() to initialize a string generation
*          instance with satisfactory, entropy-filled base-state for our simple PRNG.
*
*       +) It is expected that any buffer regarding the containment of a randomly-
*          generated numerical-string instance from bi_casegen.c / bigen_write()
*          must be initialized with the size of 48 limbs. Additionally, it is preferred 
*          for the container/buffer of the randomly-generated numerical string to be 
*          newly initialized (whether stack, arena, or heap) AND/OR empty
*/

//* ----------------------------------- TYPES & CONTAINERS ----------------------------------- *//
const case_prange data_case_pmatrix[BIGEN_CNT][BIGINT_CASE_COUNT] = {
    [ BI_CLEAN_GEN ] = {
        { 0, 0, 2.5, 3.0 }, { 0, 0, 4.6, 7.4 }, { 0, 0, 4.6, 7.4 }, // ZERO, ONE, -ONE
        { 0, 0, 8.9, 15.6 }, {0, 0, 8.9, 15.6 }, // SINGLE_LIMB, -SINGLE_LIMB
        { 0, 0, 2.5, 4.6 }, { 0, 0, 0.1, 1.5 }, // MAX, -NEG_MAX
        { 0, 0, 5.2, 7.9 }, { 0, 0, 7.3, 10.5 }, { 0, 0, 7.3, 10.5 }, // Alt, MSB, LSB
        { 0, 0, 2.5, 4.9 }, { 0, 0, 5.6, 9.7 }, // Wide Bottom, Wide Top
        { 0, 0, 5.0, 6.5 }, { 0, 0, 5.0, 6.5 }, // Power of 2, Power of 8
        { 0, 0, 5.0, 6.5 }, { 0, 0, 5.0, 6.5 }, // Power of 10, Power of 16
        { 0, 0, 1.2, 2.4 } // Pure Randomness
    },
    [ BI_EDGE_GEN ]  = {
        { 0, 0, 0.001, 0.005 }, { 0, 0, 0.004, 0.004 }, { 0, 0, 0.004, 0.004 }, // ZERO, ONE, -ONE
        { 0, 0, 0.1, 0.6 }, {0, 0, 0.1, 0.6 }, // SINGLE_LIMB, -SINGLE_LIMB
        { 0, 0, 10.4, 14.3 }, { 0, 0, 11.2, 16.3 }, // MAX, -NEG_MAX
        { 0, 0, 7.8, 9.3 }, { 0, 0, 6.3, 7.5 }, { 0, 0, 6.3, 7.5 }, // Alt, MSB, LSB
        { 0, 0, 8.7, 11.2 }, { 0, 0, 8.7, 11.2 }, // Wide Bottom, Wide Top
        { 0, 0, 2.4, 4.5 }, { 0, 0, 2.4, 4.5 }, // Power of 2, Power of 8
        { 0, 0, 2.4, 4.5 }, { 0, 0, 2.4, 4.5 }, // Power of 10, Power of 16
        { 0, 0, 0.1, 2.5 } // Pure Randomness
    },
    [ BI_MIXED_GEN ] = {
        { 0, 0, 0.01, 0.05 }, { 0, 0, 0.05, 0.09 }, { 0, 0, 0.05, 0.09 }, // ZERO, ONE, -ONE
        { 0, 0, 0.09, 0.3 }, {0, 0, 0.09, 0.3 }, // SINGLE_LIMB, -SINGLE_LIMB
        { 0, 0, 7.8, 9.4 }, { 0, 0, 8.2, 11.0 }, // MAX, -NEG_MAX
        { 0, 0, 10.2, 15.8 }, { 0, 0, 0.9, 1.5 }, { 0, 0, 0.9, 1.5 }, // Alt, MSB, LSB
        { 0, 0, 2.5, 4.9 }, { 0, 0, 5.6, 9.7 }, // Wide Bottom, Wide Top
        { 0, 0, 2.3, 5.5 }, { 0, 0, 2.3, 5.5 }, // Power of 2, Power of 8
        { 0, 0, 2.3, 5.5 }, { 0, 0, 2.3, 5.5 }, // Power of 10, Power of 16
        { 0, 0, 55.6, 66.6 } // Pure Randomness
    }
};
const case_prange cap_case_pmatrix[BIGEN_CNT][BIGINT_CASE_COUNT] = {
    [ BI_CLEAN_GEN ] = {
        { 0, 1, 2.5, 3.0 }, // SINGULAR_LIMB
        { 1, BIGINT_CAP, 19.5, 25.6 }, // EXACT_CAPACITY
        { 0, BIGINT_CAP - 1, 12.4, 13.7 }, // NEAR_CAPACITY
        { 0, BIGINT_CAP >> 2, 7.8, 10.4 }, // QUARTERLY_SPARSE
        { 0, BIGINT_CAP >> 1, 15.6, 20.8 }, // HALF_SPARSE
        { 0, BIGINT_CAP / 3 + 1, 6.8, 9.7 } // CAP_RANDOM
    },
    [ BI_EDGE_GEN ] = {
        { 0, 2, 0.001, 0.002 }, // SINGULAR_LIMB
        { 1, BIGINT_CAP, 24.5, 36.7 }, // EXACT_CAPACITY
        { 0, BIGINT_CAP - 1, 24.5, 26.7 }, // NEAR_CAPACITY
        { 0, BIGINT_CAP >> 2, 2.4, 5.6 }, // QUARTERLY_SPARSE
        { 0, BIGINT_CAP >> 1, 4.8, 11.2 }, // HALF_SPARSE
        { 0, BIGINT_CAP, 24.5, 29.5 } // CAP_RANDOM
    },
    [ BI_MIXED_GEN ] = {
        { 0, 1, 0.5, 1.5 }, // SINGULAR_LIMB
        { 1, BIGINT_CAP, 2.3, 5.6 }, // EXACT_CAPACITY
        { 0, BIGINT_CAP - 1, 4.2, 6.7 }, // NEAR_CAPACITY
        { 0, BIGINT_CAP >> 2, 7.8, 9.4 }, // QUARTERLY_SPARSE
        { 0, BIGINT_CAP >> 1, 15.6, 16.8 }, // HALF_SPARSE
        { 0, BIGINT_CAP, 67.8, 78.9 } // CAP_RANDOM
    },
};


//* ----------------------------------- FUNCTIONS ----------------------------------- *//
// Main Functions
inline void bigen_init_sesh()