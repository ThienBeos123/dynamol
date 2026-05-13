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


scase ecases_nob[35] = {};
scase ecases_b[35] = {};


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
    limb_t ectx_buf[19]; // Edge-case Memory Usage: 152 bytes
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
    // tto_str() - Non-base-parameter, No length param
    suite tto_str_suite = {0};
    create_str_suite(&tto_str_suite, "tto_str - BigInt Conversion", 
        conv_scount, rcount, ecases_nob, INVERSE, ebuf_slices[0], 
        "../logs/bi_logs/bigint_tto_str.txt", conv_ectx, &conv_rcon,
        &conv_rconfig_nob, &conv_rstate
    ); tto_str_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&tto_str_suite,
        &_bitos_conv_ingen, &exec_bitos_tto_str,
        &inv_bitos_conv_nob, &stat_bitos_tconv_nob, 
        &cmp_inv_bitos_conv, &fmt_in_to_str, &fmt_recon_bitos,
        &_bitos_conv_inlink, &_bitos_conv_insize,
        &_bitos_recon_linker, &_bitos_recon_size,
        &_bitos_outlink, &_bitos_aux2link
    );
    // tto_strb() - Base-parameter, No length param
    suite tto_strb_suite = {0};
    create_str_suite(&tto_strb_suite, "tto_strb - BigInt Conversion", 
        conv_scount, rcount, ecases_b, INVERSE, ebuf_slices[0], 
        "../logs/bi_logs/bigint_tto_str.txt", conv_ectx, &conv_rcon,
        &conv_rconfig_b, &conv_rstate
    ); tto_strb_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&tto_strb_suite,
        &_bitos_conv_ingen, &exec_bitos_tto_strb,
        &inv_bitos_conv_b, &stat_bitos_tconv_b, 
        &cmp_inv_bitos_conv, &fmt_in_to_strb, &fmt_recon_bitos,
        &_bitos_conv_inlink, &_bitos_conv_insize,
        &_bitos_recon_linker, &_bitos_recon_size,
        &_bitos_outlink, &_bitos_aux2link
    );
    // tto_strn() - Non-base-parameter, length param
    suite tto_strn_suite = {0};
    create_str_suite(&tto_strn_suite, "tto_strn - BigInt Conversion", 
        conv_scount, rcount, ecases_nob, INVERSE, ebuf_slices[0], 
        "../logs/bi_logs/bigint_tto_str.txt", conv_ectx, &conv_rcon,
        &conv_rconfig_nob, &conv_rstate
    ); tto_strn_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&tto_strn_suite,
        &_bitos_conv_ingen, &exec_bitos_tto_strn,
        &inv_bitos_conv_nob, &stat_bitos_tconv_nob,
        &cmp_inv_bitos_conv, &fmt_in_to_strn, &fmt_recon_bitos,
        &_bitos_conv_inlink, &_bitos_conv_insize,
        &_bitos_recon_linker, &_bitos_recon_size,
        &_bitos_outlink, &_bitos_aux2link
    );
    // tto_strnb() - Base-parameter, length param
    suite tto_strnb_suite = {0};
    create_str_suite(&tto_strnb_suite, "tto_strnb - BigInt Conversion",
        conv_scount, rcount, ecases_b, INVERSE, ebuf_slices[0],
        "../logs/bi_logs/bigint_tto_str.txt", conv_ectx, &conv_rcon,
        &conv_rconfig_b, &conv_rstate
    ); tto_strnb_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&tto_strnb_suite,
        &_bitos_conv_ingen, &exec_bitos_tto_strnb,
        &inv_bitos_conv_b, &stat_bitos_tconv_b,
        &cmp_inv_bitos_conv, &fmt_in_to_strnb, &fmt_recon_bitos,
        &_bitos_conv_inlink, &_bitos_conv_insize,
        &_bitos_recon_linker, &_bitos_recon_size,
        &_bitos_outlink, &_bitos_aux2link
    );


    //* ---------------------------------- SESSION STARTUP ---------------------------------- *//
    _libdnml_str_suite tconv_suite_arr[conv_scount];
    tconv_suite_arr[0] = tto_str_suite;  tconv_suite_arr[1] = tto_strb_suite;
    tconv_suite_arr[2] = tto_strn_suite; tconv_suite_arr[3] = tto_strnb_suite;
    _libdnml_session bi_conv_sesh = {0}; create_str_session(
        &bi_conv_sesh, "I/O - BigInt --> String Conversion",
        100, conv_scount, tconv_suite_arr, conv_omode
    ); start_str_session(&bi_conv_sesh);
    return 0;
}