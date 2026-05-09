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
*      refer to the testing unit(s):
*
*           +) stobi_sassign_runner.c
*           +) stobi_assign_runner.c
*           +) stobi_tassign_runner.c
*/


// Edge-case VALUE STORAGE - BASE-PREFIX
limb_t one = 1, zero = 0;
limb_t small_mulval_bp[35] = {
    5, // Base-64: 5
    UINT64_C(183293895038276755), // Base-64: ABCDEFGHIJ
    UINT64_C(44749486092289), // Base-64: ABCDEF01
    UINT64_C(290481608618505), // Base-64: 0123456789
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
limb_t case_18[6] = {
    UINT64_C(11712218459498128010),
    UINT64_C(9955385690573408808),
    UINT64_C(2928054614874532002),
    UINT64_C(11712218459498128010),
    UINT64_C(9955385690573408808),
    UINT64_C(2928054614874532002),
};
limb_t case_20[12] = {
    UINT64_C(6423701355093973285), /* 0 */ UINT64_C(2707324343907067282), /* 1 */
    UINT64_C(10545501312955814489), /* 2 */ UINT64_C(6423701355093973285), /* 3 */
    UINT64_C(2707324343907067282), /* 4 */ UINT64_C(10545501312955814489), /* 5 */
    UINT64_C(6423701355093973285), /* 6 */ UINT64_C(2707324343907067282), /* 7 */
    UINT64_C(10545501312955814489), /* 8 */ UINT64_C(6423701355093973285), /* 9 */
    UINT64_C(2707324343907067282), /* 10 */ UINT64_C(10545501312955814489) /* 11 */
};
limb_t case_final[48] = {
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_C(70368744177663),
};
limb_t base_param_final[48] = {
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX
};
// Edge-case VALUE STORAGE - BASE-PARAMETER

// Edge-case STORAGE
scase ecases_bprefix[32] = {
    /* ------------------------------------------------------------------------------------------------------------------------------------ */
    /* Case Number  | Input                                         | Required Size | Output size   |   Expected Ouput                      */
    /* -------------------------------------------------------- EASY SUCCESS CASE --------------------------------------------------------- */
    { /* 1.         | "0,5"                                         | 1             | 2         ---->   STR_SUCCESS (5)                     */
        .in = &(stobi_assign_in){ .str = "0,5", .len = 4, .base = 0, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[0], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 2.      | "0,ABCDEFGHIJ"                                | 1             | 2         ---->   STR_SUCCESS (183293895038276755)    */
        .in = &(stobi_assign_in){ .str = "0,ABCDEFGHIJ", .len = 13, .base = 0, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[1], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 3.      | "0,0123456789ABCDEFGHIJk"                     | 2             | 2         ---->   STR_SUCCESS (too long)              */ 
        .in = &(stobi_assign_in){ .str = "0,0123456789ABCDEFGHIJk", .len = 24, .base = 0, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_3, .n = 2, .cap = 2, .sign = 1 }
        }
    }, { /* 4.      | "-0,ABCDEF01"                                 | 1             | 2         ---->   STR_SUCCESS (-44749486092289)       */ 
        .in = &(stobi_assign_in){ .str = "-0,ABCDEF01", .len = 11, .base = 0, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[2], .n = 1, .cap = 1, .sign = -1 }
        }
    }, { /* 5.      | "0,PIQv29.Sop5w...KnN1UxovFGvP"               | 1             | 4         ---->   STR_SUCCESS (290481608618505)       */ 
        .in = &(stobi_assign_in){ 
            .str = "0,PIQv29.Sop5wYEzB8mbYnoryDDJmX8Au4rpzKnN1UxovFGvP", 
            .len = 51, .base = 0, .bi_size = 5
        },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_5, .n = 5, .cap = 5, .sign = 1 }
        }
    }, { /* 6.      | "0,0000000001"                                | 1             | 2         ---->   STR_SUCCESS (1)                     */ 
        .in = &(stobi_assign_in){ .str = "0,0000000001", .len = 13, .base = 0, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &one, .n = 1, .cap = 1, .sign = 1 }
        }
    }, 
    /* ---------------------------------------------------------- INCOMPLETE CASE --------------------------------------------------------- */
    { /* 7.         | "0,+"                                         | ??            | 1         ---->   STR_INCOMPLETE                      */ 
        .in = &(stobi_assign_in){ .str = "0,+", .len = 4, .base = 0, .bi_size = 1 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { /* 8.      | "0,-"                                         | ??            | 1         ---->   STR_INCOMPLETE                      */ 
        .in = &(stobi_assign_in){ .str = "0,-", .len = 4, .base = 0, .bi_size = 1 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { /* 9.      | "        0,+"                                 | ??            | 1         ---->   STR_INCOMPLETE                      */
        .in = &(stobi_assign_in){ .str = "        0,+", .len = 12, .base = 0, .bi_size = 1 },
        .exp = { .type = BIGINT, .status = STR_SUCCESS, .cap = 0, INVAL_BI() }
    }, { /* 10.     | "0,0000000000000000000"                       | 1             | 1         ---->   STR_SUCCESS (0)                     */
        .in = &(stobi_assign_in){ 
            .str = "0,0000000000000000000",
            .len = 22, .base = 0, .bi_size = 1
        },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &zero, .n = 0, .cap = 1, .sign = 1 }
        }
    }, { /* 10.     | "0,-0000000000000000000"                      | ??            | 1         ---->   STR_INVALID_SIGN                    */
        .in = &(stobi_assign_in){ 
            .str = "0,-0000000000000000000",
            .len = 23, .base = 0, .bi_size = 1
        },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 12.     | "               0,-+012"                      | 1             | 1         ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_assign_in){
            .str = "               0,-+012", 
            .len = 23, .base = 0, .bi_size = 1 
        },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, 
    /* ------------------------------------------------------- MAIN / TRUNCATION CASE ----------------------------------------------------- */
    { /* 13.        | "0,ncgwFYphl3NqaSA9Rck5f1R.IoJEQd"            | 3             | 2         ---->   STR_TRUNC_SUCCESS                   */
        .in = &(stobi_assign_in){ 
            .str = "0,ncgwFYphl3NqaSA9Rck5f1R.IoJEQd", 
            .len = 33, .base = 0, .bi_size = 2 // Needs 3 limbs
        },
        .exp = { 
            .type = BIGINT, .status = STR_TRUNC_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_13, .n = 2, .cap = 2, .sign = 1 }
        }
    }, { /* 14.     | "0,AAAAAAAAAAAA...AAAAAAAAAAAA" (truncated)   | 6             | 4         ---->   STR_TRUNC_SUCCESS                   */
        .in = &(stobi_assign_in){
            .str = "0,AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", 
            .len = 67, .base = 0, .bi_size = 4 // Needs 6 limbs
        },
        .exp = { 
            .type = BIGINT, .status = STR_TRUNC_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_14, .n = 4, .cap = 4, .sign = 1 }
        }
    }, { /* 15.     | "0,/49ln9OVrlc/...ewrMbxMaIKyk" (truncated)   | 12            | 8         ---->   STR_TRUNC_SUCCESS                   */
        .in = &(stobi_assign_in){
            .str = "0,"
                "/49ln9OVrlc/H2iWWmMsnzNqgapOqVdv9gZaGDcPTqtSi.tYGx0FQetwxgk/hXxR"
                "q2CBC/VIplL4td8unHK.TuzGKCnPiff/OjVKtpX2XIjP/ibpsdo3ewrMbxMaIKyk",
            .len = 131, .base = 0, .bi_size = 8 // Needs 12 limbs
        },
        .exp = {
            .type = BIGINT, .status = STR_TRUNC_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_15, .n = 8, .cap = 8, .sign = 1 }
        }
    }, { /* 16.     | "-0,KHGIVkEr0/nV...ZMUu0xgSiLIt" (truncated)  | 18            | 12        ---->   STR_TRUNC_SUCCESS                   */
        .in = &(stobi_assign_in){
            .str = "-0,"
                "KHGIVkEr0/nVU1Jy93oDfIKvU5GUYo5ZP.V7XabRM99Hv8SRxVdw5NqP7mXQ03yK"
                ".vzV5L9vO2TPng4O/6Ze827gWU3uHaNMnQH3NewjRDxpT7jqM4xJH2V2YAwowQw6"
                "n8QcSW8jcf9kowA23W8W.jJfwO6eO4Gyjz5pFh07Qj3pG9ijVoTQZMUu0xgSiLIt",
            .len = 196, .base = 0, .bi_size = 12
        },
        .exp = { 
            .type = BIGINT, .status = STR_TRUNC_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_16, .n = 12, .cap = 12, .sign = -1 }
        }
    }, 
    //todo FINISHES THESE REMAINING CASES BELOW:
    { /* 17.     | "-0,0123456789ABCDEFGHIJKL//"                 | 3             | 2         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){ 
            .str = "-0,0123456789ABCDEFGHIJKL//",
            .len = 28, .base = 0, .bi_size = 2
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 18.     | "0,AAAAAAAAAAAA...AAAAAAAAAAAA" (truncated)   | 6             | 6         ---->   STR_SUCCESS (too large)             */
        .in = &(stobi_assign_in){ 
            .str = "0,AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
            .len = 67, .base = 0, .bi_size = 6
        },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_18, .n = 6, .cap = 6, .sign = 1 }
        }
    }, { /* 19.     | "0,FFFFFFFFFFFF...FFFFFFFFFFFF" (truncated)   | 6             | 5         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){ 
            .str = "0,FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF",
            .len = 67, .base = 0, .bi_size = 5
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 20.     | "0,abababababab...abababababab" (truncated)   | 12            | 12        ---->   STR_SUCCESS (too large)             */
        .in = &(stobi_assign_in){ 
            .str = "0,"
                "abababababababababababababababab"
                "abababababababababababababababab"
                "abababababababababababababababab"
                "abababababababababababababababab",
            .len = 131, .base = 0, .bi_size = 12
        },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_20, .n = 12, .cap = 12, .sign = 1 }
        }
    }, { /* 21.     | "0,zzzzzzzzzzzz...zzzzzzzzzzzz" (truncated)   | 12            | 11        ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){ 
            .str = "0,"
                "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz"
                "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz"
                "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz"
                "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz", 
            .len = 131, .base = 0, .bi_size = 11
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 22.     | "0,0123456789ABCDEFGHIJkl"                    | 2             | 1         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){ 
            .str = "0,0123456789ABCDEFGHIJk", 
            .len = 25, .base = 0, .bi_size = 1 // Needs 2 limbs
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 23.     | "0,0123456789ABCDEFGHIJKLMNOPQRSTUVW"         | 3             | 1         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "0,0123456789ABCDEFGHIJKLMNOPQRSTUVW",
            .len = 36, .base = 0, .bi_size = 1 // Needs 3 limbs
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 24.     | "0,W0duRnMbTl9r...ol9FjIrhpfJB" (truncated)   | 5             | 2         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "0,W0duRnMbTl9rKlPogNgirSOY9Ve/ZDj/ol9FjIrhpfJB",
            .len = 47, .base = 0, .bi_size = 2 // Needs 5 limbs
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 25.     | "0,YgWrUX3AMTJq...nmGbGrHoaI2y" (truncated)   | 6             | 3         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "0,YgWrUX3AMTJqUIxpRrTRfhALGcKNNxvcP8ATlIyizJ.nmGbGrHoaI2y",
            .len = 58, .base = 0, .bi_size = 3 // Needs 6 limbs
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 26.     | "0,YGSR6yo3f6J/...BkWDEgAv3GVf" (truncated)   | 7             | 3         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "0,YGSR6yo3f6J/64Ay/qqro0aan2OGGnZCMoiSeJ4x.IJj6jGG1DKSwXBkWDEgAv3GVf",
            .len = 69, .base = 0, .bi_size = 3 // Needs 7 limbs
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 27.     | "0,DY9k6Ck5wyJL...d/RXMBq3gn/E" (truncated)   | 9             | 5         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "-0,"
                "DY9k6Ck5wyJLdC7YJVzfpE26qhqlkiQdxlr7DfzAr.ObGc9x"
                "8nkuRrUjiDrk49tiPnkVet1LCuWBqWMylhpCd/RXMBq3gn/E", 
            .len = 100, .base = 0, .bi_size = 5 // Needs 9 limbs
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 28.     | "0,kOnBoy/D6Boq...PJFE5BQZX646" (truncated)   | 12            | 7         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "0,"
                "kOnBoy/D6Boq1N2QZDJDxZBcGoplbyu1"
                "UzYYaEFd0OdigRogduD1WNTwgVbPF.j."
                "wg8P.osJcEd01SG/wON.dLehyzgZTQ1x"
                "9cS1N7v/m1KKFHIYnsFkPJFE5BQZX646",
            .len = 131, .base = 0, .bi_size = 7 // Needs 12 limbs
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 29.     | "0,4uPRVSViikpw...clCaaH65NeHD" (truncated)   | 18            | 12        ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "-0,"
                "4uPRVSViikpwy7tf.XCxyAYCuHAq.vJCHJEDOiznJs7uELlk"
                "zCHGn4155gKYcu6MmQlgRn0DV/6Yra1TVm62jpmRZba7R4Nz"
                "BPjyq.UDnuhjj6wNsvSxuGsBEpio/gMugqwtpN4MnAXalhMg"
                "NpJGehC0/y/xkuQ82V8mn.yuJSS8Tz3wxr9TclCaaH65NeHD",
            .len = 196, .base = 0, .bi_size = 12 // Needs 18 limbs
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 30.     | "0,abc"                                       | 1             | 0         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){ .str = "0,abc", .len = 6, .base = 0, .bi_size = 0 },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 31.     | "0,c1p3VosXkxu...eMcVWYbIvWu" (truncated)     | 48            | 47        ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){ .str = "0,"
            "c1p3VosXkxuHRNIZzsrErMFUEqxwpEfi5i.a8yCTd1zpph70QSVRhQIXHCew8HhL"
            "VxHkIQoCBAWgsthtdvxuy87Nc0v5J.cm5.hEK0RO1DHi0rlz/Xcy6LdqvLnnHEJa"
            "H/.dy8/qXfccJDMeK45hfMvJBP3elo4H6pYae2Fey/VGrJ89ietg4sChmHyv8eET"
            "B2aatGeCtGaAy1bAnKoxdOqIfjmpIzBDRCXYUoLalH2zElHLomPRJdxy2jcUjve9"
            "ZNKLv/GgIWCYO9rxc.UxdQx4geyR3AlrbByyBPHcNj9G..vyO/V27eq6hJAO02pD"
            "0dP4pG2N.9XEOna18Q0oxnHrSda0OhSW.EpzW5yMTMERIkbguiUPOVUunLMCI0fE"
            ".HLhujMSp2b2Swb/Dbk9nLlDlHoKuOzWwaCqFi3AhzqAO9noVYhTOlcitagkj47V"
            "rA5zDuWEDeMQbLB8Q.ym6vC6SfDWrJPDoKN8okl78EyuB18eqoeMcVWYbIvWu",
        .len = 512, .base = 0, .bi_size = 47 },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 32.     | "0,//////////.../////////" (truncated)        | 48            | 48        ---->   STR_STATUS (woah big ig)            */
        .in = &(stobi_assign_in){ .str = "0,"
            "////////////////////////////////////////////////////////////////"
            "////////////////////////////////////////////////////////////////"
            "////////////////////////////////////////////////////////////////"
            "////////////////////////////////////////////////////////////////"
            "////////////////////////////////////////////////////////////////"
            "////////////////////////////////////////////////////////////////"
            "////////////////////////////////////////////////////////////////"
            "/////////////////////////////////////////////////////////////",
        .len = 512, .base = 0, .bi_size = 48 },
        .exp = { 
            .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0,
            .data.bi = { .limbs = case_final, .n = 48, .cap = 48, .sign = 1 }
        }
    }, 
    /* ------------------------------------------------------------------------------------------------------------------------------------ */
};
scase ecases_base[32] = {};


// Main Code
int main(int argc, char **argv) {
    //* ---------------------------------- PRE-TEST SETUP ---------------------------------- *//
    // Parse terminal args + Setup env constants
    u16 rcount = (argc >= 1) ? (u16)(_stou64(argv[1], strlen(argv[1]))) : 100;
    _dnml_output_mode tassign_omode; if (argc >= 2) {
        u8 sesh_count = _stou64(argv[2], strlen(argv[2]));
        tassign_omode = (sesh_count <= 3) ? DNML_VOUT : DNML_COUT;
    } else tassign_omode = DNML_VOUT;
    u8 tassign_ecount = 32, tassign_scount = 4;
    // Buffer Setup
    limb_t ectx_buf[19]; // Edge-case Memory Usage: 128 bytes
    rctx_t tassign_rctx = {0}; str_res *ebuf_slices[tassign_scount];
    str_res fail_ebuf[(tassign_ecount << 1) * tassign_scount];
    strbump_t tassign_ectx = { .ctx = ectx_buf, .off = 0, .size = 19 };
    _dist_buf(ebuf_slices, fail_ebuf, tassign_ecount << 1, tassign_scount, sizeof(str_res));
    input_container tassign_icon = { .cont_type = CTX, .cont.rctx = &tassign_rctx };


    //* ---------------------------------- SUITE SETUP ---------------------------------- *//
    // tget_str() -- Base-prefix, No length param
    suite tget_str_suite = {0};
    create_str_suite(&tget_str_suite, "bigInt_tget_str - String Assignment", 
        tassign_scount, rcount, ecases_bprefix, EVAL, ebuf_slices[0],
        "../logs/bigInt_get_strsa.txt", tassign_ectx, &tassign_icon
    ); tget_str_suite.cap_mode = RANDOMIZED;
    fill_suite_reval(&tget_str_suite,
        &_stobi_assign_ingen_nob, &exec_stobi_tget_str,
        &eval_stobi_tget_str, &stat_stobi_sget_str, &cmp_eval_stobi
    );
    // tget_strn() -- Base-prefix, Length param
    suite tget_strn_suite = {0};
    create_str_suite(&tget_strn_suite, "bigInt_tget_strn - String Assignment",
        tassign_scount, rcount, ecases_bprefix, EVAL, ebuf_slices[1],
        "../logs/bigInt_get_strsa.txt", tassign_ectx, &tassign_icon
    ); tget_strn_suite.cap_mode = RANDOMIZED;
    fill_suite_reval(&tget_str_suite,
        &_stobi_assign_ingen_nob, &exec_stobi_tget_strn,
        &eval_stobi_tget_str, &stat_stobi_sget_strn, &cmp_eval_stobi
    );
    // tget_strb() -- Base-param, No length param
    suite tget_strb_suite = {0};
    create_str_suite(&tget_strb_suite, "bigInt_tget_strb - String Assignment",
        tassign_scount, rcount, ecases_bprefix, EVAL, ebuf_slices[1],
        "../logs/bigInt_get_strsa.txt", tassign_ectx, &tassign_icon
    ); tget_strb_suite.cap_mode = RANDOMIZED;
    fill_suite_reval(&tget_str_suite,
        &_stobi_assign_ingen_b, &exec_stobi_tget_strb,
        &eval_stobi_tget_strb, &stat_stobi_sget_strb, &cmp_eval_stobi
    );
    // tget_strnb() -- Base-param, Length param
    suite tget_strnb_suite = {0};
    create_str_suite(&tget_strnb_suite, "bigInt_tget_strnb - String Assignment",
        tassign_scount, rcount, ecases_bprefix, EVAL, ebuf_slices[1],
        "../logs/bigInt_get_strsa.txt", tassign_ectx, &tassign_icon
    ); tget_strnb_suite.cap_mode = RANDOMIZED;
    fill_suite_reval(&tget_str_suite,
        &_stobi_assign_ingen_b, &exec_stobi_tget_strnb,
        &eval_stobi_tget_strb, &stat_stobi_sget_strnb, &cmp_eval_stobi
    );


    //* ---------------------------------- SESSION STARTUP ---------------------------------- *//
    _libdnml_str_suite tassign_suite_arr[tassign_scount];
    tassign_suite_arr[0] = tget_str_suite;  tassign_suite_arr[1] = tget_strn_suite;
    tassign_suite_arr[2] = tget_strb_suite; tassign_suite_arr[3] = tget_strnb_suite;
    _libdnml_session bi_tassign_sesh = {0}; create_str_session(
        &bi_tassign_sesh, "I/O - String --> BigInt Assignment (Size-aware)",
        100, tassign_scount, tassign_suite_arr, tassign_omode
    ); start_str_session(&bi_tassign_sesh);
    return 0;
}