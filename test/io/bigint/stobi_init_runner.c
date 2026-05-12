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
*/



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
scase ecases_bprefix[27] = {
    /* -------------------------------------------------------------------------------------------- */
    /* Case Number  |   Input                               |   Expected Ouput                      */
    /* ------------------------------ PREPARSE LEXICAL FAILURE CASE ------------------------------- */ 
    { /* 1.         |   ""                              ---->   STR_EMPTY                           */
        .in = &(stobi_init_in){ .str = "", .len = 0, .base = 0 },
        .exp = { .type = BIGINT, .status = STR_EMPTY, .cap = 0, INVAL_BI() }
    }, { /* 2.      |   "  "                            ---->   STR_EMPTY                           */
        .in = &(stobi_init_in){ .str = "  ", .len = 2, .base = 0 },
        .exp = { .type = BIGINT, .status = STR_EMPTY, .cap = 0, INVAL_BI() }
    }, { /* 3.      |   "+"                             ---->   STR_INCOMPLETE                      */
        .in = &(stobi_init_in){ .str = "+", .len = 1, .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { /* 4.      |   "-"                             ---->   STR_INCOMPLETE                      */
        .in = &(stobi_init_in){ .str = "-", .len = 1, .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { /* 5.      |   "-0"                            ---->   STR_INVALID_SIGN                    */
        .in = &(stobi_init_in){ .str = "-0", .len = 2, .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INVALID_SIGN, .cap = 0, INVAL_BI() }
    },
    /* --------------------------------- ON-THE-EDGE SUCCESS CASE --------------------------------- */ 
    { /* 6.         |   "+0"                            ---->   STR_SUCCESS (0)                     */
        .in = &(stobi_init_in){ .str = "+0", .len = 2, .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[4], .n = 0, .cap = 1, .sign = 1 }
        }
    }, { /* 7.      |   "0"                             ---->   STR_SUCCESS (0)                     */
        .in = &(stobi_init_in){ .str = "0", .len = 1, .base = 0 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[4], .n = 0, .cap = 1, .sign = 1 }
        }
    }, { /* 8.      |   "000000"                        ---->   STR_SUCCESS (0)                     */
        .in = &(stobi_init_in){ .str = "000000", .len = 6, .base = 0 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[4], .n = 0, .cap = 1, .sign = 1 }
        }
    }, { /* 9.      |   "   123"                        ---->   STR_SUCCESS (123)                   */
        .in = &(stobi_init_in){ .str = "   123", .len = 6, .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[0], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 10.     |   "18446744073709551616"          ---->   STR_SUCCESS (18446744073709551616)  */
        .in = &(stobi_init_in){ .str = "18446744073709551616", .len = 20, .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0, 
            .data.bi = { .limbs = big_limbs, .n = 2, .cap = 2, .sign = 1 }
        }
    }, { /* 11.     |   "999999...999999" (Len 128)     ---->   STR_SUCCESS (10^128 - 1)            */
        .in = &(stobi_init_in){ .str = 
            "99999999999999999999999999999999"
            "99999999999999999999999999999999"
            "99999999999999999999999999999999"
            "99999999999999999999999999999999",
        .len = 128, .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0, 
            .data.bi = { .limbs = &big_limbs[2], .n = 7, .cap = 7, .sign = 1 }
        }
    },
    /* -------------------------- NUMERICAL-SEGMENT DIGIT-BASED FAILURE --------------------------- */
    { /* 12.        |   "1234abcd"                      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_init_in){ .str = "1234abcd", .len = 8, .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 13.     |   "1234    "                      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_init_in){ .str = "1234    ", .len = 8, .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 14.     |   "abcd1234"                      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_init_in){ .str = "abcd1234", .len = 8, .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 15.     |   "1234_5678"                     ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_init_in){ .str = "1234_5678", .len = 9, .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, 
    /* ----------------------------------- BASE-SPECIFIC FAILURE ---------------------------------- */
    { /* 16.        |   "0x"                            ---->   STR_INCOMPLETE                      */
        .in = &(stobi_init_in){ .str = "0x", .len = 2, .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { /* 17.     |   "0xFG"                          ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_init_in){ .str = "0xFG", .len = 4, .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 18.     |   "0b102"                         ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_init_in){ .str = "0b102", .len = 5, .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 19.     |   "0o89"                          ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_init_in){ .str = "0o89", .len = 4, .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 20.     |   "00x123"                        ---->   STR_INVALID_BASE_PREFIX             */
        .in = &(stobi_init_in){ .str = "00x123", .len = 6, .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INVALID_BASE_PREFIX, .cap = 0, INVAL_BI() }
    }, { /* 21.     |   "0{11)1234A"                    ---->   STR_INVALID_BASE_PREFIX             */
        .in = &(stobi_init_in){ .str = "0{11)1234A", .len = 10, .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INVALID_BASE_PREFIX, .cap = 0, INVAL_BI() }
    },
    /* ---------------------------------- BASE-SPECIFIC SUCCESSES --------------------------------- */
    { /* 22.        |   "0x0"                           ---->   STR_SUCCESS (0)                     */
        .in = &(stobi_init_in){ .str = "0x0", .len = 3, .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[4], .n = 0, .cap = 1, .sign = 1 }
        }
    }, { /* 23.     |   "0xFF"                          ---->   STR_SUCCESS (255)                   */
        .in = &(stobi_init_in){ .str = "0xFF", .len = 4, .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[1], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 24.     |   "0b101010"                      ---->   STR_SUCCESS (42)                    */
        .in = &(stobi_init_in){ .str = "0b101010", .len = 8, .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[2], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 25.     |   "0o77"                          ---->   STR_SUCCESS (63)                    */
        .in = &(stobi_init_in){ .str = "0o77", .len = 4, .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[3], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 26.     |   "0{11}1234A"                    ---->   STR_SUCCESS (17720)                 */
        .in = &(stobi_init_in){ .str = "0{11}1234A", .len = 10, .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[5], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 27.     |   "0{7}123456"                    ---->   STR_SUCCESS (22875)                 */
        .in = &(stobi_init_in){ .str = "0{7}123456", .len = 10, .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[6], .n = 1, .cap = 1, .sign = 1 }
        }
    }, 
    /* -------------------------------------------------------------------------------------------- */
};
scase ecases_base[27] = {
    /* -------------------------------------------------------------------------------------------- */
    /* Case Number  |   Input                               |   Expected Ouput                      */
    /* ------------------------------ PREPARSE LEXICAL FAILURE CASE ------------------------------- */ 
    { /* 1.         |   ""                              ---->   STR_EMPTY                           */
        .in = &(stobi_init_in){ .str = "", .len = 0, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_EMPTY, .cap = 0, INVAL_BI() }
    }, { /* 2.      |   "  "                            ---->   STR_EMPTY                           */
        .in = &(stobi_init_in){ .str = "  ", .len = 2, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_EMPTY, .cap = 0, INVAL_BI() }
    }, { /* 3.      |   "+"                             ---->   STR_INCOMPLETE                      */
        .in = &(stobi_init_in){ .str = "+", .len = 1, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { /* 4.      |   "-"                             ---->   STR_INCOMPLETE                      */
        .in = &(stobi_init_in){ .str = "-", .len = 1, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { /* 5.      |   "-0"                            ---->   STR_INVALID_SIGN                    */
        .in = &(stobi_init_in){ .str = "-0", .len = 2, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INVALID_SIGN, .cap = 0, INVAL_BI() }
    },
    /* --------------------------------- ON-THE-EDGE SUCCESS CASE --------------------------------- */ 
    { /* 6.         |   "+0"                            ---->   STR_SUCCESS (0)                     */
        .in = &(stobi_init_in){ .str = "+0", .len = 2, .base = 10 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[4], .n = 0, .cap = 1, .sign = 1 }
        }
    }, { /* 7.      |   "0"                             ---->   STR_SUCCESS (0)                     */
        .in = &(stobi_init_in){ .str = "0", .len = 1, .base = 10 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[4], .n = 0, .cap = 1, .sign = 1 }
        }
    }, { /* 8.      |   "000000"                        ---->   STR_SUCCESS (0)                     */
        .in = &(stobi_init_in){ .str = "000000", .len = 6, .base = 10 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[4], .n = 0, .cap = 1, .sign = 1 }
        }
    }, { /* 9.      |   "   123"                        ---->   STR_SUCCESS (123)                   */
        .in = &(stobi_init_in){ .str = "   123", .len = 6, .base = 10 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[0], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 10.     |   "18446744073709551616"          ---->   STR_SUCCESS (18446744073709551616)  */
        .in = &(stobi_init_in){ .str = "18446744073709551616", .len = 20, .base = 10 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0, 
            .data.bi = { .limbs = big_limbs, .n = 2, .cap = 2, .sign = 1 }
        }
    }, { /* 11.     |   "999999...999999" (Len 128)     ---->   STR_SUCCESS (10^128 - 1)            */
        .in = &(stobi_init_in){ .str =
            "99999999999999999999999999999999"
            "99999999999999999999999999999999"
            "99999999999999999999999999999999"
            "99999999999999999999999999999999",
        .len = 128, .base = 10 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0, 
            .data.bi = { .limbs = &big_limbs[2], .n = 7, .cap = 7, .sign = 1 }
        }
    },
    /* -------------------------- NUMERICAL-SEGMENT DIGIT-BASED FAILURE --------------------------- */
    { /* 12.        |   "1234abcd"                      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_init_in){ .str = "1234abcd", .len = 8, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 13.     |   "1234    "                      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_init_in){ .str = "1234    ", .len = 8, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 14.     |   "abcd1234"                      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_init_in){ .str = "abcd1234", .len = 8, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 15.     |   "1234_5678"                     ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_init_in){ .str = "1234_5678", .len = 9, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, 
    /* -------------------------------------------------------------------------------------------- */
    /* Case Number  |   Input               |   Base        |   Expected Ouput                      */
    /* ----------------------------------- BASE-SPECIFIC FAILURE ---------------------------------- */
    { /* 16.        |   "-0000"             |   8       ---->   STR_INVALID_SIGN                    */
        .in = &(stobi_init_in){ .str = "-0000", .len = 5, .base = 8 },
        .exp = { .type = BIGINT, .status = STR_INVALID_SIGN, .cap = 0, INVAL_BI() }
    }, { /* 17.     |   "-FGFF"             |   16      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_init_in){ .str = "-FGFF", .len = 5, .base = 16 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 18.     |   "111222~"           |   11      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_init_in){ .str = "111222~", .len = 7, .base = 11 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 19.     |   ".91234"            |   10      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_init_in){ .str = ".91234", .len = 6, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 20.     |   "     101 "         |   2       ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_init_in){ .str = "     101 ", .len = 9, .base = 2 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 21.     |   "1234567"           |   7       ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_init_in){ .str = "1234567", .len = 7, .base = 7 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    },
    /* ---------------------------------- BASE-SPECIFIC SUCCESSES --------------------------------- */
    { /* 22.        |   "000123456789"      |   10      ---->   STR_SUCCESS (123456789)             */
        .in = &(stobi_init_in){ .str = "000123456789", .len = 12, .base = 10 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_valb[0], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 23.     |   "-101010373"        |   8       ---->   STR_SUCCESS (17043707)              */
        .in = &(stobi_init_in){ .str = "-101010373", .len = 10, .base = 8 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0, 
            .data.bi =  { .limbs = &multi_valb[1], .n = 1, .cap = 1, .sign = -1 }
        }
    }, { /* 24.     |   "-ABABABABA"        |   12      ---->   STR_SUCCESS (4726791790)            */
        .in = &(stobi_init_in){ .str = "-ABABABABA", .len = 10, .base = 12 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0, 
            .data.bi = { .limbs = &multi_valb[2], .n = 1, .cap = 1, .sign = -1 }
        }
    }, { /* 25.     |   "      -FFF"        |   16      ---->   STR_SUCCESS (4095)                  */
        .in = &(stobi_init_in){ .str = "      -FFF", .len = 10, .base = 16 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0, 
            .data.bi = { .limbs = &multi_valb[3], .n = 1, .cap = 1, .sign = -1 }
        }
    }, { /* 26.     |   "  0000123654"      |   7       ---->   STR_SUCCESS (22971)                 */
        .in = &(stobi_init_in){ .str = "  0000123654", .len = 12, .base = 7 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_valb[4], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 27.     |   "10001101011110001" |   2       ---->   STR_SUCCESS (72433)                 */
        .in = &(stobi_init_in){ .str = "10001101011110001", .len = 17, .base = 2 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_valb[5], .n = 1, .cap = 1, .sign = 1 }
        }
    }, 
    /* -------------------------------------------------------------------------------------------- */
};


// Main Code
int main(int argc, char **argv) {
    //* ---------------------------------- PRE-TEST SETUP ---------------------------------- *//
    // Parse terminal args + Setup env constants
    u16 rcount = (argc >= 1) ? (u16)(_stou64(argv[1], strlen(argv[1]))) : 100;
    _dnml_output_mode init_omode; if (argc >= 2) {
        u8 sesh_count = _stou64(argv[2], strlen(argv[2]));
        init_omode = (sesh_count <= 3) ? DNML_VOUT : DNML_COUT;
    } else init_omode = DNML_VOUT;
    u8 init_ecount = 27, init_scount = 4;

    // Edge-case Buffer Setup
    limb_t ectx_buf[19]; // Edge-case Memory Usage: 128 bytes
    str_res *ebuf_slices[init_scount], fail_ebuf[(init_ecount << 1) * init_scount];
    strbump_t init_ectx = { .ctx = ectx_buf, .off = 0, .size = 19 };
    _dist_buf(ebuf_slices, fail_ebuf, init_ecount << 1, init_scount, sizeof(str_res));
    // Rand-case Buffer Setup:
    rctx_res_t init_res_rctx = {0}; rctx_input_t init_in_rctx = {0};
    rand_container init_rcon = { 
        .in_cont_type = CTX,
        .in_cont.rctx = &init_in_rctx,
        .res_cont = &init_res_rctx
    };
    // Randomization Configuration
    xoshiro256_state init_rstate = {0}; u64 side_mix = 0;
    __GET_ENTROPY_FAST(init_rstate.s, sizeof(u64) << 2);
    __GET_ENTROPY_FAST(side_mix, sizeof(u64));
    seed_xoshiro256(&init_rstate, side_mix);
    str_rand_mod init_rconfig = {0}, // Base-parameter / Non-base-prefix
    init_bp_rconfig = {0}; // Base-prefix / Non-base-parameter
    strgen_init_sesh(&init_rconfig, false, &init_rstate);
    strgen_init_sesh(&init_bp_rconfig, true, &init_rstate);


    //* ---------------------------------- SUITE SETUP ---------------------------------- *//
    // strinit() -- Base-prefix, No length param
    suite strinit_suite = {0};
    create_str_suite(&strinit_suite, "strinit - String Intialization", 
        init_scount, rcount, ecases_bprefix, INVERSE, ebuf_slices[0], 
        "../logs/bigint_strinit.txt", init_ectx, &init_rcon,
        &init_bp_rconfig, &init_rstate
    ); strinit_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&strinit_suite,
        &_stobi_init_ingen_nob, &exec_stobi_strinit,
        &inv_stobi_init_nob, NULL, &cmp_inv_stobi_init,
        &fmt_in_strinit, &fmt_recon_stobi,
        &_stobi_init_inlink, &_stobi_init_insize,
        &_stobi_recon_linker, &_stobi_recon_size,
        &_stobi_outlink, &_stobi_aux2link
    );
    // strninit() -- Base-prefix, Length param
    suite strninit_suite = {0};
    create_str_suite(&strninit_suite, "strninit - String Intialization", 
        init_scount, rcount, ecases_bprefix, INVERSE, ebuf_slices[0], 
        "../logs/bigint_strinit.txt", init_ectx, &init_rcon,
        &init_bp_rconfig, &init_rstate
    ); strninit_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&strninit_suite,
        &_stobi_init_ingen_nob, &exec_stobi_strninit,
        &inv_stobi_init_nob, NULL, &cmp_inv_stobi_init,
        &fmt_in_strninit, &fmt_recon_stobi,
        &_stobi_init_inlink, &_stobi_init_insize,
        &_stobi_recon_linker, &_stobi_recon_size,
        &_stobi_outlink, &_stobi_aux2link
    );
    // strbinit() -- Base-param, No length param
    suite strbinit_suite = {0};
    create_str_suite(&strbinit_suite, "strbinit - String Intialization", 
        init_scount, rcount, ecases_bprefix, INVERSE, ebuf_slices[0], 
        "../logs/bigint_strinit.txt", init_ectx, &init_rcon,
        &init_rconfig, &init_rstate
    ); strbinit_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&strbinit_suite,
        &_stobi_init_ingen_b, &exec_stobi_strbinit,
        &inv_stobi_init_b, NULL, &cmp_inv_stobi_initb,
        &fmt_in_strbinit, &fmt_recon_stobi,
        &_stobi_init_inlink, &_stobi_init_insize,
        &_stobi_recon_linker, &_stobi_recon_size,
        &_stobi_outlink, &_stobi_aux2link
    );
    // strnbinit() -- Base-param, Length param
    suite strnbinit_suite = {0};
    create_str_suite(&strnbinit_suite, "strnbinit - String Intialization", 
        init_scount, rcount, ecases_bprefix, INVERSE, ebuf_slices[0], 
        "../logs/bigint_strinit.txt", init_ectx, &init_rcon,
        &init_rconfig, &init_rstate
    ); strnbinit_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&strnbinit_suite,
        &_stobi_init_ingen_b, &exec_stobi_strnbinit,
        &inv_stobi_init_b, NULL, &cmp_inv_stobi_initb,
        &fmt_in_strnbinit, &fmt_recon_stobi,
        &_stobi_init_inlink, &_stobi_init_insize,
        &_stobi_recon_linker, &_stobi_recon_size,
        &_stobi_outlink, &_stobi_aux2link
    );


    //* ---------------------------------- SESSION STARTUP ---------------------------------- *//
    _libdnml_str_suite init_suite_arr[init_scount];
    init_suite_arr[0] = strinit_suite;  init_suite_arr[1] = strninit_suite;
    init_suite_arr[2] = strbinit_suite; init_suite_arr[3] = strnbinit_suite;
    _libdnml_session bi_init_sesh = {0}; create_str_session(
        &bi_init_sesh, "I/O - String --> BigInt Initialization",
        100, init_scount, init_suite_arr, init_omode
    ); start_str_session(&bi_init_sesh);
    return 0;
}