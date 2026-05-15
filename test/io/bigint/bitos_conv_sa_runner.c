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
limb_t small_mulval[5] = {
    10, // For Case 4
    UINT16_MAX, // For case 5
    UINT64_MAX,  // For case 6
};

scase ecases_nob[30] = {
    /* ---------------------------------------------------------------------------------------------------------------- */
    /* Case Number  | Input                                     | Capacity  | Expected Ouput                            */
    /* ------------------------------------------------ TRIVIAL CASES ------------------------------------------------- */
    { /* 1          | 0 (n = 0, sign = 1)                       | 0         | STR_INVALID_CAP                           */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 0,
            .x = { .limbs = &zero, .n = 0, .cap = 1, .sign = 1 }
        }, INVAL_STR(STR_INVALID_CAP)
    }, { /* 2       | 1 (n = 1, sign = -1)                      | 1         | STR_INVALID_CAP                           */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 0,
            .x = { .limbs = &one, .n = 1, .cap = 1, .sign = -1 }
        }, INVAL_STR(STR_INVALID_CAP)
    }, { /* 3       | 1 (n = 1, sign = 1)                       | 1         | STR_SUCCESS                           */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 1,
            .x = { .limbs = &one, .n = 1, .cap = 1, .sign = 1 }
        },
        .exp = { 
            .type = STRING, .status = STR_SUCCESS,
            .cap = 1, .data.len = 1, .pstr = "1"
        }
    }, { /* 4       | -10 (n = 1, sign = -1)                    | 3         | STR_SUCCESS                           */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 3,
            .x = { .limbs = &small_mulval[0], .n = 1, .cap = 1, .sign = -1 }
        },
        .exp = { 
            .type = STRING, .status = STR_SUCCESS,
            .cap = 3, .data.len = 3, .pstr = "-10"
        }
    }, { /* 5      | 65535 (n = 1, sign = 1)                    | 8         | STR_SUCCESS                           */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 8,
            .x = { .limbs = &small_mulval[1], .n = 1, .cap = 1, .sign = 1 }
        },
        .exp = { 
            .type = STRING, .status = STR_SUCCESS,
            .cap = 8, .data.len = 5, .pstr = "65535"
        }
    }, { /* 6      | 2^64 - 1 (n = 1, sign = 1)                 | 24        | STR_SUCCESS                           */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 24,
            .x = { .limbs = &small_mulval[2], .n = 1, .cap = 1, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS,
            .cap = 24, .data.len = 20, .pstr = "18446744073709551615"
        }
    }, { /* 7      | -(2^64 - 1) (n = 1, sign = -1)             | 20        | STR_INVALID_CAP                       */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 20,
            .x = { .limbs = &small_mulval[2], .n = 1, .cap = 1, .sign = 1 }
        }, INVAL_STR(STR_INVALID_CAP)
    },
    /* ------------------------------------------------- EDGE CASES --------------------------------------------------- */
    /* ---------------------------------------------------------------------------------------------------------------- */
};
scase ecases_b[30] = {};


// Main Code
int main(int argc, char **argv) {
    //* ---------------------------------- PRE-TEST SETUP ---------------------------------- *//
    // Parse terminal args + Setup env constants
    u16 rcount = (argc >= 1) ? (u16)(_stou64(argv[1], strlen(argv[1]))) : 100;
    _dnml_output_mode conv_omode; if (argc >= 2) {
        u8 sesh_count = _stou64(argv[2], strlen(argv[2]));
        conv_omode = (sesh_count <= 3) ? DNML_VOUT : DNML_COUT;
    } else conv_omode = DNML_VOUT;
    u8 conv_ecount = 30, conv_scount = 4;

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
    bi_rand_mod conv_rconfig = {0}; // Non-base-prefix
    bigen_init_sesh(&conv_rconfig, &conv_rstate);


    //* ------------------------------------ SUITE SETUP ------------------------------------ *//
    // to_str() - Non-base-parameter, No length param
    suite to_str_suite = {0};
    create_str_suite(&to_str_suite, "to_str - BigInt Conversion", 
        conv_scount, rcount, ecases_nob, INVERSE, ebuf_slices[0], 
        "../logs/bi_logs/bigint_to_str_sa.txt", conv_ectx, &conv_rcon,
        &conv_rconfig, &conv_rstate
    ); to_str_suite.cap_mode = RANDOMIZED;
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
        "../logs/bi_logs/bigint_to_str_sa.txt", conv_ectx, &conv_rcon,
        &conv_rconfig, &conv_rstate
    ); to_strb_suite.cap_mode = RANDOMIZED;
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
        "../logs/bi_logs/bigint_to_str_sa.txt", conv_ectx, &conv_rcon,
        &conv_rconfig, &conv_rstate
    ); to_strn_suite.cap_mode = RANDOMIZED;
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
        "../logs/bi_logs/bigint_to_str_sa.txt", conv_ectx, &conv_rcon,
        &conv_rconfig, &conv_rstate
    ); to_strnb_suite.cap_mode = RANDOMIZED;
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
        &bi_conv_sesh, "I/O - BigInt --> String Conversion (Size-aware)",
        100, conv_scount, conv_suite_arr, conv_omode
    ); start_str_session(&bi_conv_sesh);
    return 0;
}