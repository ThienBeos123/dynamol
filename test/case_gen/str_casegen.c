#include "case_gen.h"

/* String Random Generation Requirements
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
*   - Note 2 - str_casegen.c SPECIFIC
*       +) It is expected that any usage of str_casegen.c must first use the
*          function strgen_init_sesh() to initialize a string generation instance
*          with satisfactory, entropy-filled base-state for our simple PRNG.
*
*       +) It is expected that any buffer regarding the containment of a randomly-
*          generated numerical-string instance from str_casegen.c / strgen_write()
*          must be initialized with the size of 512 BYTES. Additionally, it is preferred 
*          for the container/buffer of the randomly-generated numerical string to be newly 
*          initialized (whether stack, arena, or heap) AND/OR empty
*/

//* ----------------------------------- TYPES & CONTAINERS ----------------------------------- *//
const component_prob_t prob_matrix[STR_GMODE_CNT][8] = {
    [ STR_CLEAN_MODE ] = {
        {5.2f, 1, 1}, {0, 0, 0},
        {50.0f, 1, 1, 0.0f, 0.0f}, // Signs 
        {50.0f, 1, 1, 0.0f, 0.0f}, // Base-prefix
        {0.0f, 0, 0, 0.0f, 0.0f}, // Junk
        {0.0f, 0, 0, 0.0f, 0.0f}, // Invalid Digits
        {0.0f, 0, 0, 0.0f, 0.0f}, // Early Null
        {0.0f} // Shuffle Chance
    },
    [ STR_STANDARD_MODE ] = {
        {30.5f, 2, 4}, {27.6f, 1, 6},
        {75.0f, 1, 1, 0.0f, 0.0f}, // Signs
        {67.67f, 1, 2, 0.4f, 5.7f}, // Base-prefix
        {8.5f, 1, 3, 1.2f, 6.7f}, // Junk
        {3.3f, 1, 3, 0.001f, 0.3f}, // Invalid Digits
        {0.01f, 0, 0, 0.0001f, 0.005f}, // Early Null
        {0.00004f} // Shuffle Chance
    },
    [ STR_FAULTY_MODE ] = {
        {50.0f, 2, 10}, {50.0f, 2, 16},
        {95.0f, 1, 2, 12.34f, 45.25f}, // Sign
        {90.0f, 1, 4, 45.25f, 78.6f}, // Base-prefix
        {25.0f, 6, 72, 12.3f, 34.5f}, // Junk
        {6.5f, 1, 6, 0.001f, 0.3f}, // Invalid Digits
        {2.5f, 0, 0, 0.0025f, 0.01f}, // Early Null
        {0.05f} // Shuffle Chance
    },
};
const uint8_t drift_vector[STR_GMODE_CNT][4] = {
    [ STR_CLEAN_MODE ] = {0, 0, 0, 0},
    [ STR_STANDARD_MODE ] = {3, 6, 7, 16},
    [ STR_FAULTY_MODE ] = {1, 2, 3, 24}
};
char junk_candidates[256] = {0};
uint8_t junk_pool_size = 0;


//* ----------------------------------- FUNCTIONS ----------------------------------- *//
// Helper Functions
static inline bool __is_junk(uint8_t curr_pos, uint8_t base, uint8_t drift) {
    for (int d = 1; d < drift; d++) {
        /*       Boundary Check           Within Base - Valid Digit        */
        if ((curr_pos - d >= 0 && _VALUE_LOOKUP_SEN_[curr_pos - d] < base) 
        || (curr_pos + d <= 255 && _VALUE_LOOKUP_SEN_[curr_pos + d] < base)) return false;
    } return true;
}
static void __prep_junk_pool(uint8_t base, uint8_t drift, xoshiro256_state *state) {
    junk_pool_size = 0;
    if (base == 64) {
        for (uint8_t r = 0; r < 4; ++r) {
            uint8_t start = _ASCII_INVAL_RANGE64[r][0];
            uint8_t end   = _ASCII_INVAL_RANGE64[r][1];
            for (int i = start; i <= end; i++) {
                junk_candidates[junk_pool_size++] = _VALUE_LOOKUP_SEN_[i];
            }
        }
    } else {
        for (uint16_t i = 0; i < 256; ++i) {
            uint8_t val = _VALUE_LOOKUP_SEN_[i];
            if (val != (uint8_t)-1 && val < base) continue;
            if (__is_junk(i, base, drift)) {
                junk_candidates[junk_pool_size++] = _VALUE_LOOKUP_SEN_[i];
            }
        }
    }
}
static inline char __get_inval_digit(uint8_t base, uint8_t drift, xoshiro256_state *state) {
    uint8_t dr = (xoshiro256pp_next(state) % drift);
    return (base - 1 + dr >= 64) ?
        _DIGIT_SEN_[base - 1] + dr : 
        _DIGIT_SEN_[base - 1 + dr];
}
static inline char __get_junk(xoshiro256_state *state) {
    return junk_candidates[__rng_range(state, 0, junk_pool_size)];
}
static inline char __get_valdigit(uint8_t base, xoshiro256_state *state) {
    assert(base > 1);
    uint8_t rand_val = __rng_range(state, 0, base - 1);
    return _DIGIT_SEN_[rand_val];
}
// Component Determinators
static inline str_gen_mode _strgen_roll_gmode(xoshiro256_state *state) {
    float roll = (float)(fmod(__froll(state), 100));
    if (roll <= 20.0f) return STR_CLEAN_MODE;
    else if (roll <= 70.0f) return STR_STANDARD_MODE;
    else return STR_FAULTY_MODE;
}
SLV _strgen_rseed_include(str_rand_mod* config, bool bprefix) {
    str_gen_mode i = config->mod_gen_mode;
    float curr_roll = (float)(fmodf(__froll(&config->base_state), 100));
    // ----- 1. Collateral Component Inclusion Filling -----
    config->whitespace = (curr_roll < prob_matrix[i][0].chance) ? true : false;
    curr_roll = (float)(fmodf(__froll(&config->base_state), 100));
    config->lzeros = (curr_roll < prob_matrix[i][1].chance) ? true : false;

    // ----- 2. Small Lexical Component Inclusion Filling -----
    curr_roll = (float)(fmodf(__froll(&config->base_state), 100));
    config->sign = (curr_roll < prob_matrix[i][2].chance) ? true : false;
    if (!bprefix) {
        curr_roll = (float)(fmodf(__froll(&config->base_state), 100));
        config->bprefix = (curr_roll < prob_matrix[i][3].chance) ? true : false;
    }
    curr_roll = (float)(fmodf(__froll(&config->base_state), 100));
    config->early_null = (curr_roll < prob_matrix[i][5].chance) ? true : false;

    // ----- 3. Main Numerical Component Inclusion Filling -----
    curr_roll = (float)(fmodf(__froll(&config->base_state), 100));
    config->junk = (curr_roll < prob_matrix[i][4].chance) ? true : false;
    curr_roll = (float)(fmodf(__froll(&config->base_state), 100));
    config->inval_digit = (curr_roll < prob_matrix[i][5].chance) ? true : false;
}
SLV _strgen_rseed_quant(str_rand_mod* config, bool bprefix) {
    str_gen_mode i = config->mod_gen_mode; float curr_roll = 0;
    // ----- 1. Whitespace & Leading Zeros -----
    config->wscount = (config->whitespace) ? __rng_range(&config->base_state,
    prob_matrix[i][0].low_qbound, prob_matrix[i][0].high_qbound) : 0;
    config->lzcount = (config->lzeros) ? __rng_range(&config->base_state,
    prob_matrix[i][1].low_qbound, prob_matrix[i][1].high_qbound) : 0;

    // ----- 2. Sign, Base-prefix Counts, & Early Null -----
    float mixed_sign_prob = __rng_frange(&config->base_state, prob_matrix[i][2].low_pbound, prob_matrix[i][2].high_pbound);
    curr_roll = (float)(fmodf(__froll(&config->base_state), 100));
    config->mixed_sign = (config->sign) ? ((curr_roll < mixed_sign_prob) ? true : false) : false;
    if (!bprefix) {
        float mixed_bprefix_prob = __rng_frange(&config->base_state, 
            prob_matrix[i][3].low_pbound, 
            prob_matrix[i][3].high_pbound
        ); curr_roll = (float)(fmodf(__froll(&config->base_state), 100));
        config->mixed_bp = (config->bprefix) ? ((curr_roll < mixed_sign_prob) ? true : false) : false;
        config->bprefix_cnt = (config->bprefix) ? __rng_range(&config->base_state,
        prob_matrix[i][3].low_qbound, prob_matrix[i][3].high_qbound) : 0;
    }
    config->enull_chance = (config->early_null) ? __rng_frange(&config->base_state,
    prob_matrix[i][6].low_pbound, prob_matrix[i][6].high_pbound) : 0;

    // ------ 3. Junk Seeding ------
    config->junk_chance = (config->junk) ? __rng_frange(&config->base_state, 
    prob_matrix[i][4].low_pbound, prob_matrix[i][4].high_pbound) : 0;
    config->max_junk_cnt = (config->junk) ? __rng_range(&config->base_state,
    prob_matrix[i][4].low_qbound, prob_matrix[i][4].high_qbound) : 0;
    config->junk_drift = (config->junk) ? __rng_range(&config->base_state,
    drift_vector[i][2], drift_vector[i][3]) : 0;
    
    // ------ 4. Invalid Digit Seeding ------
    config->inval_digit_cnt = (config->inval_digit) ? __rng_range(&config->base_state,
    prob_matrix[i][5].low_qbound, prob_matrix[i][5].high_qbound) : 0;
    config->init_inval_chance = (config->inval_digit) ? min(
        __rng_frange(&config->base_state, prob_matrix[i][5].low_pbound, prob_matrix[i][5].high_pbound), 
        (float)(config->inval_digit_cnt / config->str_len)
    ) : 0;
    config->inval_digit_drift = (config->inval_digit) ? 
    __rng_range(&config->base_state, drift_vector[i][0], drift_vector[i][1]) : 0;

}
SLV _strgen_bias_shuffle(str_rand_mod *config, str_areas *order) {
    float roll = xoshiro256pp_fnext01(&config->base_state);
    str_gen_mode i = config->mod_gen_mode;
    if (roll > prob_matrix[i][7].chance) return;
    roll = xoshiro256pp_fnext01(&config->base_state);
    if (roll < 0.6f) {
        str_areas tmp = order[WHITESPACE];
        order[WHITESPACE] = order[SIGNS];
        order[SIGNS] = tmp;
    } if (roll < 0.9f) {
        str_areas tmp = order[BASE_PREFIX];
        order[BASE_PREFIX] = order[LEADING_ZEROS];
        order[LEADING_ZEROS] = tmp;
    } else {
        for (uint8_t i = 3; i > 0; --i) {
            uint64_t j = __rng_range(&config->base_state, 0, i);
            str_areas tmp = order[i];
            order[i] = order[j];
            order[j] = tmp;
        }
    }
}
// Write Helpers
STV _strgen_write_ws_(char *buf, size_t len, str_rand_mod* config, size_t *cursor, bool *term) {
    if (!config->whitespace) return; 
    float enull_roll;
    for (size_t i = 0; i < config->wscount; ++i) {
        if (*cursor >= config->str_len) { *term = true; return; }
        enull_roll = (float)(fmodf(__froll(&config->base_state), 100));
        if (config->early_null && enull_roll < config->enull_chance) {
            buf[*cursor] = '\0'; *term = true; return;
        } buf[*cursor] = ' '; *cursor++;
    }
}
STV _strgen_write_sign_(char *buf, size_t len, str_rand_mod* config, size_t *cursor, bool *term) {
    if (!config->sign) return;
    if (*cursor >= config->str_len) { *term = true; return; }
    float enull_roll = (float)(fmodf(__froll(&config->base_state), 100));
    if (config->early_null && enull_roll < config->enull_chance) { 
        buf[*cursor] = '\0'; *term = true; return;
    } int8_t sign = (__rng_range(&config->base_state, 1, 2) == 1) ? 1 : -1;
    buf[*cursor] = (sign == 1) ? '+' : '-'; *cursor++;

    if (config->mixed_sign) {
        if (*cursor >= config->str_len) { *term = true; return; }
        enull_roll = (float)(fmodf(__froll(&config->base_state), 100));
        if (config->early_null && enull_roll < config->enull_chance) { 
            buf[*cursor] = '\0'; *term = true; return;
        } buf[*cursor] = (sign == 1) ? '-' : '+'; *cursor++;
    }
}
STV _strgen_write_lz_(char *buf, size_t len, str_rand_mod* config, size_t *cursor, bool *term) {
    if (!config->lzeros) return;
    float enull_roll;
    for (size_t i = 0; i < config->lzcount; ++i) {
        if (*cursor >= config->str_len) { *term = true; return; }
        enull_roll = (float)(fmodf(__froll(&config->base_state), 100));
        if (config->early_null && enull_roll < config->enull_chance) {
            buf[*cursor] = '\0'; *term = true; return;
        } buf[*cursor] = '0'; *cursor++;
    }
}
STV _strgen_write_bprefix_(char *buf, size_t len, str_rand_mod* config, size_t *cursor, bool *term) {
    if (!config->bprefix) return;
    if (*cursor >= config->str_len) { *term = true; return; }
    float enull_roll = (float)(fmodf(__froll(&config->base_state), 100));
    if (config->early_null && enull_roll < config->enull_chance) { 
        buf[*cursor] = '\0'; *term = true; return;
    } uint8_t upsub = (__rng_range(&config->base_state, 1, 10) == 1) ? 32 : 0;
    buf[*cursor] = '0'; *cursor++;
    switch (config->base) {
        case 2: {
            if (*cursor >= config->str_len) { *term = true; return; }
            if (config->early_null && enull_roll < config->enull_chance) { 
                buf[*cursor] = '\0'; *term = true; return;
            } buf[*cursor] = 'b' - upsub; *cursor++; break;
        }
        case 8: {
            if (*cursor >= config->str_len) { *term = true; return; }
            if (config->early_null && enull_roll < config->enull_chance) { 
                buf[*cursor] = '\0'; *term = true; return;
            } buf[*cursor] = 'o' - upsub; *cursor++; break;
        }
        case 16: {
            if (*cursor >= config->str_len) { *term = true; return; }
            if (config->early_null && enull_roll < config->enull_chance) { 
                buf[*cursor] = '\0'; *term = true; return;
            } buf[*cursor] = 'x' - upsub; *cursor++; break;
        }
        case 10: break;
        default: {
            uint8_t tmp = config->base;
            if (*cursor >= config->str_len) { *term = true; return; }
            if (config->early_null && enull_roll < config->enull_chance) { 
                buf[*cursor] = '\0'; *term = true; return;
            } buf[*cursor] = '{'; *cursor++;
            while (tmp) {
                if (*cursor >= config->str_len) { *term = true; return; }
                if (config->early_null && enull_roll < config->enull_chance) { 
                    buf[*cursor] = '\0'; *term = true; return;
                } buf[*cursor] = '0' - (tmp % 10); 
                *cursor++; 
            }
            if (*cursor >= config->str_len) { *term = true; return; }
            if (config->early_null && enull_roll < config->enull_chance) { 
                buf[*cursor] = '\0'; *term = true; return;
            } buf[*cursor] = '}'; *cursor++;
        } break;
    }
    if (config->mixed_bp) {
        for (uint8_t i = 0; i < config->bprefix_cnt; ++i) {
            uint8_t rand_base = __rng_range(&config->base_state, 1, 64),
            upsub = (__rng_range(&config->base_state, 1, 10) == 1) ? 32 : 0;
            if (*cursor >= config->str_len) { *term = true; return; }
            if (config->early_null && enull_roll < config->enull_chance) { 
                buf[*cursor] = '\0'; *term = true; return;
            } buf[*cursor] = '0'; *cursor++;
            switch (rand_base) {
                case 2: {
                    if (*cursor >= config->str_len) { *term = true; return; }
                    if (config->early_null && enull_roll < config->enull_chance) { 
                        buf[*cursor] = '\0'; *term = true; return;
                    } buf[*cursor] = 'b' - upsub; *cursor++; break;
                }
                case 8: {
                    if (*cursor >= config->str_len) { *term = true; return; }
                    if (config->early_null && enull_roll < config->enull_chance) { 
                        buf[*cursor] = '\0'; *term = true; return;
                    } buf[*cursor] = 'o' - upsub; *cursor++; break;
                }
                case 16: {
                    if (*cursor >= config->str_len) { *term = true; return; }
                    if (config->early_null && enull_roll < config->enull_chance) { 
                        buf[*cursor] = '\0'; *term = true; return;
                    } buf[*cursor] = 'x' - upsub; *cursor++; break;
                }
                case 10: break;
                default: {
                    if (*cursor >= config->str_len) { *term = true; return; }
                    if (config->early_null && enull_roll < config->enull_chance) { 
                        buf[*cursor] = '\0'; *term = true; return;
                    } buf[*cursor] = '{'; *cursor++;
                    while (rand_base) {
                        if (*cursor >= config->str_len) { *term = true; return; }
                        if (config->early_null && enull_roll < config->enull_chance) { 
                            buf[*cursor] = '\0'; *term = true; return;
                        } buf[*cursor] = '0' - (rand_base % 10); 
                        *cursor++; 
                    }
                    if (*cursor >= config->str_len) { *term = true; return; }
                    if (config->early_null && enull_roll < config->enull_chance) { 
                        buf[*cursor] = '\0'; *term = true; return;
                    } buf[*cursor] = '}'; *cursor++;
                } break;
            }
        }
    }
}
STV _strgen_write_num_(char *buf, size_t len, str_rand_mod* config, size_t *cursor, bool *term) {
    float enull_roll, junk_roll, inval_roll;
    // We roll for special-cases from least-likely --> most-likely
    for (; *cursor < config->str_len; *cursor++) {
        if (*cursor >= config->str_len) { *term = true; return; }
        // Rolling for Early Null
        if (config->early_null) {
            enull_roll = (float)(fmodf(__froll(&config->base_state), 100));
            if (enull_roll < config->enull_chance) { buf[*cursor] = '\0'; *term = true; return; }
        }
        // Rolling for Invalid Digit
        if (config->inval_digit && config->inval_digit_cnt) {
            inval_roll = (float)(fmodf(__froll(&config->base_state), 100));
            if (inval_roll < config->init_inval_chance) {
                buf[*cursor] = __get_inval_digit(
                    config->base, 
                    config->inval_digit_drift, 
                    &config->base_state
                ); config->inval_digit_cnt--;
                size_t digit_left = (config->str_len - *cursor - 1);
                config->init_inval_chance = (float)(config->inval_digit_cnt / digit_left);
                *cursor++; continue;
            }
        }
        // Rolling for Junk
        if (config->junk && config->max_junk_cnt) {
            junk_roll = (float)(fmodf(__froll(&config->base_state), 100));
            if (junk_roll < config->junk_chance) {
                buf[*cursor] = __get_junk(&config->base_state);
                config->max_junk_cnt--; *cursor++;
                continue;
            }
        } buf[*cursor] = __get_valdigit(config->base, &config->base_state); 
        *cursor++;
    }
}
// Main Functions
inline void strgen_init_sesh(str_rand_mod *config, bool bprefix, xoshiro256_state *add_state) {
    // Generation of State
    uint64_t side_mix = 0;
    config->base_state = (xoshiro256_state){0};
    __GET_ENTROPY_FAST(config->base_state.s, sizeof(uint64_t) << 2);
    __GET_ENTROPY_FAST(&side_mix, sizeof(uint64_t));
    seed_xoshiro256(&config->base_state, side_mix);
    // Base-prefix Option handling
    if (!bprefix) config->bprefix = false;
    // State Mixing --> Further randomization
    config->base_state = mix_xoshiro256(&config->base_state, add_state);
    xoshiro256pp_next(&config->base_state); // Further scramble
}
inline size_t strgen_len(void) { return STR_CAP; }
void strgen_write(char *buf, size_t len, str_rand_mod* config, bool bprefix) {
    // Setup
    config->mod_gen_mode = _strgen_roll_gmode(&config->base_state);
    config->str_len = __rng_skrange(&config->base_state, 0, strgen_len(), 0.7f);
    config->base = __rng_range(&config->base_state, STR_MIN_BASE, STR_MAX_BASE);
    _strgen_rseed_include(config, bprefix); _strgen_rseed_quant(config, bprefix);
    str_areas order[4] = {WHITESPACE, SIGNS, BASE_PREFIX, LEADING_ZEROS};
    _strgen_bias_shuffle(config, order); size_t cursor = 0;
    bool end_signal = false;

    // Main writing loop
    for (uint8_t i = 0; i < 5; ++i) {
        if (end_signal || cursor >= config->str_len) break;
        if (i < 5) { switch (order[i]) {
                case WHITESPACE: _strgen_write_ws_(buf, len, config, &cursor, &end_signal);
                case SIGNS: _strgen_write_sign_(buf, len, config, &cursor, &end_signal);
                case BASE_PREFIX: _strgen_write_bprefix_(buf, len, config, &cursor, &end_signal);
                case LEADING_ZEROS: _strgen_write_lz_(buf, len, config, &cursor, &end_signal);
            }
        } else {
            __prep_junk_pool(config->base, config->junk_drift, &config->base_state);
            _strgen_write_num_(buf, len, config, &cursor, &end_signal);
            memset(junk_candidates, '\0' /* or just 0 */, 256); junk_pool_size = 0;
        }
    }
}
