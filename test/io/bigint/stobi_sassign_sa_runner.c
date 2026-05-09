// STDLIB & /include Utilities
#include <include.h>
#include <system/sys.h>
// Main Components
#include "../../../test_ui/_strui.h"
#include "../case_gen/case_gen.h"
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
*                          for its output. Setting it as INPUT_BYTE_CAP is only acceptable during
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

/* Note - UNIT-SPECIFIC:
*   +) ".base = 0 in ecases_bprefix[27], WHY?"
*      A noteable oddity is the decision ot set the .base metadata in _libdnml_scases
*      input of arrays of edge cases in some STOBI Runners, in which is reserved for 
*      function that lacks the parameter/argument for user-input base, being set to 0. 
*      This is purely a safety mechanism, in which if anybody were to mistakenly
*      put a base-parameterized variant in the place of non-base-parameterized
*      function execution wrappers, or utilizes non-base-parameterized edge cases 
*      for base-parameterized functions in production (SPECIFICALLY WITH THE SETTING
*      _DNML_DEBUG_MODE ON), it is caught at compile-time using our test_assert() 
*      function macros, and the programs returns an error message from assertions
*      (EITHER LIBC ASSERTIONS OR LIB-DNML CUSTOM ASSERTIONS)
*
*   +) This runner has all edge test cases with the bigInt output buffer at the maximum
*      capacity (512 bytes / 64 limbs), for this runner is utilized for parsing and
*      numerical-handling correctness. Size-handling API testing is tested in the file
*      "stobi_sassign_sa_runner.c"
*/


// Edge-case VALUE STORAGE - BASE-PREFIX
limb_t one = 1;
limb_t small_mulval_bp[35] = {
    5, // Base-64: 5
    UINT16_C(3965), // Base-64: zz
    UINT64_C(183293895038276755), // Base-64: ABCDEFGHIJ
    UINT64_C(10925117571), // Base-64: ABC123
    UINT64_C(290481608618505), // Base-64: 0123456789
};
limb_t bp_case_6[3] = {
    // Base-64: ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef
    UINT64_C(17879723973686229545),
    UINT64_C(1538390599243603321),
    UINT64_C(2932702320612428085)
};
limb_t bp_case_9[2] = {
    // Base-64: 0123456789ABCDEFGHIJk
    UINT64_C(11730809282449712366),
    UINT64_C(1161926434474020),
};
limb_t bp_case_10[4] = {
    // Base-64: 0123456789ABCDEFGHIJKLMNOPQRSTUVWxyz
    UINT64_C(10852392897987460925),
    UINT64_C(10322554443867199329),
    UINT64_C(1175869551687708883),
    UINT16_C(4227),
};
limb_t bp_case_11[5] = {
    // For ecase_bprefix's CASE 11 (Range: [15] --> [19])
    // Base-64: KT7wH/q0lzdRBmxfb4tCtQbDMLozsmVMteeshvxnbJq
    UINT64_C(8802525983350871284),
    UINT64_C(11904532734475509595),
    UINT64_C(7835203037231002486),
    UINT64_C(5136330223486435175),
    1,
};
limb_t bp_case_16[2] = {
    // Base-64: 0123456789ABCDEFGHIJ
    UINT64_C(10559587436499899539),
    UINT64_C(18155100538656),
};
limb_t bp_case_18[6] = {
    UINT64_C(11712218459498128010),
    UINT64_C(9955385690573408808),
    UINT64_C(2928054614874532002),
    UINT64_C(11712218459498128010),
    UINT64_C(9955385690573408808),
    UINT64_C(2928054614874532002),
};
limb_t bp_case_20[12] = {
    UINT64_C(6423701355093973285), /* 0 */ UINT64_C(2707324343907067282), /* 1 */
    UINT64_C(10545501312955814489), /* 2 */ UINT64_C(6423701355093973285), /* 3 */
    UINT64_C(2707324343907067282), /* 4 */ UINT64_C(10545501312955814489), /* 5 */
    UINT64_C(6423701355093973285), /* 6 */ UINT64_C(2707324343907067282), /* 7 */
    UINT64_C(10545501312955814489), /* 8 */ UINT64_C(6423701355093973285), /* 9 */
    UINT64_C(2707324343907067282), /* 10 */ UINT64_C(10545501312955814489) /* 11 */
};
// Edge-case VALUE STORAGE - BASE-PARAMETER

// Edge-case STORAGE
scase ecases_bprefix[35] = {
    /* ---------------------------------------------------------------------------------------------------- */
    /* Case Number  |   Input                                       |   Expected Ouput                      */
    /* ---------------------------------------- EASY SUCCESS CASE ----------------------------------------- */
    { /* 1.         | "0,5"                                     ---->   STR_SUCCESS (5)                     */
        .in = &(stobi_assign_in){ .str = "0,5", .len = 4, .base = 0, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[0], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 2.      | "0,zz"                                    ---->   STR_SUCCESS (3965)                  */ 
        .in = &(stobi_assign_in){ .str = "0,zz", .len = 5, .base = 0, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[1], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 3.      | "0,ABCDEFGHIJ"                            ---->   STR_SUCCESS (183293895038276755)    */ 
        .in = &(stobi_assign_in){ .str = "0,ABCDEFGHIJ", .len = 13, .base = 0, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[2], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 4.      | "-0,ABC123"                               ---->   STR_SUCCESS (10925117571)           */ 
        .in = &(stobi_assign_in){ .str = "-0,ABC123", .len = 10, .base = 0, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[3], .n = 1, .cap = 1, .sign = -1 }
        }
    }, { /* 5.      | "0,0123456789"                            ---->   STR_SUCCESS (290481608618505)       */ 
        .in = &(stobi_assign_in){ .str = "0,0123456789", .len = 13, .base = 0, .bi_size = 4 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[4], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 6.      | "0,ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef"      ---->   STR_SUCCESS (idk too long)          */ 
        .in = &(stobi_assign_in){ 
            .str = "0,ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef",
            .len = 35, .base = 0, .bi_size = 8
        },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = bp_case_6, .n = 3, .cap = 1, .sign = 1 }
        }
    }, { /* 7.      | "0,0000000001"                            ---->   STR_SUCCESS (1)                     */ 
        .in = &(stobi_assign_in){ .str = "0,0000000001", .len = 13, .base = 0, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &one, .n = 1, .cap = 1, .sign = 1 }
        }
    },
    /* ----------------------------------------- ON-THE-EDGE CASE ----------------------------------------- */
    { /* 8.         | "0,0123456789"                                ---->   STR_SUCCESS (290481608618505)       */
        .in = &(stobi_assign_in){ .str = "0,0123456789", .len = 13, .base = 0, .bi_size = 1 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[4], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 9.      | "0,0123456789ABCDEFGHIJk"                     ---->   STR_SUCCESS (too long)              */
        .in = &(stobi_assign_in){ .str = "0,0123456789ABCDEFGHIJk", .len = 24, .base = 0, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = bp_case_9, .n = 2, .cap = 2, .sign = 1 }
        }
    }, { /* 10.     | "0,0123456789ABCDEFGHIJKLMNOPQRSTUVWxyz"      ---->   STR_SUCCESS (too long)              */
        .in = &(stobi_assign_in){ 
            .str = "0,0123456789ABCDEFGHIJKLMNOPQRSTUVWxyz", 
            .len = 39, .base = 0, .bi_size = 4 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = bp_case_10, .n = 4, .cap = 4, .sign = 1 }
        }
    }, { /* 11.     | "0,KT7wH/q0lzdR...MteeshvxnbJq" (truncated)   ---->   STR_SUCCESS (too long)              */
        .in = &(stobi_assign_in){ 
            // This string is randomly-generated from "/script/rand_base64.py"
            .str = "0,KT7wH/q0lzdRBmxfb4tCtQbDMLozsmVMteeshvxnbJq",
            .len = 46, .base = 0, .bi_size = 5 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = bp_case_11, .n = 5, .cap = 5, .sign = 1 }
        }
    }, { /* 12.     | "0,///////////"                               ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){ .str = "0,///////////", .len = 14, .base = 0, .bi_size = 1 },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 13.     | "0,//////////////////////"                    ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){ .str = "0,//////////////////////", .len = 26, .base = 0, .bi_size = 2 },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 14.     | "0,0123456789ABCDEFGHIJKLMNOPQRSTUVWxyz"      ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "0,0123456789ABCDEFGHIJKLMNOPQRSTUVWxyz", 
            .len = 39, .base = 0, .bi_size = 3
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 15.     | "0,juiB7KHUrY.Y...ghF7bOpArFI" (truncated)    ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "0,juiB7KHUrY.Y5bJOXVNrNHsFe8t5m.rGEghF7bOpArFI",
            .len = 47, .base = 0, .bi_size = 4
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 16.     | "-0,0123456789ABCDEFGHIJ"                     ---->   STR_SUCCESS (too big)               */
        .in = &(stobi_assign_in){ 
            .str = "-0,0123456789ABCDEFGHIJ", 
            .len = 24, .base = 0, .bi_size = 2
        },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = bp_case_16, .n = 2, .cap = 2, .sign = -1 }
        }
    }, { /* 17.     | "-0,0123456789ABCDEFGHIJKL//"                 ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){ 
            .str = "-0,0123456789ABCDEFGHIJKL//",
            .len = 28, .base = 0, .bi_size = 2
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 18.     | "0,AAAAAAAAAAAA...AAAAAAAAAAAA" (truncated)   ---->   STR_SUCCESS (too large)             */
        .in = &(stobi_assign_in){ 
            .str = "0,AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
            .len = 67, .base = 0, .bi_size = 6
        },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = bp_case_18, .n = 6, .cap = 6, .sign = 1 }
        }
    }, { /* 19.     | "0,FFFFFFFFFFFF...FFFFFFFFFFFF" (truncated)   ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){ 
            .str = "0,FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF",
            .len = 67, .base = 0, .bi_size = 5
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 20.     | "0,abababababab...abababababab" (truncated)   ---->   STR_SUCCESS (too large)             */
        .in = &(stobi_assign_in){ 
            .str = "0, \
                abababababababababababababababab \
                abababababababababababababababab \
                abababababababababababababababab \
                abababababababababababababababab \
            ", .len = 131, .base = 0, .bi_size = 12
        },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = bp_case_20, .n = 12, .cap = 12, .sign = 1 }
        }
    }, { /* 21.     | "0,zzzzzzzzzzzz...zzzzzzzzzzzz" (truncated)   ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){ 
            .str = "0, \
                zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz \
                zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz \
                zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz \
                zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz \
            ", .len = 131, .base = 0, .bi_size = 11
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, 
    /* ---------------------------------------- CLEAR-FAILURE CASE ---------------------------------------- */
    /* ---------------------------------------------------------------------------------------------------- */
};
scase ecases_base[35] = {
    /* -------------------------------------------------------------------------------------------- */
    /* Case Number  |   Input                               |   Expected Ouput                      */
    /* -------------------------------------------------------------------------------------------- */
};


// Main Code
int main(int argc, char **argv) {
    //* ---------------------------------- PRE-TEST SETUP ---------------------------------- *//
    // Parse terminal args + Setup env constants
    u16 rcount = (argc >= 1) ? (u16)(_stou64(argv[1], strlen(argv[1]))) : 100;
    _dnml_output_mode sassign_omode; if (argc >= 2) {
        u8 sesh_count = _stou64(argv[2], strlen(argv[2]));
        sassign_omode = (sesh_count <= 3) ? DNML_VOUT : DNML_COUT;
    } else sassign_omode = DNML_VOUT;
    u8 sassign_ecount = 27, sassign_scount = 4;
    // Buffer Setup
    limb_t ectx_buf[19]; // Edge-case Memory Usage: 128 bytes
    rctx_t sassign_rctx = {0}; str_res *ebuf_slices[sassign_scount];
    str_res fail_ebuf[(sassign_ecount << 1) * sassign_scount];
    strbump_t sassign_ectx = { .ctx = ectx_buf, .off = 0, .size = 19 };
    _dist_buf(ebuf_slices, fail_ebuf, sassign_ecount << 1, sassign_scount, sizeof(str_res));
    input_container sassign_icon = { .cont_type = CTX, .cont.rctx = &sassign_rctx };


    //* ---------------------------------- SUITE SETUP ---------------------------------- *//
    // from_str() -- Base-prefix, No length param
    suite sget_str_suite = {0};
    create_str_suite(&sget_str_suite, "bigInt_tget_str - String Assignment", 
        sassign_scount, rcount, ecases_bprefix, INVERSE, ebuf_slices[0],
        "../logs/bigInt_get_str.txt", sassign_ectx, &sassign_icon
    ); sget_str_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&sget_str_suite,
        &_stobi_assign_ingen_nob, &exec_stobi_sget_str,
        &inv_stobi_assign_nob, &stat_stobi_get_str,
        &cmp_inv_stobi_assign, &fmt_in_get_str, &fmt_recon_stobi
    );
    // from_strn() -- Base-prefix, Length param
    suite sget_strn_suite = {0};
    create_str_suite(&sget_strn_suite, "bigInt_tget_strn - String Assignment",
        sassign_scount, rcount, ecases_bprefix, INVERSE, ebuf_slices[1],
        "../logs/bigInt_get_str.txt", sassign_ectx, &sassign_icon
    ); sget_strn_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&sget_strn_suite,
        &_stobi_assign_ingen_nob, &exec_stobi_sget_strn,
        &inv_stobi_assign_nob, &stat_stobi_get_strn,
        &cmp_inv_stobi_assign, &fmt_in_get_strn, &fmt_recon_stobi
    );
    // from_strb() -- Base-param, No length param
    suite sget_strb_suite = {0};
    create_str_suite(&sget_strb_suite, "bigInt_tget_strb - String Assignment",
        sassign_scount, rcount, ecases_bprefix, INVERSE, ebuf_slices[1],
        "../logs/bigInt_get_str.txt", sassign_ectx, &sassign_icon
    ); sget_strb_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&sget_strb_suite,
        &_stobi_assign_ingen_b, &exec_stobi_sget_strb,
        &inv_stobi_assign_b, &stat_stobi_get_strb,
        &cmp_inv_stobi_assignb, &fmt_in_get_strb, &fmt_recon_stobi
    );
    // from_strnb() -- Base-param, Length param
    suite sget_strnb_suite = {0};
    create_str_suite(&sget_strnb_suite, "bigInt_tget_strnb - String Assignment",
        sassign_scount, rcount, ecases_bprefix, INVERSE, ebuf_slices[1],
        "../logs/bigInt_get_str.txt", sassign_ectx, &sassign_icon
    ); sget_strnb_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&sget_strnb_suite,
        &_stobi_assign_ingen_b, &exec_stobi_sget_strnb,
        &inv_stobi_assign_b, &stat_stobi_get_strnb, 
        &cmp_inv_stobi_assignb, &fmt_in_get_strnb, &fmt_recon_stobi
    );


    //* ---------------------------------- SESSION STARTUP ---------------------------------- *//
    _libdnml_str_suite sassign_suite_arr[sassign_scount];
    sassign_suite_arr[0] = sget_str_suite;  sassign_suite_arr[1] = sget_strn_suite;
    sassign_suite_arr[2] = sget_strb_suite; sassign_suite_arr[3] = sget_strnb_suite;
    _libdnml_session bi_sassign_sesh = {0}; create_str_session(
        &bi_sassign_sesh, "I/O - String --> BigInt Assignment",
        100, sassign_scount, sassign_suite_arr, sassign_omode
    ); start_str_session(&bi_sassign_sesh);
    return 0;
}