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
#include "../../../libdnml_base.h"
#include "../../../dynamol/big_numbers/bigInt_func.h"
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
*   +) ".base = 0 in ecases_bprefix, WHY?"
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
*   +) This runner is specifically designed to test size-awareness of functions
*      and their handling of it (Truncative API, Strict API, etc). Therefore,
*      the vast majority of edge-test cases is in base-64, with randomized input
*      that doesn't stress-test core correctness. For "core-correctness", please
*      refer to the testing unit: "stobi_fscan_runner.c", containing every API 
*      tested on their correctness"
*/

// THESE HAS TO BE INITIATED IN main()
FILE *tscan_in_nob, *tscan_in_b, *tscan_randin;

// Edge-case VALUE STORAGE - BASE-PREFIX
limb_t one = 1, zero = 0;
limb_t small_mulval_bp[4] = {
    5, // Base-64: 5
    UINT64_C(183293895038276755), // Base-64: ABCDEFGHIJ
    UINT64_C(44749486092289), // Base-64: ABCDEF01
    UINT64_C(39285586473),
};
limb_t case_3[2] = {
    // Base-64: 0123456789ABCDEFGHIJk
    UINT64_C(11730809282449712366),
    UINT64_C(1161926434474020),
};
limb_t case_5[5] = {
    // Base-64 String: PIQv29.Sop5wYEzB8mbYnoryDDJmX8Au4rpzKnN1UxovFGvP
    UINT64_C(1567670195793497689), UINT64_C(17331120993138965843),
    UINT64_C(2524657673719002324), UINT64_C(11501290961767034699),
    UINT32_C(1697036552),
};
limb_t case_13[2] = {
    // TRUNCATED!
    // Base-64: ncgwFYphl3NqaSA9Rck5f1R.IoJEQd
    UINT64_C(6503602364934317735),
    UINT64_C(14078051345472349880),
};
limb_t case_14[4] = {
    // TRUNCATED!
    // Base-64: AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
    UINT64_C(11712218459498128010),
    UINT64_C(9955385690573408808),
    UINT64_C(2928054614874532002),
    UINT64_C(11712218459498128010),
};
limb_t case_15[8] = {
    // TRUNCATED!
    // Base-64: 
    //  "/49ln9OVrlc/H2iWWmMsnzNqgapOqVdv9gZaGDcPTqtSi.tYGx0FQetwxgk/hXxR
    //   q2CBC/VIplL4td8unHK.TuzGKCnPiff/OjVKtpX2XIjP/ibpsdo3ewrMbxMaIKyk"
    UINT64_C(12490337422959791918),
    UINT64_C(6484842012884108195),
    UINT64_C(7122395390896407851),
    UINT64_C(10326842784168974975),
    UINT64_C(4962529427559104119),
    UINT64_C(14997843501902909173),
    UINT64_C(10230751924948180699),
    UINT64_C(15903316265229356906),
};
limb_t case_16[12] = {
    // Base-64: 0123456789ABCDEFGHIJ
    UINT64_C(7473727681649005751), UINT64_C(17528180369435089549), UINT64_C(13245494692151782096),
    UINT64_C(15342050348699239484), UINT64_C(7983653443908804858), UINT64_C(14161189086906129042),
    UINT64_C(2865002596868402822), UINT64_C(17542782188142908228), UINT64_C(14241581995052068062),
    UINT64_C(2443908551039927766), UINT64_C(6469035043450316832), UINT64_C(18131315169999544359),
    
};
limb_t case_17[2] = {
    // Base-64: 0123456789ABCDEFGHIJKL//
    UINT64_C(15244474820299153407),
    UINT64_C(9444138059404839728)
};
limb_t case_18[5] = {
    // Base-64: zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz++-
    UINT64_C(16104300381809926013),
    UINT64_C(9076969306111049207),
    UINT64_C(17861133150734645215),
    UINT64_C(16104300381809926013),
    UINT64_C(9076969306111049207),
};
limb_t case_20[8] = {
    /* Base-64:
    *  "////////////////////////////////
    *   ////////////////////////////////
    *   //////////////////////////////// ?1?""
    */
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX
};
limb_t case_21[6] = {
    // Base-64:
    //  "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz
    //   zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz"
    UINT64_C(16104300381809926013), UINT64_C(9076969306111049207),
    UINT64_C(17861133150734645215), UINT64_C(16104300381809926013),
    UINT64_C(9076969306111049207), UINT64_C(17861133150734645215),
};
limb_t case_22[18] = {
    /* Base-64:
    *  "abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcd
    *   abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcd
    *   abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcd
    *   abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcd
    *   ~^^+~"
    */
    UINT64_C(6460293104247396775), UINT64_C(12074812127183480722), UINT64_C(10545644249475945049),
    UINT64_C(6460293104247396775), UINT64_C(12074812127183480722), UINT64_C(10545644249475945049),
    UINT64_C(6460293104247396775), UINT64_C(12074812127183480722), UINT64_C(10545644249475945049),
    UINT64_C(6460293104247396775), UINT64_C(12074812127183480722), UINT64_C(10545644249475945049),
    UINT64_C(6460293104247396775), UINT64_C(12074812127183480722), UINT64_C(10545644249475945049),
    UINT64_C(6460293104247396775), UINT64_C(12074812127183480722), UINT64_C(10545644249475945049),
};
limb_t case_26[2] = {
    // Base-64: 1AAAAAAAAAAAAAAAA
    UINT64_C(11712218459498128010), 
    UINT64_C(4976708136) 
};
limb_t case_final[48] = {
    /* Base-64:
    *  "////////////////////////////////
    *   ////////////////////////////////
    *   ////////////////////////////////
    *   ////////////////////////////////
    *   ////////////////////////////////
    *   ////////////////////////////////
    *   ////////////////////////////////
    *   ////////////////////////////////"
    */   
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
};
// Edge-case VALUE STORAGE - BASE-PARAMETER

// Edge-case STORAGE
scase ecases_bprefix[28] = { // 128 limbs ---> 1024 bytes / 1KB
    /* ------------------------------------------------------------------------------------------------------------------------------------ */
    /* Case Number  | Input                                         | Required      | Capacity      |   Expected Ouput                      */
    /* -------------------------------------------------------- EASY SUCCESS CASE --------------------------------------------------------- */
    { /* 1.         | "0,5"                                         | 1             | 2         ---->   STR_SUCCESS (5)                     */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[0], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 2.      | "0,ABCDEFGHIJ"                                | 1             | 2         ---->   STR_SUCCESS (183293895038276755)    */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[1], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 3.      | "0,0123456789ABCDEFGHIJk"                     | 2             | 2         ---->   STR_SUCCESS (too long)              */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_3, .n = 2, .cap = 2, .sign = 1 }
        }
    }, { /* 4.      | "-0,ABCDEF01"                                 | 1             | 2         ---->   STR_SUCCESS (-44749486092289)       */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[2], .n = 1, .cap = 1, .sign = -1 }
        }
    }, { /* 5.      | "0,PIQv29.Sop5w...KnN1UxovFGvP"               | 1             | 4         ---->   STR_SUCCESS (290481608618505)       */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 5 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_5, .n = 5, .cap = 5, .sign = 1 }
        }
    }, { /* 6.      | "0,0000000001"                                | 1             | 2         ---->   STR_SUCCESS (1)                     */ 
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &one, .n = 1, .cap = 1, .sign = 1 }
        }
    }, 
    /* ---------------------------------------------------------- INCOMPLETE CASE --------------------------------------------------------- */
    { /* 7.         | "0,+"                                         | ??            | 1         ---->   STR_INCOMPLETE                      */ 
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 1 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { /* 8.      | "0,-"                                         | ??            | 1         ---->   STR_INCOMPLETE                      */ 
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 1 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { /* 9.      | "        0,+"                                 | ??            | 1         ---->   STR_INCOMPLETE                      */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 1 },
        .exp = { .type = BIGINT, .status = STR_SUCCESS, .cap = 0, INVAL_BI() }
    }, { /* 10.     | "0,0000000000000000000"                       | 1             | 1         ---->   STR_SUCCESS (0)                     */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 1 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &zero, .n = 0, .cap = 1, .sign = 1 }
        }
    }, { /* 10.     | "0,-0000000000000000000"                      | ??            | 1         ---->   STR_INVALID_SIGN                    */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 1 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 12.     | "               0,-+012"                      | 1             | 1         ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 1 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, 
    /* ------------------------------------------------------- MAIN / TRUNCATION CASE ----------------------------------------------------- */
    { /* 13.        | "0,ncgwFYphl3NqaSA9Rck5f1R.IoJEQd"            | 3             | 2         ---->   STR_TRUNC_SUCCESS                   */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 2 }, // Needs 3 limbs
        .exp = { 
            .type = BIGINT, .status = STR_TRUNC_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_13, .n = 2, .cap = 2, .sign = 1 }
        }
    }, { /* 14.     | "0,AAAAAAAAAAAA...AAAAAAAAAAAA" (truncated)   | 6             | 4         ---->   STR_TRUNC_SUCCESS                   */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 4 }, // Needs 6 limbs
        .exp = { 
            .type = BIGINT, .status = STR_TRUNC_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_14, .n = 4, .cap = 4, .sign = 1 }
        }
    }, { /* 15.     | "0,/49ln9OVrlc/...ewrMbxMaIKyk" (truncated)   | 12            | 8         ---->   STR_TRUNC_SUCCESS                   */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 8 }, // Needs 12 limbs
        .exp = {
            .type = BIGINT, .status = STR_TRUNC_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_15, .n = 8, .cap = 8, .sign = 1 }
        }
    }, { /* 16.     | "-0,KHGIVkEr0/nV...ZMUu0xgSiLIt" (truncated)  | 18            | 12        ---->   STR_TRUNC_SUCCESS                   */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 12 }, // Needs 18 limbs
        .exp = { 
            .type = BIGINT, .status = STR_TRUNC_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_16, .n = 12, .cap = 12, .sign = -1 }
        }
    }, { /* 17.     | "-0,0123456789ABCDEFGHIJKL//"                 | 3             | 2         ---->   STR_TRUNC_SUCCESS                   */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 2 }, // Needs 3 limbs
        .exp = {
            .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0,
            .data.bi = { .limbs = case_17, .n = 2, .cap = 2, .sign = -1 }
        }
    }, { /* 18.     | "0,zzzzzzzzzzzz...zzzzzzzzz++-" (truncated)   | 7             | 5         ---->   STR_TRUNC_SUCCESS                   */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 5 }, // Needs 7 limbs
            // (Technically 6, but the preprocessing of size also counts for the invalid digits)
        .exp = {
            .type = BIGINT, .status = STR_TRUNC_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_18, .n = 5, .cap = 5, .sign = 1 }
        }
    }, { /* 19.     | "0,1ab##efg"                                  | ??            | 1         ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 1 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 20.     | "-0,///////////...//////// ?!?" (truncated)   | 10            | 8         ---->   STR_TRUNC_SUCCESS                   */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 8 }, // Needs 10 limbs
            // (Technically 9, but the preprocessing of size also counts for the invalid digits)
        .exp = {
            .type = BIGINT, .status = STR_TRUNC_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_20, .n = 8, .cap = 8, .sign = -1 }
        }
    }, { /* 21.     | "0,zzzzzzzzzzzz...zzzzzzzzzzzz" (truncated)   | 12            | 6         ---->   STR_TRUNC_SUCCESS                   */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 6 }, // Needs 12 limbs
        .exp = { 
            .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0,
            .data.bi = { .limbs = case_21, .n = 6, .cap = 6, .sign = 1 }
        }
    }, { /* 22.     | "-0,abcdabcdabc...abcdabcdabcd"               | 24            | 18        ---->   STR_TRUNC_SUCCESS                   */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 18 }, // Needs 24 limbs
        .exp = { 
            .type = BIGINT, .status = STR_TRUNC_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_22, .n = 18, .cap = 18, .sign = -1 }
        }
    }, { /* 23.     | "0,JvtNiXYhkiNR...S/gwnVpMujv:" (truncated)   | 4             | 4         ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 4 }, // Needs 4 limbs
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 24.     | "0,uxYp.uBNtioOVqHcZXQj "                     | 2             | 2         ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 2 }, // Needs 2 limbs
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 25.     | "-0,////////////...////////////" (truncated)  | 24            | 8         ---->   STR_TRUNC_SUCCESS                   */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 8 }, // Needs 24 limbs
        .exp = { 
            .type = BIGINT, .status = STR_TRUNC_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_20 /* uses the same values */, .n = 8, .cap = 8, .sign = -1 }
        }
    }, { /* 26.     | "-0,0000000000000000001AAAAAAAAAAAAAAAA"      | 2             | 2         ---->   STR_SUCCESS                         */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 2 }, // Needs 2 limbs
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_26, .n = 2, .cap = 2, .sign = -1 }
        }
    }, { /* 27.     | "-0,abcdef\0ghijklmnop"                       | 1             | 1         ---->   STR_SUCCESS                         */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 1 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[3], .n = 1, .cap = 1, .sign = -1}
        }
    }, { /* 28.     | "-0,///////////...////////////" (truncated)   | 48            | 48        ---->   STR_SUCCESS                         */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 48 }, // Needs 48 limbs
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_final, .n = 48, .sign = -1 }
        }
    },
    /* ------------------------------------------------------------------------------------------------------------------------------------ */
};
scase ecases_base[28] = { // 128 limbs ---> 1024 bytes / 1KB
    /* ---------------------------------------------------------------------------------------------------------------------------------- */
    /* Case Number  | Input                                       | Required      | Capacity      |   Expected Ouput                      */
    /* ------------------------------------------------------- EASY SUCCESS CASE -------------------------------------------------------- */
    { /* 1.         | "5"                                         | 1             | 2         ---->   STR_SUCCESS (5)                     */
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[0], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 2.      | "ABCDEFGHIJ"                                | 1             | 2         ---->   STR_SUCCESS (183293895038276755)    */
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[1], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 3.      | "0123456789ABCDEFGHIJk"                     | 2             | 2         ---->   STR_SUCCESS (too long)              */
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_3, .n = 2, .cap = 2, .sign = 1 }
        }
    }, { /* 4.      | "-ABCDEF01"                                 | 1             | 2         ---->   STR_SUCCESS (-44749486092289)       */
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[2], .n = 1, .cap = 1, .sign = -1 }
        }
    }, { /* 5.      | "PIQv29.Sop5w...KnN1UxovFGvP"               | 1             | 4         ---->   STR_SUCCESS (290481608618505)       */
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 5 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_5, .n = 5, .cap = 5, .sign = 1 }
        }
    }, { /* 6.      | "0000000001"                                | 1             | 2         ---->   STR_SUCCESS (1)                     */ 
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &one, .n = 1, .cap = 1, .sign = 1 }
        }
    }, 
    /* --------------------------------------------------------- INCOMPLETE CASE -------------------------------------------------------- */
    { /* 7.         | "+"                                         | ??            | 1         ---->   STR_INCOMPLETE                      */ 
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 1 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { /* 8.      | "-"                                         | ??            | 1         ---->   STR_INCOMPLETE                      */ 
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 1 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { /* 9.      | "        +"                                 | ??            | 1         ---->   STR_INCOMPLETE                      */
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 1 },
        .exp = { .type = BIGINT, .status = STR_SUCCESS, .cap = 0, INVAL_BI() }
    }, { /* 10.     | "0000000000000000000"                       | 1             | 1         ---->   STR_SUCCESS (0)                     */
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 1 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &zero, .n = 0, .cap = 1, .sign = 1 }
        }
    }, { /* 10.     | "-0000000000000000000"                      | ??            | 1         ---->   STR_INVALID_SIGN                    */
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 1 },
        .exp = { .type = BIGINT, .status = STR_INVALID_SIGN, .cap = 0, INVAL_BI() }
    }, { /* 12.     | "               -+012"                      | 1             | 1         ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 1  },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, 
    /* ------------------------------------------------------ MAIN / TRUNCATION CASE ---------------------------------------------------- */
    { /* 13.        | "ncgwFYphl3NqaSA9Rck5f1R.IoJEQd"            | 3             | 2         ---->   STR_TRUNC_SUCCESS                   */
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 2 }, // Needs 3 limbs
        .exp = { 
            .type = BIGINT, .status = STR_TRUNC_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_13, .n = 2, .cap = 2, .sign = 1 }
        }
    }, { /* 14.     | "AAAAAAAAAAAA...AAAAAAAAAAAA" (truncated)   | 6             | 4         ---->   STR_TRUNC_SUCCESS                   */
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 4 }, // Needs 6 limbs
        .exp = { 
            .type = BIGINT, .status = STR_TRUNC_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_14, .n = 4, .cap = 4, .sign = 1 }
        }
    }, { /* 15.     | "/49ln9OVrlc/...ewrMbxMaIKyk" (truncated)   | 12            | 8         ---->   STR_TRUNC_SUCCESS                   */
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 8 }, // Needs 12 limbs
        .exp = {
            .type = BIGINT, .status = STR_TRUNC_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_15, .n = 8, .cap = 8, .sign = 1 }
        }
    }, { /* 16.     | "-KHGIVkEr0/nV...ZMUu0xgSiLIt" (truncated)  | 18            | 12        ---->   STR_TRUNC_SUCCESS                   */
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 12 }, // Needs 18 limbs
        .exp = { 
            .type = BIGINT, .status = STR_TRUNC_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_16, .n = 12, .cap = 12, .sign = -1 }
        }
    }, { /* 17.     | "-0123456789ABCDEFGHIJKL//"                 | 3             | 2         ---->   STR_TRUNC_SUCCESS                   */
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 2 }, // Needs 3 limbs
        .exp = {
            .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0,
            .data.bi = { .limbs = case_17, .n = 2, .cap = 2, .sign = -1 }
        }
    }, { /* 18.     | "zzzzzzzzzzzz...zzzzzzzzz++-" (truncated)   | 7             | 5         ---->   STR_TRUNC_SUCCESS                   */
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 5 }, // Needs 7 limbs
            // (Technically 6, but the preprocessing of size also counts for the invalid digits)
        .exp = {
            .type = BIGINT, .status = STR_TRUNC_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_18, .n = 5, .cap = 5, .sign = 1 }
        }
    }, { /* 19.     | "1ab##efg"                                  | ??            | 1         ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 1 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 20.     | "////////////...//////// ?!?" (truncated)   | 10            | 8         ---->   STR_TRUNC_SUCCESS                   */
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 8 }, // Needs 10 limbs
            // (Technically 9, but the preprocessing of size also counts for the invalid digits)
        .exp = {
            .type = BIGINT, .status = STR_TRUNC_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_20, .n = 8, .cap = 8, .sign = -1 }
        }
    }, { /* 21.     | "zzzzzzzzzzzz...zzzzzzzzzzzz" (truncated)   | 12            | 6         ---->   STR_TRUNC_SUCCESS                   */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = 6 }, // Needs 12 limbs
        .exp = { 
            .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0,
            .data.bi = { .limbs = case_21, .n = 6, .cap = 6, .sign = 1 }
        }
    }, { /* 22.     | "-abcdabcdabc...abcdabcdabcd"               | 24            | 18        ---->   STR_TRUNC_SUCCESS                   */
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 18 }, // Needs 24 limbs
        .exp = {
            .type = BIGINT, .status = STR_TRUNC_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_22, .n = 18, .cap = 18, .sign = -1 }
        }
    }, { /* 23.     | "JvtNiXYhkiNR...S/gwnVpMujv:" (truncated)   | 4             | 4         ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 4 }, // Needs 4 limbs
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 24.     | "uxYp.uBNtioOVqHcZXQj "                     | 2             | 2         ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 2 }, // Needs 2 limbs
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 25.     | "-///////////...////////////" (truncated)   | 24            | 8         ---->   STR_TRUNC_SUCCESS                   */
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 8 }, // Needs 24 limbs
        .exp = { 
            .type = BIGINT, .status = STR_TRUNC_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_20 /* uses the same values */, .n = 8, .cap = 8, .sign = -1 }
        }
    }, { /* 26.     | "-0000000000000000001AAAAAAAAAAAAAAAA"      | 2             | 2         ---->   STR_SUCCESS                         */
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 2 }, // Needs 2 limbs
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_26, .n = 2, .cap = 2, .sign = -1 }
        }
    }, { /* 27.     | "-abcdef\0ghijklmnop"                       | 1             | 1         ---->   STR_SUCCESS                         */
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 1 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[3], .n = 1, .cap = 1, .sign = -1}
        }
    }, { /* 28.     | "-///////////...////////////" (truncated)   | 48            | 48        ---->   STR_SUCCESS                         */
        .in = &(stobi_scan_in){ .base = 64, .bi_size = 48 }, // Needs 48 limbs
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_final, .n = 48, .sign = -1 }
        }
    },
    /* ------------------------------------------------------------------------------------------------------------------------------------ */
};



static inline void setup_cases(FILE *nob, FILE *b, u8 ecount) {
    for (u8 i = 0; i < ecount; i++) {
        stobi_scan_in* nob_case = (stobi_scan_in*)ecases_bprefix[i].in;
        stobi_scan_in* b_case = (stobi_scan_in*)ecases_base[i].in;
        nob_case->stream = nob; b_case->stream = b;
    }
}


int main(int argc, char **argv) { _libdnml_init();
    //* ---------------------------------- PRE-TEST SETUP ---------------------------------- *//
    // Parse terminal args + Setup env constants
    u16 rcount = (argc >= 1) ? (u16)(_stou64(argv[1], strlen(argv[1]))) : 100;
    _dnml_output_mode tscan_omode; if (argc >= 2) {
        u8 sesh_count = _stou64(argv[2], strlen(argv[2]));
        tscan_omode = (sesh_count <= 3) ? DNML_VOUT : DNML_COUT;
    } else tscan_omode = DNML_VOUT;
    u8 tscan_ecount = 28, tscan_scount = 2;
    // Buffer and Test Cases Setup
    tscan_in_nob = fopen("input_files/bi_trunc_scansa_nob.txt", "r");
    tscan_in_b = fopen("input_files/bi_trunc_scansa_b.txt", "r");
    tscan_randin = fopen("input_files/bi_rand_io.txt", "r+");
    DNML_FOPEN_ERR(tscan_in_nob, 
        "bi_trunc_scansa_nob.txt",
        "input_files/bi_trunc_scansa_nob.txt"
    );
    DNML_FOPEN_ERR(tscan_in_b,
        "bi_trunc_scansa_b.txt",
        "input_files/bi_trunc_scansa_b.txt"
    );
    DNML_FOPEN_ERR(tscan_randin, "bi_rand_io.txt", "input_files/bi_rand_io.txt");
    setup_cases(tscan_in_nob, tscan_in_b, tscan_ecount);

    // Edge Case Buffer Setup
    limb_t ectx_buf[128]; // Edge-case Memory Usage: approx 1kb
    str_res *ebuf_slices[tscan_scount], fail_ebuf[(tscan_ecount << 1) * tscan_scount];
    strbump_t tscan_ectx = { .ctx = ectx_buf, .off = 0, .size = 128 };
    _dist_buf(ebuf_slices, fail_ebuf, tscan_ecount << 1, tscan_scount, sizeof(str_res));
    // Random Case Buffer Setup
    rctx_res_t tscan_res_rctx = {0};
    rand_container tscan_rcon = {
        .in_cont_type = STREAM,
        .in_cont.stream = tscan_randin,
        .res_cont = &tscan_res_rctx
    };
    // Randomization Configuration
    xoshiro256_state scan_rstate = {0}; u64 side_mix = 0;
    __GET_ENTROPY_FAST(scan_rstate.s, sizeof(u64) << 2);
    __GET_ENTROPY_FAST(&side_mix, sizeof(u64));
    seed_xoshiro256(&scan_rstate, side_mix);
    str_rand_mod scan_rconfig = {0}, // Base-parameter / Non-base-prefix
    scan_bp_rconfig = {0}; // Base-prefix / Non-base-parameter
    strgen_init_sesh(&scan_rconfig, false, &scan_rstate);
    strgen_init_sesh(&scan_bp_rconfig, true, &scan_rstate);


    //* ---------------------------------- STRICT API SUITE ---------------------------------- *//
    FILE *idk = fopen("logs/bigInt_ftscan_sa.txt", "w"); fclose(idk); 
    // fsscan() -- Base-prefix, No length param
    suite ftscan_suite = {0};
    create_str_suite(&ftscan_suite, "bigInt_ftscan - String Stream Scan",
        tscan_ecount, rcount, ecases_bprefix, EVAL, ebuf_slices[0],
        "logs/bigInt_ftscan_sa.txt", &tscan_ectx, &tscan_rcon,
        &scan_bp_rconfig, &scan_rstate
    ); ftscan_suite.cap_mode = RANDOMIZED;
    fill_suite_reval(&ftscan_suite,
        &_stobi_scan_ingen_nob, &exec_stobi_ftscan, &eval_stobi_ftscan, 
        &stat_stobi_fscan_nobsa, &cmp_eval_stobi, &fmt_in_fsscan,
        &_stobi_scan_inlink, &_stobi_scan_insize,
        &_stobi_outlink, &_stobi_aux2link
    );
    // fsscanb() -- Base-param, No length param
    suite ftscanb_suite = {0};
    create_str_suite(&ftscanb_suite, "bigInt_ftscanb - String Stream Scan",
        tscan_ecount, rcount, ecases_bprefix, EVAL, ebuf_slices[1],
        "logs/bigInt_ftscan_sa.txt", &tscan_ectx, &tscan_rcon,
        &scan_rconfig, &scan_rstate
    ); ftscanb_suite.cap_mode = RANDOMIZED;
    fill_suite_reval(&ftscanb_suite,
        &_stobi_scan_ingen_nob, &exec_stobi_ftscanb, &eval_stobi_ftscanb, 
        &stat_stobi_fscan_bsa, &cmp_eval_stobi, &fmt_in_fsscanb,
        &_stobi_scan_inlink, &_stobi_scan_insize,
        &_stobi_outlink, &_stobi_aux2link
    );


    //* ---------------------------------- SESSION STARTUP ---------------------------------- *//
    _libdnml_str_suite tscan_suite_arr[tscan_scount];
    tscan_suite_arr[0] = ftscan_suite; 
    tscan_suite_arr[1] = ftscanb_suite;
    _libdnml_session bi_tscan_sesh = {0}; create_str_session(
        &bi_tscan_sesh, "I/O - String --> BigInt Stream Scan (Size-aware)",
        100, tscan_scount, tscan_suite_arr, tscan_omode
    ); start_str_session(&bi_tscan_sesh);
    return 0;
}