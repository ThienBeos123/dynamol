#ifndef IO_BASE_CASE_GEN_H
#define IO_BASE_CASE_GEN_H


#include <include.h>
#include <char_tables.h>
#include "../../test_ui/_strui.h"
#include "../../test_ui/_test_base.h"
#include "../../intrinsics/intrinsics.h"
#include "../../util/util.h"

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




//* ================================ STRING GENERATION - str_casegen.c ================================ *//
typedef enum { WHITESPACE, LEADING_ZEROS, SIGNS, BASE_PREFIX } str_areas;
typedef struct { 
    float chance; 
    uint8_t low_qbound; uint8_t high_qbound; // Quantitative Bounds
    float low_pbound; float high_pbound; // Probability Bounds
} component_prob_t;
typedef enum { CLEAN_MODE, STANDARD_MODE, FAULTY_MODE } gen_mode;
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
    gen_mode mod_gen_mode;
} str_rand_mod;

inline void strgen_init_sesh(str_rand_mod *config, bool bprefix, xoshiro256_state *add_state);
inline size_t strgen_len(void);
void strgen_write(char *buf, size_t len, str_rand_mod* config, bool bprefix);




#endif