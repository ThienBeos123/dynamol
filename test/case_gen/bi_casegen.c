/*
Copyright (C) 2026 @ThienBeos123/@Poly-glon

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

  http://apache.org

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/



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
        { 0, 0, 2.5f, 3.0f }, { 0, 0, 4.6f, 7.4f }, // ZERO, ONE
        { 0, 0, 1.2f, 3.4f }, // Pure Randomness
        { 0, 0, 2.5f, 4.6f }, { 0, 0, 2.5f, 4.6f }, // MAX, ALT
        { 0, 0, 6.7f, 9.9f }, { 0, 0, 5.6f, 8.5f }, // MSB, LSB
        { 0, 0, 5.0f, 6.5f }, { 0, 0, 5.0f, 6.5f }, { 0, 0, 5.0f, 6.5f }, // Power 8, 10, 16
    },
    [ BI_STD_GEN ]  = {
        { 0, 0, 0.001, 0.005 }, { 0, 0, 0.004, 0.004 }, // ZERO, ONE
        { 0, 0, 0.1, 2.5 }, // Pure Randomness
        { 0, 0, 17.8, 25.3 }, { 0, 0, 18.3, 26.7 }, // MAX, ALT
        { 0, 0, 6.3, 7.5 }, { 0, 0, 6.3, 7.5 }, // MSB, LSB
        { 0, 0, 3.4, 6.5 }, { 0, 0, 3.4, 6.5 }, { 0, 0, 3.4, 6.5 }, // Power of 8, 10, 16
    },
    [ BI_RAND_GEN ] = {
        { 0, 0, 0.01, 0.05 }, { 0, 0, 0.05, 0.09 }, // ZERO, ONE
        { 0, 0, 55.6, 66.6 }, // Pure Randomness
        { 0, 0, 7.8, 9.4 }, { 0, 0, 10.2, 15.8 }, // MAX, ALT
        { 0, 0, 0.9, 1.5 }, { 0, 0, 0.9, 1.5 }, // MSB, LSB
        { 0, 0, 2.3, 5.5 }, { 0, 0, 2.3, 5.5 }, { 0, 0, 2.3, 5.5 }, // Power 8, 10, 16
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
        [ SINGULAR_LIMB ] = { 0, 1, 0.5f, 0.0f }, // The "probabilist upper-bound" in NOT used
        [ EXACT_CAPACITY ] = { 0, 0, 0.0f, 0.0f }, // Exact Capacity has no variants
        [ NEAR_CAPACITY ] = { 0, 1, 1.2f, 0.0f }, // The "probabilist upper-bound" in NOT used
        [ QUARTERLY_SPARSE ] = { 0, 0, 24.7f, 25.3f },
        [ HALF_SPARSE ] = { 0, 0, 49.4f, 50.6f },
    },
    [ BI_STD_GEN ] = {
        [ SINGULAR_LIMB ] = { 1, 3, 15.6f, 0.0f }, // The "probabilist upper-bound" in NOT used
        [ EXACT_CAPACITY ] = { 0, 0, 0.0f, 0.0f }, // Exact Capacity has no variants
        [ NEAR_CAPACITY ] = { 2, 4, 16.5f, 0.0f }, // The "probabilist upper-bound" in NOT used
        [ QUARTERLY_SPARSE ] = { 0, 0, 10.6f, 56.7f },
        [ HALF_SPARSE ] = { 0, 0, 21.8f, 78.9f },
    },
    [ BI_RAND_GEN ] = {
        [ SINGULAR_LIMB ] = { 0, 3, 8.9f, 0.0f }, // The "probabilist upper-bound" in NOT used
        [ EXACT_CAPACITY ] = { 0, 0, 0.0f, 0.0f }, // Exact Capacity has no variants
        [ NEAR_CAPACITY ] = { 0, 5, 14.7f, 0.0f }, // The "probabilist upper-bound" in NOT used
        [ QUARTERLY_SPARSE ] = { 0, 0, 18.4f, 36.6f },
        [ HALF_SPARSE ] = { 0, 0, 30.6f, 64.5f },
    },
};
const size_t cap_skewness_vector[BIGEN_CNT] = { 50, 85, 75 };


//* ----------------------------------- FUNCTIONS ----------------------------------- *//
// Lowly Helper Functions
static void __redistribute_delta(float *prob_spect, float delta, bi_rand_mod *config) {
    // Delta here means the difference between the naive, initial allocation of chances
    // with the supposedly limit (100%) (Eg: 109.5% - 100% = 9.5% (Delta))
    // -----> Only when delta is 0, are we satisfied, since we're are exactly at the limit
    while (delta) {
        // Choose a random index to choose from
        int rand_idx = __rng_range(&config->state, 0, BIGINT_CASE_COUNT - 1);
        float min = data_case_pmatrix[config->mod_gen_mode][rand_idx].low_pbound;
        float max = data_case_pmatrix[config->mod_gen_mode][rand_idx].high_pbound;
        float curr = prob_spect[rand_idx];

        // Increase or decrease the chances based on delta and the cases' limits
        if (delta > 0 && curr < max) { prob_spect[rand_idx]++; --delta; } // Decrease delta closer to 0
        else if (curr < min) { prob_spect[rand_idx]--; ++delta; } // Increase delta closer to 0
    }
}
static inline void __ccase_singular_limb(bi_rand_mod *config) {
    float roll = (float)(fmod(__froll(&config->state), 100)); size_t add_size = 0;
    if (roll <= ccase_variant_matrix[config->mod_gen_mode][config->cap_case].low_pbound) {
        add_size = (size_t)(__rng_range(&config->state,
            ccase_variant_matrix[config->mod_gen_mode][config->cap_case].low_qbound,
            ccase_variant_matrix[config->mod_gen_mode][config->cap_case].high_qbound
        ));
    }
    if (1 + add_size > config->cap) config->len = config->cap;
    else config->len = 1 + add_size;
    config->init_fill_chance = 100;
}
static inline void __ccase_near_cap(bi_rand_mod *config) {
    float roll = (float)(fmod(__froll(&config->state), 100)); size_t sub_size = 0;
    if (roll <= ccase_variant_matrix[config->mod_gen_mode][config->cap_case].low_pbound) {
        sub_size = (size_t)(__rng_range(&config->state,
            ccase_variant_matrix[config->mod_gen_mode][config->cap_case].low_qbound,
            ccase_variant_matrix[config->mod_gen_mode][config->cap_case].high_qbound
        ));
    }
    if (config->cap - 1 - sub_size <= 0) config->len = 1;
    else config->len = config->cap - 1 - sub_size;
}
static inline float __data_shift_skew(bi_gen_mode gmode) {
    switch (gmode) {
        case BI_CLEAN_GEN: return 5.0f; break;
        case BI_STD_GEN: return 15.0f; break;
        case BI_RAND_GEN: return 50.0f; break;
        case BIGEN_CNT: return 5.0f; break;
        default: return 5.0f; break;
        /* The final case is to silent compiler warnings */
        /* We explicitly return the value of CLEAN mode to minimize errors */
    } return 5.0f; /* Same, just for safety */
}
static int __roulette_wheel_select(const float *spect, xoshiro256_state *state) {
    float roll = (float)(fmod(__froll(state), 100));
    int i = 0; while (roll > 0) {
        roll -= spect[i]; ++i;
    }  return i;
}
// Component Determinators / Setup Helpers
static inline bi_gen_mode _bigen_roll_gmode(xoshiro256_state *state) {
    float roll = (float)(fmod(__froll(state), 100));
    if (roll <= 20.0f) return BI_CLEAN_GEN;
    else if (roll <= 60.0f) return BI_STD_GEN;
    else return BI_RAND_GEN;
}
static void _alloc_spectrum(bi_rand_mod *config) {
    // 1. Normal Allocations based on matrix
    float total = 0;
    for (int i = 0; i < BIGINT_CASE_COUNT; ++i) {
        float min = data_case_pmatrix[config->mod_gen_mode][i].low_pbound;
        float max = data_case_pmatrix[config->mod_gen_mode][i].high_pbound;
        config->data_prob_spectrum[i] = __rng_frange(&config->state, min, max);
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
static inline void _rseed_cap_metadata(bi_rand_mod *config) {
    if (!config->cap) { config->len = 0; return; }
    config->cap_case = __roulette_wheel_select(
        cap_case_pmatrix[config->mod_gen_mode], 
        &config->state
    );
    switch(config->cap_case) {
        case SINGULAR_LIMB: __ccase_singular_limb(config); break;
        case EXACT_CAPACITY: { config->len = config->cap; config->init_fill_chance = 100; break; }
        case NEAR_CAPACITY: __ccase_near_cap(config); break;
        case QUARTERLY_SPARSE: {
            config->init_fill_chance = __rng_frange(&config->state,
                ccase_variant_matrix[config->mod_gen_mode][config->cap_case].low_pbound,
                ccase_variant_matrix[config->mod_gen_mode][config->cap_case].high_pbound
            ); config->len = (config->cap >> 2) + 1;
        } break;
        case HALF_SPARSE: {
            config->init_fill_chance = __rng_frange(&config->state,
                ccase_variant_matrix[config->mod_gen_mode][config->cap_case].low_pbound,
                ccase_variant_matrix[config->mod_gen_mode][config->cap_case].high_pbound
            ); config->len = (config->cap >> 1) + 1;
        } break;
        /* These two cases are purely for safety purposes, in which we do the most deterministic case for them */
        case BICAP_CASE_COUNT: { config->len = config->cap; config->init_fill_chance = 100; break; }
        default: { config->len = config->cap; config->init_fill_chance = 100; break; }
    }
}
// Write Helpers
static inline uint64_t _biwrite_max(bi_rand_mod *config) {
    uint8_t shift = (uint8_t)__rng_skrange(&config->state, 0, 56, __data_shift_skew(config->mod_gen_mode));
    uint8_t dir = (uint8_t)__rng_range(&config->state, 0, 1);
    return (dir) ? UINT64_MAX << shift : UINT64_MAX >> shift;
}
static inline uint64_t _biwrite_alt(bi_rand_mod *config) {
    uint8_t roll = (uint8_t)__rng_range(&config->state, 1, 14);
    switch (roll) {
        case 1: return UINT64_C(0x1111111111111111);
        case 2: return UINT64_C(0x2222222222222222);
        case 3: return UINT64_C(0x3333333333333333);
        case 4: return UINT64_C(0x4444444444444444);
        case 5: return UINT64_C(0x5555555555555555);
        case 6: return UINT64_C(0x6666666666666666);
        case 7: return UINT64_C(0x7777777777777777);
        case 8: return UINT64_C(0x8888888888888888);
        case 9: return UINT64_C(0x9999999999999999);
        case 10: return UINT64_C(0xAAAAAAAAAAAAAAAA);
        case 11: return UINT64_C(0xBBBBBBBBBBBBBBBB);
        case 12: return UINT64_C(0xCCCCCCCCCCCCCCCC);
        case 13: return UINT64_C(0xDDDDDDDDDDDDDDDD);
        case 14: return UINT64_C(0xEEEEEEEEEEEEEEEE);
    } return UINT64_MAX;
}
static inline uint64_t _biwrite_msb(bi_rand_mod *config) {
    uint8_t shift = (uint8_t)__rng_skrange(
        &config->state, 0, 63,
        __data_shift_skew(config->mod_gen_mode)
    ); uint64_t val = UINT64_C(0x8000000000000000);
    return val >> shift;
}
static inline uint64_t _biwrite_lsb(bi_rand_mod *config) {
    uint8_t shift = (uint8_t)__rng_skrange(
        &config->state, 0, 63, 
        __data_shift_skew(config->mod_gen_mode)
    ); return 1 << shift;
}
static inline uint64_t _biwrite_power(xoshiro256_state *state, uint8_t base, uint8_t max_exp) {
    uint8_t exp = __rng_range(state, 0, max_exp);
    return ((exp) ? _dnml_ipower_u64(base, exp) : 1);
}
// Main Functions
void bigen_init_sesh(bi_rand_mod *config, xoshiro256_state *add_state) {
    // Generation of State
    uint64_t side_mix = 0;
    config->state = (xoshiro256_state){0};
    __GET_ENTROPY_FAST(&config->state.s, sizeof(uint64_t) << 2);
    __GET_ENTROPY_FAST(&side_mix, sizeof(uint64_t));
    seed_xoshiro256(&config->state, side_mix);
    // State Mixing --> Further Randomization
    config->state = mix_xoshiro256(&config->state, add_state);
    xoshiro256pp_next(&config->state); // Further scramble
}
size_t bigen_len(void) { return BIGINT_CAP; }
void bigen_write(bigInt *buf, bi_rand_mod *config) {
    // Setup
    config->mod_gen_mode = _bigen_roll_gmode(&config->state);
    config->cap = __rng_skrange(
        &config->state, 0, bigen_len(), 
        cap_skewness_vector[config->mod_gen_mode]
    ); size_t i = 0;
    _rseed_cap_metadata(config); _alloc_spectrum(config);

    // Main Writing Loop
    buf->cap = config->cap; buf->n = config->len;
    while (i < config->len) {
        int dcase = __roulette_wheel_select(config->data_prob_spectrum, &config->state);
        switch (dcase) {
            // Small/Random Cases
            case CASE_ZERO: { if (i == config->len - 1) continue; buf->limbs[i] = 0; } break;
            case CASE_ONE: { buf->limbs[i] = 1; } break;
            case CASE_RANDOM: { 
                uint64_t rand_num = xoshiro256pp_next(&config->state); 
                if (!rand_num && i == config->len - 1) continue;
                buf->limbs[i] = rand_num;
            } break;
            // Edge Cases
            case CASE_MAX: { buf->limbs[i] = _biwrite_max(config); } break;
            case CASE_ALT: { buf->limbs[i] = _biwrite_alt(config); } break;
            case CASE_MSB: { buf->limbs[i] = _biwrite_msb(config); } break;
            case CASE_LSB: { buf->limbs[i] = _biwrite_lsb(config); } break;
            // Power Cases
            case CASE_PO8: { buf->limbs[i] = _biwrite_power(&config->state, 8, 21); } break;
            case CASE_PO10: { buf->limbs[i] = _biwrite_power(&config->state, 10, 19); } break;
            case CASE_PO16: { buf->limbs[i] = _biwrite_power(&config->state, 16, 15); } break;
        } ++i;
    }
    if (config->len) buf->sign = (__rng_range(&config->state, 0, 1)) ? 1 : -1;
    else buf->sign = 1;
    __BIGINT_INTERNAL_TRIM_LZ__(buf);
    if (buf->n == 0) buf->sign = 1;
}