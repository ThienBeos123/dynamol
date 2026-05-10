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
limb_t one = 1;
limb_t small_mulval_bp[5] = {
    5, // Base-64: 5
    UINT16_C(3965), // Base-64: zz
    UINT64_C(183293895038276755), // Base-64: ABCDEFGHIJ
    UINT64_C(10925117571), // Base-64: ABC123
    UINT64_C(290481608618505), // Base-64: 0123456789
};
limb_t case_6[3] = {
    // Base-64: ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef
    UINT64_C(17879723973686229545),
    UINT64_C(1538390599243603321),
    UINT64_C(2932702320612428085)
};
limb_t case_9[2] = {
    // Base-64: 0123456789ABCDEFGHIJk
    UINT64_C(11730809282449712366),
    UINT64_C(1161926434474020),
};
limb_t case_10[4] = {
    // Base-64: 0123456789ABCDEFGHIJKLMNOPQRSTUVWxyz
    UINT64_C(10852392897987460925),
    UINT64_C(10322554443867199329),
    UINT64_C(1175869551687708883),
    UINT16_C(4227),
};
limb_t case_11[5] = {
    // For ecase_bprefix's CASE 11 (Range: [15] --> [19])
    // Base-64: KT7wH/q0lzdRBmxfb4tCtQbDMLozsmVMteeshvxnbJq
    UINT64_C(8802525983350871284),
    UINT64_C(11904532734475509595),
    UINT64_C(7835203037231002486),
    UINT64_C(5136330223486435175),
    1,
};
limb_t case_16[2] = {
    // Base-64: 0123456789ABCDEFGHIJ
    UINT64_C(10559587436499899539),
    UINT64_C(18155100538656),
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
        .in = &(stobi_assign_in){ .str = "0,5", .len = 3, .base = 0, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[0], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 2.      | "0,zz"                                        | 1             | 2         ---->   STR_SUCCESS (3965)                  */ 
        .in = &(stobi_assign_in){ .str = "0,zz", .len = 4, .base = 0, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[1], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 3.      | "0,ABCDEFGHIJ"                                | 1             | 2         ---->   STR_SUCCESS (183293895038276755)    */ 
        .in = &(stobi_assign_in){ .str = "0,ABCDEFGHIJ", .len = 12, .base = 0, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[2], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 4.      | "-0,ABC123"                                   | 1             | 2         ---->   STR_SUCCESS (10925117571)           */ 
        .in = &(stobi_assign_in){ .str = "-0,ABC123", .len = 9, .base = 0, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[3], .n = 1, .cap = 1, .sign = -1 }
        }
    }, { /* 5.      | "0,0123456789"                                | 1             | 4         ---->   STR_SUCCESS (290481608618505)       */ 
        .in = &(stobi_assign_in){ .str = "0,0123456789", .len = 12, .base = 0, .bi_size = 4 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[4], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 6.      | "0,ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef"          | 3             | 8         ---->   STR_SUCCESS (idk too long)          */ 
        .in = &(stobi_assign_in){ 
            .str = "0,ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef",
            .len = 34, .base = 0, .bi_size = 8
        },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_6, .n = 3, .cap = 1, .sign = 1 }
        }
    }, { /* 7.      | "0,0000000001"                                | 1             | 2         ---->   STR_SUCCESS (1)                     */ 
        .in = &(stobi_assign_in){ .str = "0,0000000001", .len = 12, .base = 0, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &one, .n = 1, .cap = 1, .sign = 1 }
        }
    },
    /* --------------------------------------------------------- ON-THE-EDGE CASE --------------------------------------------------------- */
    { /* 8.         | "0,0123456789"                                | 1             | 1         ---->   STR_SUCCESS (290481608618505)       */
        .in = &(stobi_assign_in){ .str = "0,0123456789", .len = 12, .base = 0, .bi_size = 1 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[4], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 9.      | "0,0123456789ABCDEFGHIJk"                     | 2             | 2         ---->   STR_SUCCESS (too long)              */
        .in = &(stobi_assign_in){ .str = "0,0123456789ABCDEFGHIJk", .len = 23, .base = 0, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_9, .n = 2, .cap = 2, .sign = 1 }
        }
    }, { /* 10.     | "0,0123456789ABCDEFGHIJKLMNOPQRSTUVWxyz"      | 4             | 4         ---->   STR_SUCCESS (too long)              */
        .in = &(stobi_assign_in){ 
            .str = "0,0123456789ABCDEFGHIJKLMNOPQRSTUVWxyz", 
            .len = 38, .base = 0, .bi_size = 4 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_10, .n = 4, .cap = 4, .sign = 1 }
        }
    }, { /* 11.     | "0,KT7wH/q0lzdR...MteeshvxnbJq" (truncated)   | 5             | 5         ---->   STR_SUCCESS (too long)              */
        .in = &(stobi_assign_in){
            .str = "0,KT7wH/q0lzdRBmxfb4tCtQbDMLozsmVMteeshvxnbJq",
            .len = 45, .base = 0, .bi_size = 5 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_11, .n = 5, .cap = 5, .sign = 1 }
        }
    }, { /* 12.     | "0,///////////"                               | 2             | 1         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){ .str = "0,///////////", .len = 13, .base = 0, .bi_size = 1 },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 13.     | "0,//////////////////////"                    | 3             | 2         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){ .str = "0,//////////////////////", .len = 25, .base = 0, .bi_size = 2 },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 14.     | "0,0123456789ABCDEFGHIJKLMNOPQRSTUVWxyz"      | 4             | 3         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "0,0123456789ABCDEFGHIJKLMNOPQRSTUVWxyz", 
            .len = 38, .base = 0, .bi_size = 3
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 15.     | "0,juiB7KHUrY.Y...ghF7bOpArFI" (truncated)    | 5             | 4         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "0,juiB7KHUrY.Y5bJOXVNrNHsFe8t5m.rGEghF7bOpArFI",
            .len = 46, .base = 0, .bi_size = 4
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 16.     | "-0,0123456789ABCDEFGHIJ"                     | 2             | 2         ---->   STR_SUCCESS (too big)               */
        .in = &(stobi_assign_in){ 
            .str = "-0,0123456789ABCDEFGHIJ", 
            .len = 23, .base = 0, .bi_size = 2
        },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_16, .n = 2, .cap = 2, .sign = -1 }
        }
    }, { /* 17.     | "-0,0123456789ABCDEFGHIJKL//"                 | 3             | 2         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){ 
            .str = "-0,0123456789ABCDEFGHIJKL//",
            .len = 27, .base = 0, .bi_size = 2
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 18.     | "0,AAAAAAAAAAAA...AAAAAAAAAAAA" (truncated)   | 6             | 6         ---->   STR_SUCCESS (too large)             */
        .in = &(stobi_assign_in){ 
            .str = "0,AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
            .len = 66, .base = 0, .bi_size = 6
        },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_18, .n = 6, .cap = 6, .sign = 1 }
        }
    }, { /* 19.     | "0,FFFFFFFFFFFF...FFFFFFFFFFFF" (truncated)   | 6             | 5         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){ 
            .str = "0,FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF",
            .len = 66, .base = 0, .bi_size = 5
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 20.     | "0,abababababab...abababababab" (truncated)   | 12            | 12        ---->   STR_SUCCESS (too large)             */
        .in = &(stobi_assign_in){ 
            .str = "0,"
                "abababababababababababababababab"
                "abababababababababababababababab"
                "abababababababababababababababab"
                "abababababababababababababababab",
            .len = 130, .base = 0, .bi_size = 12
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
            .len = 130, .base = 0, .bi_size = 11
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, 
    /* -------------------------------------------------------- CLEAR-FAILURE CASE -------------------------------------------------------- */
    { /* 22.        | "0,0123456789ABCDEFGHIJkl"                    | 2             | 1         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){ 
            .str = "0,0123456789ABCDEFGHIJk", 
            .len = 24, .base = 0, .bi_size = 1 // Needs 2 limbs
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 23.     | "0,0123456789ABCDEFGHIJKLMNOPQRSTUVW"         | 3             | 1         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "0,0123456789ABCDEFGHIJKLMNOPQRSTUVW",
            .len = 35, .base = 0, .bi_size = 1 // Needs 3 limbs
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 24.     | "0,W0duRnMbTl9r...ol9FjIrhpfJB" (truncated)   | 5             | 2         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "0,W0duRnMbTl9rKlPogNgirSOY9Ve/ZDj/ol9FjIrhpfJB",
            .len = 46, .base = 0, .bi_size = 2 // Needs 5 limbs
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 25.     | "0,YgWrUX3AMTJq...nmGbGrHoaI2y" (truncated)   | 6             | 3         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "0,YgWrUX3AMTJqUIxpRrTRfhALGcKNNxvcP8ATlIyizJ.nmGbGrHoaI2y",
            .len = 57, .base = 0, .bi_size = 3 // Needs 6 limbs
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 26.     | "0,YGSR6yo3f6J/...BkWDEgAv3GVf" (truncated)   | 7             | 3         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "0,YGSR6yo3f6J/64Ay/qqro0aan2OGGnZCMoiSeJ4x.IJj6jGG1DKSwXBkWDEgAv3GVf",
            .len = 68, .base = 0, .bi_size = 3 // Needs 7 limbs
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 27.     | "0,DY9k6Ck5wyJL...d/RXMBq3gn/E" (truncated)   | 9             | 5         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "-0,"
                "DY9k6Ck5wyJLdC7YJVzfpE26qhqlkiQdxlr7DfzAr.ObGc9x"
                "8nkuRrUjiDrk49tiPnkVet1LCuWBqWMylhpCd/RXMBq3gn/E", 
            .len = 99, .base = 0, .bi_size = 5 // Needs 9 limbs
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 28.     | "0,kOnBoy/D6Boq...PJFE5BQZX646" (truncated)   | 12            | 7         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "0,"
                "kOnBoy/D6Boq1N2QZDJDxZBcGoplbyu1"
                "UzYYaEFd0OdigRogduD1WNTwgVbPF.j."
                "wg8P.osJcEd01SG/wON.dLehyzgZTQ1x"
                "9cS1N7v/m1KKFHIYnsFkPJFE5BQZX646",
            .len = 130, .base = 0, .bi_size = 7 // Needs 12 limbs
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 29.     | "0,4uPRVSViikpw...clCaaH65NeHD" (truncated)   | 18            | 12        ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "-0,"
                "4uPRVSViikpwy7tf.XCxyAYCuHAq.vJCHJEDOiznJs7uELlk"
                "zCHGn4155gKYcu6MmQlgRn0DV/6Yra1TVm62jpmRZba7R4Nz"
                "BPjyq.UDnuhjj6wNsvSxuGsBEpio/gMugqwtpN4MnAXalhMg"
                "NpJGehC0/y/xkuQ82V8mn.yuJSS8Tz3wxr9TclCaaH65NeHD",
            .len = 195, .base = 0, .bi_size = 12 // Needs 18 limbs
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 30.     | "0,abc"                                       | 1             | 0         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){ .str = "0,abc", .len = 5, .base = 0, .bi_size = 0 },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, 
    /* -------------------------------------------------------- ABSOLUTE LIMIT CASE ------------------------------------------------------- */
    { /* 31.        | "0,c1p3VosXkxu...eMcVWYbIvWu" (truncated)     | 48            | 47        ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){ .str = "0,"
            "c1p3VosXkxuHRNIZzsrErMFUEqxwpEfi5i.a8yCTd1zpph70QSVRhQIXHCew8HhL"
            "VxHkIQoCBAWgsthtdvxuy87Nc0v5J.cm5.hEK0RO1DHi0rlz/Xcy6LdqvLnnHEJa"
            "H/.dy8/qXfccJDMeK45hfMvJBP3elo4H6pYae2Fey/VGrJ89ietg4sChmHyv8eET"
            "B2aatGeCtGaAy1bAnKoxdOqIfjmpIzBDRCXYUoLalH2zElHLomPRJdxy2jcUjve9"
            "ZNKLv/GgIWCYO9rxc.UxdQx4geyR3AlrbByyBPHcNj9G..vyO/V27eq6hJAO02pD"
            "0dP4pG2N.9XEOna18Q0oxnHrSda0OhSW.EpzW5yMTMERIkbguiUPOVUunLMCI0fE"
            ".HLhujMSp2b2Swb/Dbk9nLlDlHoKuOzWwaCqFi3AhzqAO9noVYhTOlcitagkj47V"
            "rA5zDuWEDeMQbLB8Q.ym6vC6SfDWrJPDoKN8okl78EyuB18eqoeMcVWYbIvWu",
        .len = 511, .base = 0, .bi_size = 47 },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 32.        | "0,//////////.../////////" (truncated)     | 48            | 48        ---->   STR_STATUS (woah big ig)            */
        .in = &(stobi_assign_in){ .str = "0,"
            "////////////////////////////////////////////////////////////////"
            "////////////////////////////////////////////////////////////////"
            "////////////////////////////////////////////////////////////////"
            "////////////////////////////////////////////////////////////////"
            "////////////////////////////////////////////////////////////////"
            "////////////////////////////////////////////////////////////////"
            "////////////////////////////////////////////////////////////////"
            "/////////////////////////////////////////////////////////////",
        .len = 511, .base = 0, .bi_size = 48 },
        .exp = { 
            .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0,
            .data.bi = { .limbs = case_final, .n = 48, .cap = 48, .sign = 1 }
        }
    }, 
    /* ------------------------------------------------------------------------------------------------------------------------------------ */
};
scase ecases_base[32] = {
    /* ------------------------------------------------------------------------------------------------------------------------------------ */
    /* Case Number  | Input                                         | Required Size | Output size   |   Expected Ouput                      */
    /* -------------------------------------------------------- EASY SUCCESS CASE --------------------------------------------------------- */
    { /* 1.         | "5"                                           | 1             | 2         ---->   STR_SUCCESS (5)                     */
        .in = &(stobi_assign_in){ .str = "5", .len = 1, .base = 64, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[0], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 2.      | "zz"                                          | 1             | 2         ---->   STR_SUCCESS (3965)                  */ 
        .in = &(stobi_assign_in){ .str = "zz", .len = 2, .base = 64, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[1], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 3.      | "ABCDEFGHIJ"                                  | 1             | 2         ---->   STR_SUCCESS (183293895038276755)    */ 
        .in = &(stobi_assign_in){ .str = "ABCDEFGHIJ", .len = 10, .base = 64, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[2], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 4.      | "-ABC123"                                     | 1             | 2         ---->   STR_SUCCESS (10925117571)           */ 
        .in = &(stobi_assign_in){ .str = "-ABC123", .len = 7, .base = 64, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[3], .n = 1, .cap = 1, .sign = -1 }
        }
    }, { /* 5.      | "0123456789"                                  | 1             | 4         ---->   STR_SUCCESS (290481608618505)       */ 
        .in = &(stobi_assign_in){ .str = "0123456789", .len = 10, .base = 64, .bi_size = 4 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[4], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 6.      | "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef"            | 3             | 8         ---->   STR_SUCCESS (idk too long)          */ 
        .in = &(stobi_assign_in){ 
            .str = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef",
            .len = 32, .base = 64, .bi_size = 8
        },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_6, .n = 3, .cap = 1, .sign = 1 }
        }
    }, { /* 7.      | "0000000001"                                  | 1             | 2         ---->   STR_SUCCESS (1)                     */ 
        .in = &(stobi_assign_in){ .str = "0000000001", .len = 10, .base = 64, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &one, .n = 1, .cap = 1, .sign = 1 }
        }
    },
    /* --------------------------------------------------------- ON-THE-EDGE CASE --------------------------------------------------------- */
    { /* 8.         | "0123456789"                                  | 1             | 1         ---->   STR_SUCCESS (290481608618505)       */
        .in = &(stobi_assign_in){ .str = "0123456789", .len = 10, .base = 64, .bi_size = 1 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &small_mulval_bp[4], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 9.      | "0123456789ABCDEFGHIJk"                       | 2             | 2         ---->   STR_SUCCESS (too long)              */
        .in = &(stobi_assign_in){ .str = "0123456789ABCDEFGHIJk", .len = 21, .base = 64, .bi_size = 2 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_9, .n = 2, .cap = 2, .sign = 1 }
        }
    }, { /* 10.     | "0123456789ABCDEFGHIJKLMNOPQRSTUVWxyz"        | 4             | 4         ---->   STR_SUCCESS (too long)              */
        .in = &(stobi_assign_in){ 
            .str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWxyz", 
            .len = 36, .base = 64, .bi_size = 4 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_10, .n = 4, .cap = 4, .sign = 1 }
        }
    }, { /* 11.     | "KT7wH/q0lzdRB...VMteeshvxnbJq" (truncated)   | 5             | 5         ---->   STR_SUCCESS (too long)              */
        .in = &(stobi_assign_in){
            .str = "KT7wH/q0lzdRBmxfb4tCtQbDMLozsmVMteeshvxnbJq",
            .len = 43, .base = 64, .bi_size = 5 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_11, .n = 5, .cap = 5, .sign = 1 }
        }
    }, { /* 12.     | "///////////"                                 | 2             | 1         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){ .str = "///////////", .len = 11, .base = 64, .bi_size = 1 },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 13.     | "//////////////////////"                      | 3             | 2         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){ .str = "//////////////////////", .len = 22, .base = 64, .bi_size = 2 },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 14.     | "0123456789ABCDEFGHIJKLMNOPQRSTUVWxyz"        | 4             | 3         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWxyz",
            .len = 36, .base = 64, .bi_size = 3
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 15.     | "juiB7KHUrY.Y5...EghF7bOpArFI" (truncated)    | 5             | 4         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "juiB7KHUrY.Y5bJOXVNrNHsFe8t5m.rGEghF7bOpArFI",
            .len = 44, .base = 64, .bi_size = 4
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 16.     | "-0123456789ABCDEFGHIJ"                       | 2             | 2         ---->   STR_SUCCESS (too big)               */
        .in = &(stobi_assign_in){
            .str = "-0123456789ABCDEFGHIJ",
            .len = 21, .base = 64, .bi_size = 2
        },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_16, .n = 2, .cap = 2, .sign = -1 }
        }
    }, { /* 17.     | "-0123456789ABCDEFGHIJKL//"                   | 3             | 2         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){ 
            .str = "-0123456789ABCDEFGHIJKL//",
            .len = 25, .base = 64, .bi_size = 2
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 18.     | "AAAAAAAAAAAAA...AAAAAAAAAAAAA" (truncated)   | 6             | 6         ---->   STR_SUCCESS (too large)             */
        .in = &(stobi_assign_in){ 
            .str = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
            .len = 64, .base = 64, .bi_size = 6
        },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_18, .n = 6, .cap = 6, .sign = 1 }
        }
    }, { /* 19.     | "FFFFFFFFFFFFF...FFFFFFFFFFFFF" (truncated)   | 6             | 5         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){ 
            .str = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF",
            .len = 64, .base = 64, .bi_size = 5
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 20.     | "ababababababa...babababababab" (truncated)   | 12            | 12        ---->   STR_SUCCESS (too large)             */
        .in = &(stobi_assign_in){ 
            .str =  "abababababababababababababababab"
                    "abababababababababababababababab"
                    "abababababababababababababababab"
                    "abababababababababababababababab",
            .len = 128, .base = 64, .bi_size = 12
        },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = case_20, .n = 12, .cap = 12, .sign = 1 }
        }
    }, { /* 21.     | "zzzzzzzzzzzzz...zzzzzzzzzzzzz" (truncated)   | 12            | 11        ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){ 
            .str =  "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz"
                    "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz"
                    "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz"
                    "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz", 
            .len = 128, .base = 0, .bi_size = 11
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, 
    /* -------------------------------------------------------- CLEAR-FAILURE CASE -------------------------------------------------------- */
    { /* 22.        | "0123456789ABCDEFGHIJkl"                      | 2             | 1         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){ 
            .str = "0123456789ABCDEFGHIJk", 
            .len = 22, .base = 64, .bi_size = 1 // Needs 2 limbs
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 23.     | "0123456789ABCDEFGHIJKLMNOPQRSTUVW"           | 3             | 1         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "0123456789ABCDEFGHIJKLMNOPQRSTUVW",
            .len = 33, .base = 64, .bi_size = 1 // Needs 3 limbs
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 24.     | "W0duRnMbTl9rK.../ol9FjIrhpfJB" (truncated)   | 5             | 2         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "W0duRnMbTl9rKlPogNgirSOY9Ve/ZDj/ol9FjIrhpfJB",
            .len = 44, .base = 64, .bi_size = 2 // Needs 5 limbs
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 25.     | "YgWrUX3AMTJqU....nmGbGrHoaI2y" (truncated)   | 6             | 3         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "YgWrUX3AMTJqUIxpRrTRfhALGcKNNxvcP8ATlIyizJ.nmGbGrHoaI2y",
            .len = 55, .base = 64, .bi_size = 3 // Needs 6 limbs
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 26.     | "YGSR6yo3f6J/6...XBkWDEgAv3GVf" (truncated)   | 7             | 3         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "YGSR6yo3f6J/64Ay/qqro0aan2OGGnZCMoiSeJ4x.IJj6jGG1DKSwXBkWDEgAv3GVf",
            .len = 66, .base = 64, .bi_size = 3 // Needs 7 limbs
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 27.     | "-DY9k6Ck5wyJL...d/RXMBq3gn/E" (truncated)    | 9             | 5         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "-DY9k6Ck5wyJLdC7YJVzfpE26qhqlkiQdxlr7DfzAr.ObGc9x"
                "8nkuRrUjiDrk49tiPnkVet1LCuWBqWMylhpCd/RXMBq3gn/E", 
            .len = 97, .base = 64, .bi_size = 5 // Needs 9 limbs
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 28.     | "kOnBoy/D6Boq1...kPJFE5BQZX646" (truncated)   | 12            | 7         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "kOnBoy/D6Boq1N2QZDJDxZBcGoplbyu1"
                "UzYYaEFd0OdigRogduD1WNTwgVbPF.j."
                "wg8P.osJcEd01SG/wON.dLehyzgZTQ1x"
                "9cS1N7v/m1KKFHIYnsFkPJFE5BQZX646",
            .len = 128, .base = 64, .bi_size = 7 // Needs 12 limbs
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 29.     | "0,4uPRVSViikpw...clCaaH65NeHD" (truncated)   | 18            | 12        ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){
            .str = "-4uPRVSViikpwy7tf.XCxyAYCuHAq.vJCHJEDOiznJs7uELlk"
                "zCHGn4155gKYcu6MmQlgRn0DV/6Yra1TVm62jpmRZba7R4Nz"
                "BPjyq.UDnuhjj6wNsvSxuGsBEpio/gMugqwtpN4MnAXalhMg"
                "NpJGehC0/y/xkuQ82V8mn.yuJSS8Tz3wxr9TclCaaH65NeHD",
            .len = 193, .base = 64, .bi_size = 12 // Needs 18 limbs
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 30.     | "abc"                                         | 1             | 0         ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){ .str = "abc", .len = 3, .base = 64, .bi_size = 0 },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, 
    /* -------------------------------------------------------- ABSOLUTE LIMIT CASE ------------------------------------------------------- */
    { /* 31.        | "c1p3VosXkxuH...oeMcVWYbIvWu" (truncated)     | 48            | 47        ---->   BIGINT_ERR_RANGE                    */
        .in = &(stobi_assign_in){ 
            .str = 
            "c1p3VosXkxuHRNIZzsrErMFUEqxwpEfi5i.a8yCTd1zpph70QSVRhQIXHCew8HhL"
            "VxHkIQoCBAWgsthtdvxuy87Nc0v5J.cm5.hEK0RO1DHi0rlz/Xcy6LdqvLnnHEJa"
            "H/.dy8/qXfccJDMeK45hfMvJBP3elo4H6pYae2Fey/VGrJ89ietg4sChmHyv8eET"
            "B2aatGeCtGaAy1bAnKoxdOqIfjmpIzBDRCXYUoLalH2zElHLomPRJdxy2jcUjve9"
            "ZNKLv/GgIWCYO9rxc.UxdQx4geyR3AlrbByyBPHcNj9G..vyO/V27eq6hJAO02pD"
            "0dP4pG2N.9XEOna18Q0oxnHrSda0OhSW.EpzW5yMTMERIkbguiUPOVUunLMCI0fE"
            ".HLhujMSp2b2Swb/Dbk9nLlDlHoKuOzWwaCqFi3AhzqAO9noVYhTOlcitagkj47V"
            "rA5zDuWEDeMQbLB8Q.ym6vC6SfDWrJPDoKN8okl78EyuB18eqoeMcVWYbIvWu",
            .len = 509, .base = 64, .bi_size = 47 
        },
        .exp = { .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0, INVAL_BI() }
    }, { /* 32.     | "///////////...//////////" (truncated)        | 48            | 48        ---->   STR_STATUS (2^3072 - 1)             */
        .in = &(stobi_assign_in){ 
            .str = "////////////////////////////////////////////////////////////////"
            "////////////////////////////////////////////////////////////////"
            "////////////////////////////////////////////////////////////////"
            "////////////////////////////////////////////////////////////////"
            "////////////////////////////////////////////////////////////////"
            "////////////////////////////////////////////////////////////////"
            "////////////////////////////////////////////////////////////////"
            "////////////////////////////////////////////////////////////////",
        .len = 512, .base = 64, .bi_size = 48 },
        .exp = {
            .type = BIGINT, .status = BIGINT_ERR_RANGE, .cap = 0,
            .data.bi = { .limbs = base_param_final, .n = 48, .cap = 48, .sign = 1 }
        }
    }, 
    /* ------------------------------------------------------------------------------------------------------------------------------------ */
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
    u8 sassign_ecount = 32, sassign_scount = 4;
    // Buffer Setup
    limb_t ectx_buf[19]; // Edge-case Memory Usage: 128 bytes
    rctx_t sassign_rctx = {0}; str_res *ebuf_slices[sassign_scount];
    str_res fail_ebuf[(sassign_ecount << 1) * sassign_scount];
    strbump_t sassign_ectx = { .ctx = ectx_buf, .off = 0, .size = 19 };
    _dist_buf(ebuf_slices, fail_ebuf, sassign_ecount << 1, sassign_scount, sizeof(str_res));
    input_container sassign_icon = { .cont_type = CTX, .cont.rctx = &sassign_rctx };


    //* ---------------------------------- SUITE SETUP ---------------------------------- *//
    // sget_str() -- Base-prefix, No length param
    suite sget_str_suite = {0};
    create_str_suite(&sget_str_suite, "bigInt_sget_str - String Assignment", 
        sassign_scount, rcount, ecases_bprefix, INVERSE, ebuf_slices[0],
        "../logs/bigInt_get_strsa.txt", sassign_ectx, &sassign_icon
    ); sget_str_suite.cap_mode = RANDOMIZED;
    fill_suite_rinv(&sget_str_suite,
        &_stobi_assign_ingen_nob, &exec_stobi_sget_str,
        &inv_stobi_assign_nob, &stat_stobi_sget_str,
        &cmp_inv_stobi_assign, &fmt_in_get_sstr, &fmt_recon_stobi
    );
    // sget_strn() -- Base-prefix, Length param
    suite sget_strn_suite = {0};
    create_str_suite(&sget_strn_suite, "bigInt_sget_strn - String Assignment",
        sassign_scount, rcount, ecases_bprefix, INVERSE, ebuf_slices[1],
        "../logs/bigInt_get_strsa.txt", sassign_ectx, &sassign_icon
    ); sget_strn_suite.cap_mode = RANDOMIZED;
    fill_suite_rinv(&sget_strn_suite,
        &_stobi_assign_ingen_nob, &exec_stobi_sget_strn,
        &inv_stobi_assign_nob, &stat_stobi_sget_strn,
        &cmp_inv_stobi_assign, &fmt_in_get_sstrn, &fmt_recon_stobi
    );
    // sget_strb() -- Base-param, No length param
    suite sget_strb_suite = {0};
    create_str_suite(&sget_strb_suite, "bigInt_sget_strb - String Assignment",
        sassign_scount, rcount, ecases_bprefix, INVERSE, ebuf_slices[1],
        "../logs/bigInt_get_strsa.txt", sassign_ectx, &sassign_icon
    ); sget_strb_suite.cap_mode = RANDOMIZED;
    fill_suite_rinv(&sget_strb_suite,
        &_stobi_assign_ingen_b, &exec_stobi_sget_strb,
        &inv_stobi_assign_b, &stat_stobi_sget_strb,
        &cmp_inv_stobi_assignb, &fmt_in_get_sstrb, &fmt_recon_stobi
    );
    // sget_strnb() -- Base-param, Length param
    suite sget_strnb_suite = {0};
    create_str_suite(&sget_strnb_suite, "bigInt_sget_strnb - String Assignment",
        sassign_scount, rcount, ecases_bprefix, INVERSE, ebuf_slices[1],
        "../logs/bigInt_get_strsa.txt", sassign_ectx, &sassign_icon
    ); sget_strnb_suite.cap_mode = RANDOMIZED;
    fill_suite_rinv(&sget_strnb_suite,
        &_stobi_assign_ingen_b, &exec_stobi_sget_strnb,
        &inv_stobi_assign_b, &stat_stobi_sget_strnb,
        &cmp_inv_stobi_assignb, &fmt_in_get_sstrnb, &fmt_recon_stobi
    );


    //* ---------------------------------- SESSION STARTUP ---------------------------------- *//
    _libdnml_str_suite sassign_suite_arr[sassign_scount];
    sassign_suite_arr[0] = sget_str_suite;  sassign_suite_arr[1] = sget_strn_suite;
    sassign_suite_arr[2] = sget_strb_suite; sassign_suite_arr[3] = sget_strnb_suite;
    _libdnml_session bi_sassign_sesh = {0}; create_str_session(
        &bi_sassign_sesh, "I/O - String --> BigInt Assignment (Size-aware)",
        100, sassign_scount, sassign_suite_arr, sassign_omode
    ); start_str_session(&bi_sassign_sesh);
    return 0;
}