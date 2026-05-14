// STDLIB & /include Utilities
#include <include.h>
#include <system/sys.h>
// Main Components
#include "../../../test_ui/_strui.h"
#include "../../case_gen/case_gen.h"
#include "../_ioconv.h"
// Function wrappers
#include "bi_indef.h"
#include "bi_setup.h"
#include "bi_exec_func.h"
#include "bi_eval_fn.h"
#include "bi_util_func.h"
// Functions to be tested
#include "../../../adynamol/big_numbers/bigInt_func.h"
// Miscallenous Utilities
#include "../../../util/util.h"
#include "../../../intrinsics/intrinsics.h"

/* Note - STORAGE SETUP: 
*   +) Further intialization protocols for fail_ebuf parts (str_res)
*      is not needed in STOBI Function groups, as they utilize ectx_buf
*      as the storage unit for their bigInt results, and therefore the size
*      would be accurate for storage since we're using pointer-based storage
*      instead of header-based FAM storage
* 
*   +) However, for BITOS Function groups, it is absolutely necessary 
*      to declare the failure-handling storage for edge cases to be raw
*      bytes (uint8_t preferred for cross platform compatibility, but
*      unsigned char is partly acceptable), initializing with the formula:
* 
*           bufsize = ((sizeof(str_res) + MAXIMUM_CAP) * *_ecount << 1) * *_scount
* 
*      Where:
*           - bufsize: The size of the failure-handling buffer that contains both the
*                      storage of expected output and output
*           - MAXIMUM_CAP: The upperbound/most memory usage an edge case could've possible used
*                          for its output. Setting it as STR_CAP is only acceptable during
*                          prototypical stages, and must be replaced with the correct, peak memory
*                          usage that a case can use for its output
*           - *_ecount: The amount of edge cases for specific operation 
*                       (replace * with the operation name) 
*           - *_scount: The amount of suites that is needed to holistically test the entire function
*                       category (containing all of its variants)
*     
*      Afterwards, usage can be either implicitly interpreted as spaces of a str_res struct header,
*      for _strui.h automatically interprets such inputted buffers for edge case as str_res, or,
*      preferrably, explicit through type-casting.
*/


// INPUT DATA STORAGE SITE
limb_t zero = 0, one = 1;
limb_t small_mulval[35] = {
    255, // ecases_nob Case 4
    UINT16_C(999), // ecases_nob Case 5
    UINT64_MAX, // ecases_nob Case 6 & 7 
    UINT32_C(1000000000), // ecases_nob Case 13
};
limb_t case_8[2] = { 0, 1 }; // 2^64
limb_t case_9[2] = { 1, 1 }; // 2^64 + 1
limb_t case_10[2] = { UINT64_MAX, UINT64_MAX };
limb_t case_12[3] = { UINT64_C(0x1234567890ABCDEF), UINT64_C(0xFEDCBA0987654321), 1 };
limb_t case_15[2] = { UINT64_C(0x5555555555555555), UINT64_C(0xAAAAAAAAAAAAAAAA) };
limb_t case_16[2] = { UINT64_C(0x8000000000000000), UINT64_C(0x8000000000000000) };


scase ecases_nob[32] = {
    /* ------------------------------------------------------------------------------------------------------------ */
    /* Case Number  | Input                                     | Expected Ouput                                    */
    /* ---------------------------------------------- TRIVIAL CASES ----------------------------------------------- */
    { /* 1          | 0 (n = 0)                                 | "0"                                               */
        .in = &(bitos_conv_in){ 
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &zero, .n = 0, .cap = 1, .sign = 1 }
        },
        .exp = { .type = STRING, .data.len = 1, .cap = 1, .pstr = "0" }
    }, { /* 2       | 1 (n = 1)                                 | "1"                                               */
        .in = &(bitos_conv_in){ 
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &one, .n = 1, .cap = 1, .sign = 1 }
        },
        .exp = { .type = STRING, .data.len = 1, .cap = 1, .pstr = "1" }
    }, { /* 3       | -1 (n = 1, sign = -1)                     | "-1"                                              */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &one, .n = 1, .cap = 1, .sign = -1 }
        },
        .exp = { .type = STRING, .data.len = 2, .cap = 2, .pstr = "-1" }
    }, { /* 4       | 255 (n = 1)                               | "255"                                             */
        .in = &(bitos_conv_in){ 
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &small_mulval[0], .n = 1, .cap = 1, .sign = 1 }
        },
        .exp = { .type = STRING, .data.len = 3, .cap = 3, .pstr = "255" }
    }, { /* 5       | -999 (n = 1, sign = -1)                   | "-999"                                            */
        .in = &(bitos_conv_in){ 
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &small_mulval[1], .n = 1, .cap = 1, .sign = -1 }
        },
        .exp = { .type = STRING, .data.len = 4, .cap = 4, .pstr = "-999" }
    }, { /* 6       | 2^64 - 1 (n = 1)                          | "18446744073709551616"                            */
        .in = &(bitos_conv_in){ 
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &small_mulval[2], .n = 1, .cap = 1, .sign = 1 }
        },
        .exp = { .type = STRING, .data.len = 20, .cap = 20, .pstr = "18446744073709551616" }
    }, 
    { /* 7          | -(2^64 - 1) (n = 1, sign = -1)            | "-18446744073709551616"                           */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &small_mulval[2], .n = 1, .cap = 1, .sign = -1 }
        },
        .exp = { .type = STRING, .data.len = 21, .cap = 21, .pstr = "-18446744073709551616" }
    },
    /* ----------------------------------------------- EDGE CASES ------------------------------------------------- */
    { /* 8          | 2^64 (n = 2)                              | "18446744073709551617"                            */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_8, .cap = 2, .n = 2, .sign = 1 }
        },
        .exp = { .type = STRING, .cap = 20, .data.len = 20, .pstr = "18446744073709551617" }
    }, { /* 9       | 2^64 + 1 (n = 2)                          | "18446744073709551618"                            */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_9, .cap = 2, .n = 2, .sign = 1 }
        },
        .exp = { .type = STRING, .cap = 20, .data.len = 20, .pstr = "18446744073709551618" }
    }, { /* 10      | 2^128 - 1 (n = 2)                         | "340282366920938463463374607431768211455"         */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_10, .cap = 2, .n = 2, .sign = 1 }
        },
        .exp = { 
            .type = STRING, .cap = 39, .data.len = 39, 
            .pstr = "340282366920938463463374607431768211455" 
        }
    }, { /* 11      | -(2^128 - 1) (n = 2)                      | "-340282366920938463463374607431768211455"        */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_10, .cap = 2, .n = 2, .sign = -1 }
        },
        .exp = { 
            .type = STRING, .cap = 40, .data.len = 40, 
            .pstr = "-340282366920938463463374607431768211455" 
        }
    }, { /* 12      | idk Random ig (n = 3)                     | "-679052356442...456469093871" (truncated)        */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_12, .cap = 3, .n = 3, .sign = 1 }
        },
        .exp = { 
            .type = STRING, .cap = 39, .data.len = 39, 
            .pstr = "679052356442327393940567539456469093871" 
        }
    }, { /* 13      | 1000000000 (n = 1)                        | "1000000000"                                      */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &small_mulval[3], .cap = 1, .n = 1, .sign = 1 }
        },
        .exp = { .type = STRING, .cap = 10, .data.len = 10,  .pstr = "1000000000" }
    }, { /* 14      | -1000000000 (n = 1, sign = -1)            | "-1000000000"                                     */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &small_mulval[3], .cap = 1, .n = 1, .sign = -1 }
        },
        .exp = { .type = STRING, .cap = 11, .data.len = 11,  .pstr = "-1000000000" }
    }, { /* 15      | Alt Limbs (n = 2)                         | "226854911280...263275623765" (truncated)         */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_15, .cap = 2, .n = 2, .sign = 1 }
        },
        .exp = { 
            .type = STRING, .cap = 39, .data.len = 39,
            .pstr = "226854911280625642302767490263275623765"
        }
    }, { /* 16      | MSB Limbs (n = 2)                         | "170141183460...752738881536" (truncated)         */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_16, .cap = 2, .n = 2, .sign = 1 }
        },
        .exp = {
            .type = STRING, .cap = 39, .data.len = 39,
            .pstr = "170141183460469231740910675752738881536"
        }
    },
};
scase ecases_b[35] = {
    /* -------------------------------------------------------------------------------------------- */
    /* Case Number  |   Input               |   Base        |   Expected Ouput                      */
};


// Main Code
int main(int argc, char **argv) {
    //* ---------------------------------- PRE-TEST SETUP ---------------------------------- *//
    // Parse terminal args + Setup env constants
    u16 rcount = (argc >= 1) ? (u16)(_stou64(argv[1], strlen(argv[1]))) : 100;
    _dnml_output_mode conv_omode; if (argc >= 2) {
        u8 sesh_count = _stou64(argv[2], strlen(argv[2]));
        conv_omode = (sesh_count <= 3) ? DNML_VOUT : DNML_COUT;
    } else conv_omode = DNML_VOUT;
    u8 conv_ecount = 35, conv_scount = 4;

    // Edge-case Buffer Setup
    char ectx_buf[19]; // Edge-case Memory Usage: 152 bytes
    str_res *ebuf_slices[conv_scount], fail_ebuf[(conv_ecount << 1) * conv_scount];
    strbump_t conv_ectx = { .ctx = ectx_buf, .off = 0, .size = 19 };
    _dist_buf(ebuf_slices, fail_ebuf, conv_ecount << 1, conv_scount, sizeof(str_res));
    // Rand-case Buffer Setup:
    rctx_res_t conv_res_rctx = {0}; rctx_input_t conv_in_rctx = {0};
    rand_container conv_rcon = {
        .in_cont_type = CTX,
        .in_cont.rctx = &conv_in_rctx,
        .res_cont = &conv_res_rctx
    };
    // Randomization Configuration
    xoshiro256_state conv_rstate = {0}; u64 side_mix = 0;
    __GET_ENTROPY_FAST(conv_rstate.s, sizeof(u64) << 2);
    __GET_ENTROPY_FAST(side_mix, sizeof(u64));
    seed_xoshiro256(&conv_rstate, side_mix);
    str_rand_mod conv_rconfig_nob = {0}, // Non-base parameter / Base-10
    conv_rconfig_b = {0}; // Base-prefix / Base-parameter / Base-N
    strgen_init_sesh(&conv_rconfig_nob, false, &conv_rstate);
    strgen_init_sesh(&conv_rconfig_b, true, &conv_rstate);


    //* ------------------------------------ SUITE SETUP ------------------------------------ *//
    // to_str() - Non-base-parameter, No length param
    suite to_str_suite = {0};
    create_str_suite(&to_str_suite, "to_str - BigInt Conversion", 
        conv_scount, rcount, ecases_nob, INVERSE, ebuf_slices[0], 
        "../logs/bi_logs/bigint_to_str.txt", conv_ectx, &conv_rcon,
        &conv_rconfig_nob, &conv_rstate
    ); to_str_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&to_str_suite,
        &_bitos_conv_ingen, &exec_bitos_to_str,
        &inv_bitos_conv_nob, &stat_bitos_conv_nob, 
        &cmp_inv_bitos_conv, &fmt_in_to_str, &fmt_recon_bitos,
        &_bitos_conv_inlink, &_bitos_conv_insize,
        &_bitos_recon_linker, &_bitos_recon_size,
        &_bitos_outlink, &_bitos_aux2link
    );
    // to_strb() - Base-parameter, No length param
    suite to_strb_suite = {0};
    create_str_suite(&to_strb_suite, "to_strb - BigInt Conversion", 
        conv_scount, rcount, ecases_b, INVERSE, ebuf_slices[1], 
        "../logs/bi_logs/bigint_to_str.txt", conv_ectx, &conv_rcon,
        &conv_rconfig_b, &conv_rstate
    ); to_strb_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&to_strb_suite,
        &_bitos_conv_ingen, &exec_bitos_to_strb,
        &inv_bitos_conv_b, &stat_bitos_conv_b, 
        &cmp_inv_bitos_conv, &fmt_in_to_strb, &fmt_recon_bitos,
        &_bitos_conv_inlink, &_bitos_conv_insize,
        &_bitos_recon_linker, &_bitos_recon_size,
        &_bitos_outlink, &_bitos_aux2link
    );
    // to_strn() - Non-base-parameter, length param
    suite to_strn_suite = {0};
    create_str_suite(&to_strn_suite, "to_strn - BigInt Conversion", 
        conv_scount, rcount, ecases_nob, INVERSE, ebuf_slices[2], 
        "../logs/bi_logs/bigint_to_str.txt", conv_ectx, &conv_rcon,
        &conv_rconfig_nob, &conv_rstate
    ); to_strn_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&to_strn_suite,
        &_bitos_conv_ingen, &exec_bitos_to_strn,
        &inv_bitos_conv_nob, &stat_bitos_conv_nob, 
        &cmp_inv_bitos_conv, &fmt_in_to_strn, &fmt_recon_bitos,
        &_bitos_conv_inlink, &_bitos_conv_insize,
        &_bitos_recon_linker, &_bitos_recon_size,
        &_bitos_outlink, &_bitos_aux2link
    );
    // to_strnb() - Base-parameter, length param
    suite to_strnb_suite = {0};
    create_str_suite(&to_strnb_suite, "to_strnb - BigInt Conversion",
        conv_scount, rcount, ecases_b, INVERSE, ebuf_slices[3],
        "../logs/bi_logs/bigint_to_str.txt", conv_ectx, &conv_rcon,
        &conv_rconfig_b, &conv_rstate
    ); to_strnb_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&to_strnb_suite,
        &_bitos_conv_ingen, &exec_bitos_to_strnb,
        &inv_bitos_conv_b, &stat_bitos_conv_b, 
        &cmp_inv_bitos_conv, &fmt_in_to_strnb, &fmt_recon_bitos,
        &_bitos_conv_inlink, &_bitos_conv_insize,
        &_bitos_recon_linker, &_bitos_recon_size,
        &_bitos_outlink, &_bitos_aux2link
    );


    //* ---------------------------------- SESSION STARTUP ---------------------------------- *//
    _libdnml_str_suite conv_suite_arr[conv_scount];
    conv_suite_arr[0] = to_str_suite;  conv_suite_arr[1] = to_strb_suite;
    conv_suite_arr[2] = to_strn_suite; conv_suite_arr[3] = to_strnb_suite;
    _libdnml_session bi_conv_sesh = {0}; create_str_session(
        &bi_conv_sesh, "I/O - BigInt --> String Conversion",
        100, conv_scount, conv_suite_arr, conv_omode
    ); start_str_session(&bi_conv_sesh);
    return 0;
}