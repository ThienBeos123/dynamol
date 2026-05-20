// STDLIB & /include Utilities
#include <include.h>
#include <dnml_sys/sys.h>
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
*      numerical-handling correctness. For size-handling API testing, please refer to:
*
*       +) "stobi_fscan_strict.c"
*       +) "stobi_fscan_trunc.c"
*/

// THESE HAS TO BE INITIATED IN main()
FILE *scan_in_nob, *scan_in_b, *scan_randin;

// Test cases and Values
limb_t multi_val[7] = {123, 255, 42, 63, 0, 17720, 22875};
limb_t multi_valb[6] = {
    123456789 /* Decimal */, 17043707 /* Octal */, 
    4726791790 /* Base-12 */, 4095 /* Hexadecimal */, 
    22971 /* Base-7 */ , 72433 /* Binary */
}; limb_t big_limbs[9] = {
    0, 1, // First bigInt
    // Second bigInt
    UINT64_C(18446744073709551615),
    UINT64_C(18446744073709551615),
    UINT64_C(279109966635548160),
    UINT64_C(2554917779393558781),
    UINT64_C(14124656261812188652),
    UINT64_C(11976055582626787546),
    UINT64_C(2537941837315),
}; 
scase ecases_bprefix[27] = { // 19 limbs ---> 152 bytes
    /* -------------------------------------------------------------------------------------------- */
    /* Case Number  |   Input                               |   Expected Ouput                      */
    /* ------------------------------ PREPARSE LEXICAL FAILURE CASE ------------------------------- */ 
    { /* 1.         |   ""                              ---->   STR_EMPTY                           */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_EMPTY, .cap = 0, INVAL_BI() }
    }, { /* 2.      |   "  "                            ---->   STR_EMPTY                           */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_EMPTY, .cap = 0, INVAL_BI() }
    }, { /* 3.      |   "+"                             ---->   STR_INCOMPLETE                      */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { /* 4.      |   "-"                             ---->   STR_INCOMPLETE                      */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { /* 5.      |   "-0"                            ---->   STR_INVALID_SIGN                    */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_INVALID_SIGN, .cap = 0, INVAL_BI() }
    },
    /* --------------------------------- ON-THE-EDGE SUCCESS CASE --------------------------------- */ 
    { /* 6.         |   "+0"                            ---->   STR_SUCCESS (0)                     */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[4], .n = 0, .cap = 1, .sign = 1 }
        }
    }, { /* 7.      |   "0"                             ---->   STR_SUCCESS (0)                     */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[4], .n = 0, .cap = 1, .sign = 1 }
        }
    }, { /* 8.      |   "000000"                        ---->   STR_SUCCESS (0)                     */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[4], .n = 0, .cap = 1, .sign = 1 }
        }
    }, { /* 9.      |   "   123"                        ---->   STR_SUCCESS (123)                   */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[0], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 10.     |   "18446744073709551616"          ---->   STR_SUCCESS (18446744073709551616)  */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0, 
            .data.bi = { .limbs = big_limbs, .n = 2, .cap = 2, .sign = 1 }
        }
    }, { /* 11.     |   "999999...999999" (Len 128)     ---->   STR_SUCCESS (10^128 - 1)            */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0, 
            .data.bi = { .limbs = &big_limbs[2], .n = 7, .cap = 7, .sign = 1 }
        }
    },
    /* -------------------------- NUMERICAL-SEGMENT DIGIT-BASED FAILURE --------------------------- */
    { /* 12.        |   "1234abcd"                      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 13.     |   "1234    "                      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 14.     |   "abcd1234"                      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 15.     |   "1234_5678"                     ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, 
    /* ----------------------------------- BASE-SPECIFIC FAILURE ---------------------------------- */
    { /* 16.        |   "0x"                            ---->   STR_INCOMPLETE                      */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { /* 17.     |   "0xFG"                          ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 18.     |   "0b102"                         ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 19.     |   "0o89"                          ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 20.     |   "00x123"                        ---->   STR_INVALID_BASE_PREFIX             */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_INVALID_BASE_PREFIX, .cap = 0, INVAL_BI() }
    }, { /* 21.     |   "0{11)1234A"                    ---->   STR_INVALID_BASE_PREFIX             */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_INVALID_BASE_PREFIX, .cap = 0, INVAL_BI() }
    },
    /* ---------------------------------- BASE-SPECIFIC SUCCESSES --------------------------------- */
    { /* 22.        |   "0x0"                           ---->   STR_SUCCESS (0)                     */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[4], .n = 0, .cap = 1, .sign = 1 }
        }
    }, { /* 23.     |   "0xFF"                          ---->   STR_SUCCESS (255)                   */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[1], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 24.     |   "0b101010"                      ---->   STR_SUCCESS (42)                    */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[2], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 25.     |   "0o77"                          ---->   STR_SUCCESS (63)                    */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[3], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 26.     |   "0{11}1234A"                    ---->   STR_SUCCESS (17720)                 */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[5], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 27.     |   "0{7}123456"                    ---->   STR_SUCCESS (22875)                 */
        .in = &(stobi_scan_in){ .base = 0, .bi_size = BIGINT_CAP },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[6], .n = 1, .cap = 1, .sign = 1 }
        }
    }, 
    /* -------------------------------------------------------------------------------------------- */
};
scase ecases_base[27] = { // 19 limbs ---> 152 bytes
    /* -------------------------------------------------------------------------------------------- */
    /* Case Number  |   Input                               |   Expected Ouput                      */
    /* ------------------------------ PREPARSE LEXICAL FAILURE CASE ------------------------------- */ 
    { /* 1.         |   ""                              ---->   STR_EMPTY                           */
        .in = &(stobi_scan_in){ .base = 10, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_EMPTY, .cap = 0, INVAL_BI() }
    }, { /* 2.      |   "  "                            ---->   STR_EMPTY                           */
        .in = &(stobi_scan_in){ .base = 10, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_EMPTY, .cap = 0, INVAL_BI() }
    }, { /* 3.      |   "+"                             ---->   STR_INCOMPLETE                      */
        .in = &(stobi_scan_in){ .base = 10, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { /* 4.      |   "-"                             ---->   STR_INCOMPLETE                      */
        .in = &(stobi_scan_in){ .base = 10, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { /* 5.      |   "-0"                            ---->   STR_INVALID_SIGN                    */
        .in = &(stobi_scan_in){ .base = 10, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_INVALID_SIGN, .cap = 0, INVAL_BI() }
    },
    /* --------------------------------- ON-THE-EDGE SUCCESS CASE --------------------------------- */ 
    { /* 6.         |   "+0"                            ---->   STR_SUCCESS (0)                     */
        .in = &(stobi_scan_in){ .base = 10, .bi_size = BIGINT_CAP },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[4], .n = 0, .cap = 1, .sign = 1 }
        }
    }, { /* 7.      |   "0"                             ---->   STR_SUCCESS (0)                     */
        .in = &(stobi_scan_in){ .base = 10, .bi_size = BIGINT_CAP },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[4], .n = 0, .cap = 1, .sign = 1 }
        }
    }, { /* 8.      |   "000000"                        ---->   STR_SUCCESS (0)                     */
        .in = &(stobi_scan_in){ .base = 10, .bi_size = BIGINT_CAP },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[4], .n = 0, .cap = 1, .sign = 1 }
        }
    }, { /* 9.      |   "   123"                        ---->   STR_SUCCESS (123)                   */
        .in = &(stobi_scan_in){ .base = 10, .bi_size = BIGINT_CAP },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[0], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 10.     |   "18446744073709551616"          ---->   STR_SUCCESS (18446744073709551616)  */
        .in = &(stobi_scan_in){ .base = 10, .bi_size = BIGINT_CAP },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0, 
            .data.bi = { .limbs = big_limbs, .n = 2, .cap = 2, .sign = 1 }
        }
    }, { /* 11.     |   "999999...999999" (Len 128)     ---->   STR_SUCCESS (10^128 - 1)            */
        .in = &(stobi_scan_in){ .base = 10, .bi_size = BIGINT_CAP },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0, 
            .data.bi = { .limbs = &big_limbs[2], .n = 7, .cap = 7, .sign = 1 }
        }
    },
    /* -------------------------- NUMERICAL-SEGMENT DIGIT-BASED FAILURE --------------------------- */
    { /* 12.        |   "1234abcd"                      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 10, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 13.     |   "1234    "                      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 10, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 14.     |   "abcd1234"                      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 10, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 15.     |   "1234_5678"                     ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 10, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, 
    /* -------------------------------------------------------------------------------------------- */
    /* Case Number  |   Input               |   Base        |   Expected Ouput                      */
    /* ----------------------------------- BASE-SPECIFIC FAILURE ---------------------------------- */
    { /* 16.        |   "-0000"             |   8       ---->   STR_INVALID_SIGN                    */
        .in = &(stobi_scan_in){ .base = 8, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_INVALID_SIGN, .cap = 0, INVAL_BI() }
    }, { /* 17.     |   "-FGFF"             |   16      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 16, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 18.     |   "111222~"           |   11      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 11, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 19.     |   ".91234"            |   10      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 10, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 20.     |   "     101 "         |   2       ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 2, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 21.     |   "1234567"           |   7       ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 7, .bi_size = BIGINT_CAP },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    },
    /* ---------------------------------- BASE-SPECIFIC SUCCESSES --------------------------------- */
    { /* 22.        |   "000123456789"      |   10      ---->   STR_SUCCESS (123456789)             */
        .in = &(stobi_scan_in){ .base = 10, .bi_size = BIGINT_CAP },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_valb[0], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 23.     |   "-101010373"        |   8       ---->   STR_SUCCESS (17043707)              */
        .in = &(stobi_scan_in){ .base = 8, .bi_size = BIGINT_CAP },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0, 
            .data.bi =  { .limbs = &multi_valb[1], .n = 1, .cap = 1, .sign = -1 }
        }
    }, { /* 24.     |   "-ABABABABA"        |   12      ---->   STR_SUCCESS (4726791790)            */
        .in = &(stobi_scan_in){ .base = 12, .bi_size = BIGINT_CAP },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0, 
            .data.bi = { .limbs = &multi_valb[2], .n = 1, .cap = 1, .sign = -1 }
        }
    }, { /* 25.     |   "      -FFF"        |   16      ---->   STR_SUCCESS (4095)                  */
        .in = &(stobi_scan_in){ .base = 16, .bi_size = BIGINT_CAP },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0, 
            .data.bi = { .limbs = &multi_valb[3], .n = 1, .cap = 1, .sign = -1 }
        }
    }, { /* 26.     |   "  0000123654"      |   7       ---->   STR_SUCCESS (22971)                 */
        .in = &(stobi_scan_in){ .base = 7, .bi_size = BIGINT_CAP },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_valb[4], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 27.     |   "10001101011110001" |   2       ---->   STR_SUCCESS (72433)                 */
        .in = &(stobi_scan_in){ .base = 2, .bi_size = BIGINT_CAP },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_valb[5], .n = 1, .cap = 1, .sign = 1 }
        }
    }, 
    /* -------------------------------------------------------------------------------------------- */
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
    _dnml_output_mode scan_omode; if (argc >= 2) {
        u8 sesh_count = _stou64(argv[2], strlen(argv[2]));
        scan_omode = (sesh_count <= 3) ? DNML_VOUT : DNML_COUT;
    } else scan_omode = DNML_VOUT;
    u8 scan_ecount = 27, scan_scount = 6;
    // Buffer and Test Cases Setup
    scan_in_nob = fopen("input_files/bi_scan_nob.txt", "r");
    scan_in_b = fopen("input_files/bi_scan_b.txt", "r");
    scan_randin = fopen("input_files/bi_rand_io.txt", "r+");
    DNML_FOPEN_ERR(scan_in_nob, "bi_scan_nob.txt", "input_files/bi_scan_nob.txt");
    DNML_FOPEN_ERR(scan_in_b, "bi_scan_b.txt", "input_files/bi_scan_b.txt");
    DNML_FOPEN_ERR(scan_randin, "bi_rand_io.txt", "input_files/bi_rand_io.txt");
    setup_cases(scan_in_nob, scan_in_b, scan_ecount);
    
    // Edge Case Buffer Setup
    limb_t ectx_buf[19]; // Edge-case Memory Usage: 152 bytes
    str_res *ebuf_slices[scan_scount], fail_ebuf[(scan_ecount << 1) * scan_scount];
    strbump_t scan_ectx = { .ctx = ectx_buf, .off = 0, .size = 19 };
    _dist_buf(ebuf_slices, fail_ebuf, scan_ecount << 1, scan_scount, sizeof(str_res));
    // Random Case Buffer Setup
    rctx_res_t scan_res_rctx = {0};
    rand_container scan_rcon = {
        .in_cont_type = STREAM,
        .in_cont.stream = scan_randin,
        .res_cont = &scan_res_rctx
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


    //* ---------------------------------- STANDARD API SUITE ---------------------------------- *//
    FILE *idk = fopen("logs/bigInt_fscan.txt", "w"); fclose(idk); 
    // fscan() -- Base-prefix, No length param
    suite fscan_suite = {0};
    create_str_suite(&fscan_suite, "bigInt_fscan - String Stream Scan", 
        scan_ecount, rcount, ecases_bprefix, INVERSE, ebuf_slices[0],
        "logs/bigInt_fscan.txt", &scan_ectx, &scan_rcon,
        &scan_bp_rconfig, &scan_rstate
    ); fscan_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&fscan_suite,
        &_stobi_scan_ingen_nob, &exec_stobi_fscan,
        &inv_stobi_scan_nob, &stat_stobi_fscan_nob,
        &cmp_inv_stobi_scan, &fmt_in_fscan, &fmt_recon_stobi,
        &_stobi_scan_inlink, &_stobi_scan_insize,
        &_stobi_recon_linker, &_stobi_recon_size,
        &_stobi_outlink, &_stobi_aux2link
    );
    // fscanb() -- Base-param, No length param
    suite fscanb_suite = {0};
    create_str_suite(&fscanb_suite, "bigInt_fscanb - String Stream Scan", 
        scan_ecount, rcount, ecases_bprefix, INVERSE, ebuf_slices[1],
        "logs/bigInt_fscan.txt", &scan_ectx, &scan_rcon,
        &scan_rconfig, &scan_rstate
    ); fscanb_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&fscanb_suite,
        &_stobi_scan_ingen_b, &exec_stobi_fscanb,
        &inv_stobi_scan_b, &stat_stobi_fscan_b, 
        &cmp_inv_stobi_scanb, &fmt_in_fscanb, &fmt_recon_stobi,
        &_stobi_scan_inlink, &_stobi_scan_insize,
        &_stobi_recon_linker, &_stobi_recon_size,
        &_stobi_outlink, &_stobi_aux2link
    );

    //* ---------------------------------- STRICT API SUITE ---------------------------------- *//
    // fsscan() -- Base-prefix, No length param
    suite fsscan_suite = {0};
    create_str_suite(&fsscan_suite, "bigInt_fsscan - String Stream Scan", 
        scan_ecount, rcount, ecases_bprefix, INVERSE, ebuf_slices[2],
        "logs/bigInt_fscan.txt", &scan_ectx, &scan_rcon,
        &scan_bp_rconfig, &scan_rstate
    ); fsscan_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&fsscan_suite,
        &_stobi_scan_ingen_nob, &exec_stobi_fsscan,
        &inv_stobi_scan_nob, &stat_stobi_fscan_nobsa,
        &cmp_inv_stobi_scan, &fmt_in_fsscan, &fmt_recon_stobi,
        &_stobi_scan_inlink, &_stobi_scan_insize,
        &_stobi_recon_linker, &_stobi_recon_size,
        &_stobi_outlink, &_stobi_aux2link
    );
    // fsscanb() -- Base-param, No length param
    suite fsscanb_suite = {0};
    create_str_suite(&fsscanb_suite, "bigInt_fsscanb - String Stream Scan", 
        scan_ecount, rcount, ecases_bprefix, INVERSE, ebuf_slices[3],
        "logs/bigInt_fscan.txt", &scan_ectx, &scan_rcon,
        &scan_rconfig, &scan_rstate
    ); fsscanb_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&fsscanb_suite,
        &_stobi_scan_ingen_b, &exec_stobi_fsscanb,
        &inv_stobi_scan_b, &stat_stobi_fscan_bsa,
        &cmp_inv_stobi_scanb, &fmt_in_fsscanb, &fmt_recon_stobi,
        &_stobi_scan_inlink, &_stobi_scan_insize,
        &_stobi_recon_linker, &_stobi_recon_size,
        &_stobi_outlink, &_stobi_aux2link
    );

    //* ---------------------------------- TRUNCATIVE API SUITE ---------------------------------- *//
    // ftscan() -- Base-prefix, No length param
    suite ftscan_suite = {0};
    create_str_suite(&ftscan_suite, "bigInt_ftscan - String Stream Scan", 
        scan_ecount, rcount, ecases_bprefix, INVERSE, ebuf_slices[4],
        "logs/bigInt_fscan.txt", &scan_ectx, &scan_rcon,
        &scan_rconfig, &scan_rstate
    ); ftscan_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&ftscan_suite,
        &_stobi_scan_ingen_nob, &exec_stobi_ftscan,
        &inv_stobi_scan_nob, &stat_stobi_fscan_nobsa,
        &cmp_inv_stobi_scan, &fmt_in_fsscan, &fmt_recon_stobi,
        &_stobi_scan_inlink, &_stobi_scan_insize,
        &_stobi_recon_linker, &_stobi_recon_size,
        &_stobi_outlink, &_stobi_aux2link
    );
    // ftscanb() -- Base-param, No length param
    suite ftscanb_suite = {0};
    create_str_suite(&ftscanb_suite, "bigInt_ftscanb - String Stream Scan", 
        scan_ecount, rcount, ecases_bprefix, INVERSE, ebuf_slices[5],
        "logs/bigInt_fscan.txt", &scan_ectx, &scan_rcon,
        &scan_bp_rconfig, &scan_rstate
    ); ftscanb_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&ftscanb_suite,
        &_stobi_conv_ingen_b, &exec_stobi_ftscanb,
        &inv_stobi_scan_b, &stat_stobi_fscan_bsa,
        &cmp_inv_stobi_scanb, &fmt_in_fsscanb, &fmt_recon_stobi,
        &_stobi_scan_inlink, &_stobi_scan_insize,
        &_stobi_recon_linker, &_stobi_recon_size,
        &_stobi_outlink, &_stobi_aux2link
    );


    //* ---------------------------------- SESSION STARTUP ---------------------------------- *//
    _libdnml_str_suite scan_suite_arr[scan_scount];
    scan_suite_arr[0] = fscan_suite;  scan_suite_arr[1] = fscanb_suite;
    scan_suite_arr[2] = fsscan_suite; scan_suite_arr[3] = fsscanb_suite;
    scan_suite_arr[4] = ftscan_suite; scan_suite_arr[5] = ftscanb_suite;
    _libdnml_session bi_scan_sesh = {0}; create_str_session(
        &bi_scan_sesh, "I/O - String --> BigInt Stream Scan",
        100, scan_scount, scan_suite_arr, scan_omode
    ); start_str_session(&bi_scan_sesh);
    return 0;

    return 0;
}