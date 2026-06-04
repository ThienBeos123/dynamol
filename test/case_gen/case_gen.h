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


//* ================================ BIGINT GENERATION - bi_casegen.c ================================ *//
typedef enum { BI_CLEAN_GEN, BI_STD_GEN, BI_RAND_GEN, BIGEN_CNT } bi_gen_mode;
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
    CASE_ZERO,  // n = 0, sign = 1 (zero)
    CASE_ONE,   // limbs[0] = 1, n = 1, sign = 1
    CASE_RANDOM, // COMPLETELY RANDOM ON EVERY LIMB
    
    // EDGE CASES: LIMB PATTERNS
    CASE_MAX, // All limbs = 0xFFFFFFFFFFFFFFFF (max value)
    CASE_ALT,  // Limbs alternate 0x5555... and 0xAAAA...
    CASE_MSB, // All limbs have MSB = 1 (0x8000... and variants)
    CASE_LSB, // All limbs have LSB = 1 (odd values)
    
    // POWER CASES: BASE-SPECIFIC
    CASE_PO8,      // Value = 8^K (powers of octal base)
    CASE_PO10,     // Value = 10^K (powers of decimal base)
    CASE_PO16,     // Value = 16^K (powers of hex base)

    // Automatic tracking of case-counts
    BIGINT_CASE_COUNT
} bi_cases;
typedef struct { 
    uint8_t low_qbound, high_qbound;
    float low_pbound, high_pbound; 
} case_prange;

//* MAIN CONFIG STRUCT *//
typedef struct {
    xoshiro256_state state; bi_gen_mode mod_gen_mode;
    // Metadata AND Data's Probability & Distribution
    bi_cap_cases cap_case;
    size_t cap, len; float init_fill_chance;

    // Spectrum Buffers - Relatively light
    float cap_prob_spectrum[BICAP_CASE_COUNT];
    float data_prob_spectrum[BIGINT_CASE_COUNT];
} bi_rand_mod;

void bigen_init_sesh(bi_rand_mod *config, xoshiro256_state *add_state);
size_t bigen_len(void);
void bigen_write(bigInt *buf, bi_rand_mod *config);



//* ================================ STRING GENERATION - str_casegen.c ================================ *//
typedef enum { WHITESPACE, LEADING_ZEROS, SIGNS, BASE_PREFIX } str_areas;
typedef enum { STR_CLEAN_MODE, STR_STANDARD_MODE, STR_FAULTY_MODE, STR_GMODE_CNT} str_gen_mode;
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

void strgen_init_sesh(str_rand_mod *config, bool bprefix, xoshiro256_state *add_state);
size_t strgen_len(void);
void strgen_write(char *buf, size_t len, str_rand_mod* config, bool bprefix);




#endif