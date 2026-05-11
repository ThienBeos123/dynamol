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
const case_prange data_case_pmatrix[3][BIGINT_CASE_COUNT] = {
    [ BI_CLEAN_GEN ] = {
        { 0, 0,  }
    },
    [ BI_EDGE_GEN ]  = {},
    [ BI_MIXED_GEN ] = {}
}