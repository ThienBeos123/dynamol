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


scase ecases[35] = {};


// Main Code
int main(int argc, char **argv) {
    //* ---------------------------------- PRE-TEST SETUP ---------------------------------- *//
    // Parse terminal args + Setup env constants
    u16 rcount = (argc >= 1) ? (u16)(_stou64(argv[1], strlen(argv[1]))) : 100;
    _dnml_output_mode print_omode; if (argc >= 2) {
        u8 sesh_count = _stou64(argv[2], strlen(argv[2]));
        print_omode = (sesh_count <= 3) ? DNML_VOUT : DNML_COUT;
    } else print_omode = DNML_VOUT;
    u8 print_ecount = 35, print_scount = 2;

    // Edge-case Buffer Setup
    limb_t ectx_buf[19]; // Edge-case Memory Usage: 152 bytes
    str_res *ebuf_slices[print_scount], fail_ebuf[(print_ecount << 1) * print_scount];
    strbump_t print_ectx = { .ctx = ectx_buf, .off = 0, .size = 19 };
    _dist_buf(ebuf_slices, fail_ebuf, print_ecount << 1, print_scount, sizeof(str_res));
    // Rand-case Buffer Setup:
    rctx_res_t print_res_rctx = {0}; rctx_input_t print_in_rctx = {0};
    rand_container print_rcon = {
        .in_cont_type = CTX,
        .in_cont.rctx = &print_in_rctx,
        .res_cont = &print_res_rctx
    };
    // Randomization Configuration
    xoshiro256_state print_rstate = {0}; u64 side_mix = 0;
    __GET_ENTROPY_FAST(print_rstate.s, sizeof(u64) << 2);
    __GET_ENTROPY_FAST(side_mix, sizeof(u64));
    seed_xoshiro256(&print_rstate, side_mix);
    bi_rand_mod print_rconfig = {0}; // Non-base parameter / Base-10
    bigen_init_sesh(&print_rconfig, &print_rstate);


    //* ------------------------------------ SUITE SETUP ------------------------------------ *//
    // fputf() - Stream-based Printing
    suite fputf_suite = {0};
    create_str_suite(&fputf_suite, "fputf - BigInt Printing", 
        print_scount, rcount, ecases, INVERSE, ebuf_slices[0], 
        "../logs/bi_logs/bigInt_fputf.txt", print_ectx, &print_rcon,
        &print_rconfig, &print_rstate
    ); fputf_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&fputf_suite,
        &_bitos_print_ingen, &exec_bitos_fputf,
        &inv_bitos_fput_b, &stat_bitos_print, 
        &cmp_inv_bitos_put, &fmt_in_fputf, &fmt_recon_bitos,
        &_bitos_print_inlink, &_bitos_print_insize,
        &_bitos_recon_linker, &_bitos_recon_size,
        &_bitos_outlink, &_bitos_aux2link
    );
    // sfputf() - Buffered Printing
    suite sfputf_suite = {0};
    create_str_suite(&sfputf_suite, "sfputf - BigInt Printing", 
        print_scount, rcount, ecases, INVERSE, ebuf_slices[0], 
        "../logs/bi_logs/bigInt_fputf.txt", print_ectx, &print_rcon,
        &print_rconfig, &print_rstate
    ); sfputf_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&sfputf_suite,
        &_bitos_print_ingen, &exec_bitos_sfputf,
        &inv_bitos_fput_b, &stat_bitos_print,
        &cmp_inv_bitos_put, &fmt_in_fputf, &fmt_recon_bitos,
        &_bitos_conv_inlink, &_bitos_conv_insize,
        &_bitos_recon_linker, &_bitos_recon_size,
        &_bitos_outlink, &_bitos_aux2link
    );


    //* ---------------------------------- SESSION STARTUP ---------------------------------- *//
    _libdnml_str_suite print_suite_arr[print_scount];
    print_suite_arr[0] = fputf_suite;  print_suite_arr[1] = sfputf_suite;
    _libdnml_session bi_print_sesh = {0}; create_str_session(
        &bi_print_sesh, "I/O - BigInt --> String Printing (Formatted)",
        100, print_scount, print_suite_arr, print_omode
    ); start_str_session(&bi_print_sesh);
    return 0;
}