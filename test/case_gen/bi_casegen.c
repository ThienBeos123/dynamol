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
        { 0, 0, 2.5f, 3.0f }, { 0, 0, 4.6f, 7.4f }, { 0, 0, 4.6f, 7.4f }, // ZERO, ONE, -ONE
        { 0, 0, 10.9f, 16.6f }, {0, 0, 10.9f, 16.6f }, // SINGLE_LIMB, -SINGLE_LIMB
        { 0, 0, 2.5f, 4.6f }, { 0, 0, 2.5f, 4.6f }, // MAX, ALT
        { 0, 0, 6.7f, 9.9f }, { 0, 0, 5.6f, 8.5f }, // MSB, LSB
        { 0, 0, 2.5f, 4.9f }, { 0, 0, 1.2f, 2.5f }, // Wide Bottom, Wide Top
        { 0, 0, 5.0f, 6.5f }, { 0, 0, 5.0f, 6.5f }, // Power of 2, Power of 8
        { 0, 0, 5.0f, 6.5f }, { 0, 0, 5.0f, 6.5f }, // Power of 10, Power of 16
        { 0, 0, 1.2f, 3.4f } // Pure Randomness
    },
    [ BI_STD_GEN ]  = {
        { 0, 0, 0.001, 0.005 }, { 0, 0, 0.004, 0.004 }, { 0, 0, 0.004, 0.004 }, // ZERO, ONE, -ONE
        { 0, 0, 0.1, 0.6 }, {0, 0, 0.1, 0.6 }, // SINGLE_LIMB, -SINGLE_LIMB
        { 0, 0, 17.8, 25.3 }, { 0, 0, 18.3, 26.7 }, // MAX, ALT
        { 0, 0, 6.3, 7.5 }, { 0, 0, 6.3, 7.5 }, // MSB, LSB
        { 0, 0, 8.7, 15.2 }, { 0, 0, 8.7, 15.2 }, // Wide Bottom, Wide Top
        { 0, 0, 3.4, 6.5 }, { 0, 0, 3.4, 6.5 }, // Power of 2, Power of 8
        { 0, 0, 3.4, 6.5 }, { 0, 0, 3.4, 6.5 }, // Power of 10, Power of 16
        { 0, 0, 0.1, 2.5 } // Pure Randomness
    },
    [ BI_RAND_GEN ] = {
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
const float cap_case_pmatrix[BIGEN_CNT][BICAP_CASE_COUNT] = {
    [ BI_CLEAN_GEN ] = { 35.6, 27.7, 18.6, 5.6, 12.5 },
    [ BI_STD_GEN ] = { 5.1, 23.3, 27.5, 19.9, 24.2 },
    [ BI_RAND_GEN ] = { 10.6, 8.8, 11.5, 31.7, 37.4 },
};
const case_prange ccase_variant_matrix[BIGEN_CNT][BICAP_CASE_COUNT] = {
    // The probabilistic bounds in QUARTERLY_SPARSE & HALF_SPRASE are the bounds for 
    // the generation of the "initial-chance" that a limb would qualify to be filled. 
    // The "initial-chance will grow/diminish dynamically based on the amount of limbs 
    // left needed to be filled, ensuring that the target (n/4, n/2) is hit.
    [ BI_CLEAN_GEN ] = {
        [ SINGULAR_LIMB ] = { 0, 1, 0.5f, 1.2f }, // The "probabilist upper-bound" in NOT used
        [ EXACT_CAPACITY ] = { 0, 0, 0.0f, 0.0f }, // Exact Capacity has no variants
        [ NEAR_CAPACITY ] = { 0, 1, 1.2f, 2.5f }, // The "probabilist upper-bound" in NOT used
        [ QUARTERLY_SPARSE ] = { 0, 0, 24.7f, 25.3f }, // The "probabilist upper-bound" in NOT used
        [ HALF_SPARSE ] = { 0, 0, 49.4f, 50.6f }, // The "probabilist upper-bound" in NOT used
    },
    [ BI_STD_GEN ] = {
        [ SINGULAR_LIMB ] = { 1, 4, 15.6f, 20.8f }, // The "probabilist upper-bound" in NOT used
        [ EXACT_CAPACITY ] = { 0, 0, 0.0f, 0.0f }, // Exact Capacity has no variants
        [ NEAR_CAPACITY ] = { 2, 6, 16.5f, 21.9f }, // The "probabilist upper-bound" in NOT used
        [ QUARTERLY_SPARSE ] = { 0, 0, 10.6f, 56.7f }, // The "probabilist upper-bound" in NOT used
        [ HALF_SPARSE ] = { 0, 0, 21.8f, 78.9f }, // The "probabilist upper-bound" in NOT used
    },
    [ BI_RAND_GEN ] = {
        [ SINGULAR_LIMB ] = { 0, 3, 8.9f, 12.2f }, // The "probabilist upper-bound" in NOT used
        [ EXACT_CAPACITY ] = { 0, 0, 0.0f, 0.0f }, // Exact Capacity has no variants
        [ NEAR_CAPACITY ] = { 0, 5, 14.7f, 21.2f }, // The "probabilist upper-bound" in NOT used
        [ QUARTERLY_SPARSE ] = { 0, 0, 18.4f, 36.6f }, // The "probabilist upper-bound" in NOT used
        [ HALF_SPARSE ] = { 0, 0, 30.6f, 64.5f }, // The "probabilist upper-bound" in NOT used
    },
};


//* ----------------------------------- FUNCTIONS ----------------------------------- *//
// Helper Functions
static void __redistribute_delta(float *prob_spect, float delta, bi_rand_mod *config) {
    // Delta here means the difference between the naive, initial allocation of chances
    // with the supposedly limit (100%) (Eg: 109.5% - 100% = 9.5% (Delta))
    // -----> Only when delta is 0, are we satisfied, since we're are exactly at the limit
    while (delta) {
        // Choose a random index to choose from
        int rand_idx = __rng_range(config->state, 0, BIGINT_CASE_COUNT - 1);
        float min = data_case_pmatrix[config->mod_gen_mode][rand_idx].low_pbound;
        float max = data_case_pmatrix[config->mod_gen_mode][rand_idx].high_pbound;
        float curr = prob_spect[rand_idx];

        // Increase or decrease the chances based on delta and the cases' limits
        if (delta > 0 && curr < max) { prob_spect[rand_idx]++; --delta; } // Decrease delta closer to 0
        else if (curr < min) { prob_spect[rand_idx]--; ++delta; } // Increase delta closer to 0
    }
}
// Component Determinators / Setup Helpers
static void _bi_alloc_spectrum(bi_rand_mod *config) {
    // 1. Normal Allocations based on matrix
    float total = 0;
    for (int i = 0; i < BIGINT_CASE_COUNT; ++i) {
        float min = data_case_pmatrix[config->mod_gen_mode][i].low_pbound;
        float max = data_case_pmatrix[config->mod_gen_mode][i].high_pbound;
        config->data_prob_spectrum[i] = __rng_frange(config->state, min, max);
        total += config->data_prob_spectrum[i];
    }
    // 2. Re-calculation against surpluses/deficits
    float delta = 100 - total; // Positive --> Add, Negative --> Subtract
    if (delta) __redistribute_delta(config->data_prob_spectrum, delta, config);
    float final_total = 0;
    for (int i = 0; i < BIGINT_CASE_COUNT; ++i) {
        final_total += config->data_prob_spectrum[i];
    } assert(final_total == 100); // Casual assert here is fine in testing
}
static void _bi_select_n(bi_rand_mod *config) {
    
}
// Write Helpers
// Main Functions
inline void bigen_init_sesh(bi_rand_mod *config, xoshiro256_state *add_state) {}
inline bigen_write(bigInt *buf, bi_rand_mod *config) {}