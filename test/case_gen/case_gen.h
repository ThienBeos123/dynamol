#ifndef IO_BASE_CASE_GEN_H
#define IO_BASE_CASE_GEN_H


#include <include.h>
#include <char_tables.h>
#include "../../test_ui/_strui.h"
#include "../../test_ui/_test_base.h"
#include "../../intrinsics/intrinsics.h"
#include "../../util/util.h"

//* ================================ GENERIC RANDOM GENERATION TOOLS ================================ *//
#define u64 uint64_t
#define STV static void
#define SLV static inline void
typedef enum {
    INTENSITY_NONE,
    INTENSITY_LOW,
    INTENSITY_HIGH,
    INTENSITY_EXTREME
} weight_level_t;

weight_level_t get_weighted_level(
    u64 seed,
    int none, int low, 
    int high, int extreme
) {
    int r = seed % 100;
    if (r < none) return INTENSITY_NONE;
    if (r < (none + low)) return INTENSITY_LOW;
    if (r < (none + low + high)) return INTENSITY_HIGH;
    return INTENSITY_EXTREME;
}

// RNG Functions
static inline u64 __rng_skrange(xoshiro256_state *state, u64 min, u64 max, float median_dist) {
    float u = __seed_to_float(state);
    float skewed_u = powf(u, median_dist);
    return min + (u64)((max - min) * skewed_u);
}
static inline u64 __rng_range(xoshiro256_state *state, u64 min, u64 max) {
    u64 range = max - min + 1, r;
    u64 limit = UINT64_MAX - (UINT64_MAX % range);
    do { r = xoshiro256pp_next(state); } while (r >= limit);
    return min + (r % range);
}
static inline float __rng_frange(xoshiro256_state *state, float min, float max) {
    float log_min = logf(min <= 0 ? 1e-7f : min);
    float log_max = logf(max);
    float r = xoshiro256pp_fnext01(state);
    return expf(log_min + r * (log_max - log_min));
}
static inline float __seed_to_float(xoshiro256_state *state) {
    u64 raw = xoshiro256pp_next(state);
    return (float)(raw >> 11) * (1.0f / 9007199254740992.0f);
}



//* ================================ BIGINT GENERATION - bi_casegen.c ================================ *//
typedef enum { BI_CLEAN_GEN, BI_EDGE_GEN, BI_MIXED_GEN } bi_gen_mode;
typedef enum {
    SINGULAR_LIMB,      // One Singular Limb ---> n = 1
    EXACT_CAPACITY,     // Every limb filled ---> n = cap
    NEAR_CAPACITY,      // Every limbed filled except the last --> n = cap - 1
    QUARTERLY_SPARSE,   // Quarter of the limbs is filled --> n = cap / 4
    HALF_SPARSE,        // Half of the limbs is filled --> n = cap / 2

    // Automatic tracking of case-counts
    BICAP_CASE_COUNT
} bi_cap_cases;
typedef enum {
    // VALID CANONICAL CASES
    CASE_ZERO,                 // n = 0, sign = 1 (zero)
    CASE_ONE,                  // limbs[0] = 1, n = 1, sign = 1
    CASE_NEGATIVE_ONE,         // limbs[0] = 1, n = 1, sign = -1
    CASE_SMALL_POSITIVE,       // Single limb, value in [2, 2^64 - 1)
    CASE_SMALL_NEGATIVE,       // Single limb, value in [2, 2^64 - 1), sign = -1
    
    // EDGE CASES: LIMB PATTERNS
    CASE_MAX,                  // All limbs = 0xFFFFFFFFFFFFFFFF (max value)
    CASE_NEGATIVE_MAX,         // -(all limbs = 0xFFFF...) (minimum value)
    CASE_ALTERNATING_PATTERN,  // Limbs alternate 0x5555... and 0xAAAA...
    CASE_HIGH_BIT_SET,         // All limbs have MSB = 1 (0x8000... and variants)
    CASE_LOW_BIT_SET,          // All limbs have LSB = 1 (odd values)
    CASE_MIXED_MAGNITUDE,      // First limbs large, last limbs small (or vice versa)
    
    // EDGE CASES: BASE-SPECIFIC
    CASE_POWER_OF_2,      // Value = 2^K (powers of binary base)
    CASE_POWER_OF_8,      // Value = 8^K (powers of octal base)
    CASE_POWER_OF_10,     // Value = 10^K (powers of decimal base)
    CASE_POWER_OF_16,     // Value = 16^K (powers of hex base)

    // Automatic tracking of case-counts
    BIGINT_CASE_COUNT
} bi_cases;
typedef struct { 
    uint8_t low_qbound, high_qbound;
    float low_pbound, high_pbound; 
} case_prange;

//* MAIN CONFIG STRUCT *//
typedef struct {
    xoshiro256_state *state;
    bi_gen_mode mod_gen_mode;

    // Metadata AND Data's Probability & Distribution
    bi_cap_cases cap_case; size_t cap; 
    bool neg; bi_cases data_case;
    // Limb-individual cases/anomalies
    float limb_zero, limb_max;
    float limb_alter, limb_power;
    float limb_rand;
} bi_rand_mod;
typedef struct {
    float cap_prob_spectrum[BICAP_CASE_COUNT];
    float data_prob_spectrum[BIGINT_CASE_COUNT];
    bi_rand_mod *mod;
} bigInt_gen_ctx;



//* ================================ STRING GENERATION - str_casegen.c ================================ *//
typedef enum { WHITESPACE, LEADING_ZEROS, SIGNS, BASE_PREFIX } str_areas;
typedef enum { STR_CLEAN_MODE, STR_STANDARD_MODE, STR_FAULTY_MODE } str_gen_mode;
typedef struct { 
    float chance; 
    uint8_t low_qbound; uint8_t high_qbound; // Quantitative Bounds
    float low_pbound; float high_pbound; // Probability Bounds
} component_prob_t;
typedef struct {
    xoshiro256_state base_state;
    size_t str_len; uint8_t base; // base?
    // Key components configuration
    bool whitespace; bool lzeros;
    bool sign; bool bprefix; bool junk;
    bool inval_digit; bool early_null;
    
    // Numerical amounts configuration
    uint8_t wscount; uint8_t lzcount;
    bool mixed_sign; bool mixed_bp; size_t bprefix_cnt;
    float junk_chance; uint8_t junk_drift; size_t max_junk_cnt; // Max Junk Count may never be reached
    float init_inval_chance; uint8_t inval_digit_cnt; // Inval Digit Count will always be reached
    uint8_t inval_digit_drift; float enull_chance;

    // Further Configuration Settings
    str_gen_mode mod_gen_mode;
} str_rand_mod;

inline void strgen_init_sesh(str_rand_mod *config, bool bprefix, xoshiro256_state *add_state);
inline size_t strgen_len(void);
void strgen_write(char *buf, size_t len, str_rand_mod* config, bool bprefix);




#endif