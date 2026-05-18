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
limb_t small_mulval[4] = {
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
limb_t case_17[2] = { 0, UINT32_MAX + 1 }, case_18[4] = { 0, 0, 0, 1 };
limb_t case_19[5] = { UINT64_MAX, UINT64_MAX, 0, 0, 1 };
limb_t case_20[5] = { 0, 1, 2, 3, 4 };
limb_t case_21[4] = { UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX };
limb_t case_22[2] = { UINT64_MAX, UINT64_C(0x7FFFFFFFFFFFFFFF) };
limb_t case_23[6] = {
    UINT64_C(3846025213140126128), UINT64_C(1334182037947790491),
    UINT64_C(16447488665830626805), UINT64_C(14852190203560883297),
    UINT64_C(13521263668617507741), UINT64_C(34838838833726957)
};
limb_t case_final[48] = {
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
};

scase ecases[25] = { // 2270 bytes of memory ---> Rounded up to 2304
    /* ------------------------------------------------------------------------------------------------------------------------ */
    /* Case Number  | Input                             | Base          | Expected Ouput                                        */
    /* -------------------------------------------------- TRIVIAL CASES ------------------------------------------------------- */
    { /* 1          | 0 (n = 0)                         | 2             | "0b0"                                                 */
        .in = &(bitos_print_in){ 
            .base = 2, .uppercase = false,
            .x = { .limbs = &zero, .n = 0, .cap = 1, .sign = 1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .data.len = 3, .cap = 3, .pstr = "0b0" }
    }, { /* 2       | 1 (n = 1)                         | 8             | "0o1"                                                 */
        .in = &(bitos_print_in){ 
            .base = 8, .uppercase = false,
            .x = { .limbs = &one, .n = 1, .cap = 1, .sign = 1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .data.len = 3, .cap = 3, .pstr = "0o1" }
    }, { /* 3       | -1 (n = 1, sign = -1)             | 16            | "-0x1"                                                */
        .in = &(bitos_print_in){
            .base = 16, .uppercase = false,
            .x = { .limbs = &one, .n = 1, .cap = 1, .sign = -1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .data.len = 4, .cap = 4, .pstr = "-0x1" }
    }, { /* 4       | 255 (n = 1)                       | 10            | "255"                                                 */
        .in = &(bitos_print_in){ 
            .base = 10, .uppercase = false,
            .x = { .limbs = &small_mulval[0], .n = 1, .cap = 1, .sign = 1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .data.len = 3, .cap = 3, .pstr = "255" }
    }, { /* 5       | -999 (n = 1, sign = -1)           | 10            | "-999"                                                */
        .in = &(bitos_print_in){ 
            .base = 10, .uppercase = false,
            .x = { .limbs = &small_mulval[1], .n = 1, .cap = 1, .sign = -1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .data.len = 4, .cap = 4, .pstr = "-999" }
    }, { /* 6       | 2^64 - 1 (n = 1)                  | 16            | "0XFFFFFFFFFFFFFFFF"                                  */
        .in = &(bitos_print_in){
            .base = 16, .uppercase = true,
            .x = { .limbs = &small_mulval[2], .n = 1, .cap = 1, .sign = 1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .data.len = 18, .cap = 18, .pstr = "0XFFFFFFFFFFFFFFFF" }
    }, { /* 7       | -(2^64 - 1) (n = 1, sign = -1)    | 16            | "-0xffffffffffffffff"                                 */
        .in = &(bitos_print_in){
            .base = 16, .uppercase = false,
            .x = { .limbs = &small_mulval[2], .n = 1, .cap = 1, .sign = -1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .data.len = 19, .cap = 19, .pstr = "-0xffffffffffffffff" }
    },
    /* --------------------------------------------------- EDGE CASES --------------------------------------------------------- */
    { /* 8          | 2^64 (n = 2)                      | 2             | "0b100000000000...000000000000" (truncated)           */
        .in = &(bitos_print_in){
            .base = 2, .uppercase = false,
            .x = { .limbs = case_8, .cap = 2, .n = 2, .sign = 1 }
        },
        .exp = { 
            .type = STRING, .status = STR_SUCCESS, .cap = 67, .data.len = 67, 
            .pstr = "0b10000000000000000000000000000000000000000000000000000000000000000" 
        }
    }, { /* 9       | 2^64 + 1 (n = 2)                  | 8             | "0O2000000000000000000001"                            */
        .in = &(bitos_print_in){
            .base = 8, .uppercase = true,
            .x = { .limbs = case_9, .cap = 2, .n = 2, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 24, .data.len = 24,
            .pstr = "0O2000000000000000000001"
        }
    }, { /* 10      | 2^128 - 1 (n = 2)                 | 7             | "0{7}311551216212...356026315303" (truncated)         */
            .in = &(bitos_print_in){
            .base = 7, .uppercase = false,
            .x = { .limbs = case_10, .cap = 2, .n = 2, .sign = 1 }
        },
        .exp = { 
            .type = STRING, .status = STR_SUCCESS, .cap = 50, .data.len = 50, 
            .pstr = "0{7}3115512162124626343001006330151620356026315303" 
        }
    }, { /* 11      | -(2^128 - 1) (n = 2)              | 16            | "-0XFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"                 */
        .in = &(bitos_print_in){
            .base = 16, .uppercase = true,
            .x = { .limbs = case_10, .cap = 2, .n = 2, .sign = -1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 35, .data.len = 35,
            .pstr = "-0XFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
        }
    }, { /* 12      | idk Random ig (n = 3)             | 3             | "0{3}111210010100...202201200001" (truncated)         */
        .in = &(bitos_print_in){
            .base = 3, .uppercase = true,
            .x = { .limbs = case_12, .cap = 3, .n = 3, .sign = 1 }
        },
        .exp = { 
            .type = STRING, .status = STR_SUCCESS, .cap = 86, .data.len = 86,
            .pstr = "0{3}"
                    "11121001010011210211100011210110"
                    "20000002010100200221200112200222"
                    "020221202201200001" 
        }
    }, { /* 13      | 1000000000 (n = 1) - Power of 10  | 10            | "1000000000"                                          */
        .in = &(bitos_print_in){
            .base = 10, .uppercase = false,
            .x = { .limbs = &small_mulval[3], .cap = 1, .n = 1, .sign = 1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .cap = 10, .data.len = 10,  .pstr = "1000000000" }
    }, { /* 14      | -1000000000 (n = 1, sign = -1)    | 10            | "-1000000000"                                         */
        .in = &(bitos_print_in){
            .base = 10, .uppercase = true,
            .x = { .limbs = &small_mulval[3], .cap = 1, .n = 1, .sign = -1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .cap = 11, .data.len = 11,  .pstr = "-1000000000" }
    }, { /* 15      | Alt Limbs (n = 2)                 | 16            | "0xaaaaaaaaaaaaaaaa5555555555555555"                  */
        .in = &(bitos_print_in){
            .base = 16, .uppercase = false,
            .x = { .limbs = case_15, .cap = 2, .n = 2, .sign = 1 }
        },
        .exp = { 
            .type = STRING, .status = STR_SUCCESS, .cap = 34, .data.len = 34,
            .pstr = "0xaaaaaaaaaaaaaaaa5555555555555555"
        }
    }, { /* 16      | MSB Limbs (n = 2)                 | 2             | "0B100000000000...000000000000" (truncated)           */
        .in = &(bitos_print_in){
            .base = 2, .uppercase = true,
            .x = { .limbs = case_16, .cap = 2, .n = 2, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 130, .data.len = 130,
            .pstr = "0B"
                    "10000000000000000000000000000000"
                    "00000000000000000000000000000000"
                    "10000000000000000000000000000000"
                    "00000000000000000000000000000000"
        }
    }, { /* 17      | 2^96 (n = 2)                      | 8             | "0o100000000000000000000000000000000"                 */
        .in = &(bitos_print_in){
            .base = 8, .uppercase = false,
            .x = { .limbs = case_17, .cap = 2, .n = 2, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 35, .data.len = 35,
            .pstr = "0o100000000000000000000000000000000"
        }
    }, { /* 18      | 2^192 (n = 4)                     | 32            | "-0{32}400000000000000000000000000000000000000"       */
        .in = &(bitos_print_in){
            .base = 32, .uppercase = true,
            .x = { .limbs = case_18, .cap = 4, .n = 4, .sign = -1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 45, .data.len = 45,
            .pstr = "-0{32}400000000000000000000000000000000000000"
        }
    }, { /* 19      | idk (n = 5) - LARGELY SPARSE      | 18            | "0{18}31G025HE8916...8224E90HA311" (truncated)        */
        .in = &(bitos_print_in){
            .base = 18, .uppercase = true,
            .x = { .limbs = case_19, .cap = 5, .n = 5, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 67, .data.len = 67,
            .pstr = "0{18}31G025HE891652FC25EED6DG159AHB8HD9DC46856F404H5GAG8224E90HA311"
        }
    }, { /* 20      | [0, 1, 2, 3, 4] (n = 5, sign = -1)| 16            | "-0x400000000000...000000000000" (truncated)          */
        .in = &(bitos_print_in){
            .base = 16, .uppercase = false,
            .x = { .limbs = case_20, .cap = 5, .n = 5, .sign = -1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 68, .data.len = 68,
            .pstr = "-0x"
                    "4000000000000000300000000000000"
                    "02000000000000000100000000000000"
                    "00"
        }
    }, { /* 21      | 2^256 - 1 (n = 5, sign = -1)      | 16            | "-0xffffffffffff...ffffffffffff" (truncated)          */
        .in = &(bitos_print_in){
            .base = 16, .uppercase = false,
            .x = { .limbs = case_21, .cap = 4, .n = 4, .sign = -1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 67, .data.len = 67,
            .pstr = "-0x"
                    "ffffffffffffffffffffffffffffffff"
                    "ffffffffffffffffffffffffffffffff"
        }
    }, { /* 22      | 2^127 - 1 (n = 2) - MERSENNE PRIME| 2             | "0B111111111111...111111111111" (truncated)           */
        .in = &(bitos_print_in){
            .base = 2, .uppercase = true,
            .x = { .limbs = case_22, .cap = 2, .n = 2, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 129, .data.len = 129,
            .pstr = "0B"
                    "11111111111111111111111111111111"
                    "11111111111111111111111111111111"
                    "11111111111111111111111111111111"
                    "1111111111111111111111111111111"
        }
    }, { /* 23      | Absolutely random (n = 6)         | 48            | "0{48}1xbcg0m8hwlh...j6urtfakgmjw" (truncated)        */
        .in = &(bitos_print_in){
            .base = 0, .uppercase = false,
            .x = { .limbs = case_23, .cap = 6, .n = 6, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 73, .data.len = 73,
            .pstr = "0{48}"
                    "1XBCG0M8HWlH2Z41QAPQFHAIcILZBLgR"
                    "M7EA6c7Sab58AMlgSUj37diZj6URTfak"
                    "GMJW" //! CASE-SENSITIVE --> UPPERCASE WON'T AFFECT ITS DIGITS
        }
    }, { /* 24      | 2^3072 - 1 (n = 48)               | 16            | "0XFFFFFFFFFFFF...FFFFFFFFFFFF" (truncated)           */
        .in = &(bitos_print_in){
            .base = 16, .uppercase = true,
            .x = { .limbs = case_final, .cap = 48, .n = 48, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 770, .data.len = 770,
            .pstr = "0X"
                    "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
                    "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
                    "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
                    "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
                    "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
                    "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
                    "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
                    "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
                    "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
                    "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
                    "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
                    "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
        }
    }, { /* 25      | -(2^3072 - 1) (n = 48, sign = -1) | 64            | "-0,////////////...////////////" (truncated)          */
        .in = &(bitos_print_in){
            .base = 64, .uppercase = true,
            .x = { .limbs = case_final, .cap = 48, .n = 48, .sign = -1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 515, .data.len = 515,
            .pstr = "-0,"
                    "////////////////////////////////////////////////////////////////"
                    "////////////////////////////////////////////////////////////////"
                    "////////////////////////////////////////////////////////////////"
                    "////////////////////////////////////////////////////////////////"
                    "////////////////////////////////////////////////////////////////"
                    "////////////////////////////////////////////////////////////////"
                    "////////////////////////////////////////////////////////////////"
                    "////////////////////////////////////////////////////////////////"
        }
    },
    /* -------------------------------------------------------------------------------------------------------------------- */
};


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
    char ectx_buf[2304]; // Edge-case Memory Usage: 2304 bytes
    str_res *ebuf_slices[print_scount], fail_ebuf[(print_ecount << 1) * print_scount];
    strbump_t print_ectx = { .ctx = ectx_buf, .off = 0, .size = 2304 };
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
    __GET_ENTROPY_FAST(&side_mix, sizeof(u64));
    seed_xoshiro256(&print_rstate, side_mix);
    bi_rand_mod print_rconfig = {0}; // Base-prefix
    bigen_init_sesh(&print_rconfig, &print_rstate);


    //* ------------------------------------ SUITE SETUP ------------------------------------ *//
    // fputf() - Stream-based Printing
    suite fputf_suite = {0};
    create_str_suite(&fputf_suite, "fputf - BigInt Printing", 
        print_scount, rcount, ecases, INVERSE, ebuf_slices[0], 
        "../logs/bi_logs/bigInt_fputf.txt", &print_ectx, &print_rcon,
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
        "../logs/bi_logs/bigInt_fputf.txt", &print_ectx, &print_rcon,
        &print_rconfig, &print_rstate
    ); sfputf_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&sfputf_suite,
        &_bitos_print_ingen, &exec_bitos_sfputf,
        &inv_bitos_fput_b, &stat_bitos_print,
        &cmp_inv_bitos_put, &fmt_in_fputf, &fmt_recon_bitos,
        &_bitos_print_inlink, &_bitos_print_insize,
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