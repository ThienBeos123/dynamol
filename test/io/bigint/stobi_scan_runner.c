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

// THESE HAS TO BE INITIATED IN main()
FILE *scan_in_nob, *scan_in_b;

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
scase ecases_bprefix[27] = {
    /* -------------------------------------------------------------------------------------------- */
    /* Case Number  |   Input                               |   Expected Ouput                      */
    /* ------------------------------ PREPARSE LEXICAL FAILURE CASE ------------------------------- */ 
    { /* 1.         |   ""                              ---->   STR_EMPTY                           */
        .in = &(stobi_scan_in){ .base = 0 },
        .exp = { .type = BIGINT, .status = STR_EMPTY, .cap = 0, INVAL_BI() }
    }, { /* 2.      |   "  "                            ---->   STR_EMPTY                           */
        .in = &(stobi_scan_in){ .base = 0 },
        .exp = { .type = BIGINT, .status = STR_EMPTY, .cap = 0, INVAL_BI() }
    }, { /* 3.      |   "+"                             ---->   STR_INCOMPLETE                      */
        .in = &(stobi_scan_in){ .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { /* 4.      |   "-"                             ---->   STR_INCOMPLETE                      */
        .in = &(stobi_scan_in){ .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { /* 5.      |   "-0"                            ---->   STR_INVALID_SIGN                    */
        .in = &(stobi_scan_in){ .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INVALID_SIGN, .cap = 0, INVAL_BI() }
    },
    /* --------------------------------- ON-THE-EDGE SUCCESS CASE --------------------------------- */ 
    { /* 6.         |   "+0"                            ---->   STR_SUCCESS (0)                     */
        .in = &(stobi_scan_in){ .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[4], .n = 0, .cap = 1, .sign = 1 }
        }
    }, { /* 7.      |   "0"                             ---->   STR_SUCCESS (0)                     */
        .in = &(stobi_scan_in){ .base = 0 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[4], .n = 0, .cap = 1, .sign = 1 }
        }
    }, { /* 8.      |   "000000"                        ---->   STR_SUCCESS (0)                     */
        .in = &(stobi_scan_in){ .base = 0 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[4], .n = 0, .cap = 1, .sign = 1 }
        }
    }, { /* 9.      |   "   123"                        ---->   STR_SUCCESS (123)                   */
        .in = &(stobi_scan_in){ .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[0], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 10.     |   "18446744073709551616"          ---->   STR_SUCCESS (18446744073709551616)  */
        .in = &(stobi_scan_in){ .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0, 
            .data.bi = { .limbs = big_limbs, .n = 2, .cap = 2, .sign = 1 }
        }
    }, { /* 11.     |   "999999...999999" (Len 128)     ---->   STR_SUCCESS (10^128 - 1)            */
        .in = &(stobi_scan_in){ .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0, 
            .data.bi = { .limbs = &big_limbs[2], .n = 7, .cap = 7, .sign = 1 }
        }
    },
    /* -------------------------- NUMERICAL-SEGMENT DIGIT-BASED FAILURE --------------------------- */
    { /* 12.        |   "1234abcd"                      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 13.     |   "1234    "                      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 14.     |   "abcd1234"                      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 15.     |   "1234_5678"                     ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, 
    /* ----------------------------------- BASE-SPECIFIC FAILURE ---------------------------------- */
    { /* 16.        |   "0x"                            ---->   STR_INCOMPLETE                      */
        .in = &(stobi_scan_in){ .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { /* 17.     |   "0xFG"                          ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 18.     |   "0b102"                         ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 19.     |   "0o89"                          ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 20.     |   "00x123"                        ---->   STR_INVALID_BASE_PREFIX             */
        .in = &(stobi_scan_in){ .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INVALID_BASE_PREFIX, .cap = 0, INVAL_BI() }
    }, { /* 21.     |   "0{11)1234A"                    ---->   STR_INVALID_BASE_PREFIX             */
        .in = &(stobi_scan_in){ .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INVALID_BASE_PREFIX, .cap = 0, INVAL_BI() }
    },
    /* ---------------------------------- BASE-SPECIFIC SUCCESSES --------------------------------- */
    { /* 22.        |   "0x0"                           ---->   STR_SUCCESS (0)                     */
        .in = &(stobi_scan_in){ .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[4], .n = 0, .cap = 1, .sign = 1 }
        }
    }, { /* 23.     |   "0xFF"                          ---->   STR_SUCCESS (255)                   */
        .in = &(stobi_scan_in){ .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[1], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 24.     |   "0b101010"                      ---->   STR_SUCCESS (42)                    */
        .in = &(stobi_scan_in){ .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[2], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 25.     |   "0o77"                          ---->   STR_SUCCESS (63)                    */
        .in = &(stobi_scan_in){ .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[3], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 26.     |   "0{11}1234A"                    ---->   STR_SUCCESS (17720)                 */
        .in = &(stobi_scan_in){ .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[5], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 27.     |   "0{7}123456"                    ---->   STR_SUCCESS (22875)                 */
        .in = &(stobi_scan_in){ .base = 0 },
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
        .in = &(stobi_scan_in){ .base = 10 },
        .exp = { .type = BIGINT, .status = STR_EMPTY, .cap = 0, INVAL_BI() }
    }, { /* 2.      |   "  "                            ---->   STR_EMPTY                           */
        .in = &(stobi_scan_in){ .base = 10 },
        .exp = { .type = BIGINT, .status = STR_EMPTY, .cap = 0, INVAL_BI() }
    }, { /* 3.      |   "+"                             ---->   STR_INCOMPLETE                      */
        .in = &(stobi_scan_in){ .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { /* 4.      |   "-"                             ---->   STR_INCOMPLETE                      */
        .in = &(stobi_scan_in){ .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { /* 5.      |   "-0"                            ---->   STR_INVALID_SIGN                    */
        .in = &(stobi_scan_in){ .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INVALID_SIGN, .cap = 0, INVAL_BI() }
    },
    /* --------------------------------- ON-THE-EDGE SUCCESS CASE --------------------------------- */ 
    { /* 6.         |   "+0"                            ---->   STR_SUCCESS (0)                     */
        .in = &(stobi_scan_in){ .base = 10 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[4], .n = 0, .cap = 1, .sign = 1 }
        }
    }, { /* 7.      |   "0"                             ---->   STR_SUCCESS (0)                     */
        .in = &(stobi_scan_in){ .base = 10 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[4], .n = 0, .cap = 1, .sign = 1 }
        }
    }, { /* 8.      |   "000000"                        ---->   STR_SUCCESS (0)                     */
        .in = &(stobi_scan_in){ .base = 10 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[4], .n = 0, .cap = 1, .sign = 1 }
        }
    }, { /* 9.      |   "   123"                        ---->   STR_SUCCESS (123)                   */
        .in = &(stobi_scan_in){ .base = 10 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[0], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 10.     |   "18446744073709551616"          ---->   STR_SUCCESS (18446744073709551616)  */
        .in = &(stobi_scan_in){ .base = 10 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0, 
            .data.bi = { .limbs = big_limbs, .n = 2, .cap = 2, .sign = 1 }
        }
    }, { /* 11.     |   "999999...999999" (Len 128)     ---->   STR_SUCCESS (10^128 - 1)            */
        .in = &(stobi_scan_in){ .base = 10 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0, 
            .data.bi = { .limbs = &big_limbs[2], .n = 7, .cap = 7, .sign = 1 }
        }
    },
    /* -------------------------- NUMERICAL-SEGMENT DIGIT-BASED FAILURE --------------------------- */
    { /* 12.        |   "1234abcd"                      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 13.     |   "1234    "                      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 14.     |   "abcd1234"                      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 15.     |   "1234_5678"                     ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, 
    /* -------------------------------------------------------------------------------------------- */
    /* Case Number  |   Input               |   Base        |   Expected Ouput                      */
    /* ----------------------------------- BASE-SPECIFIC FAILURE ---------------------------------- */
    { /* 16.        |   "-0000"             |   8       ---->   STR_INVALID_SIGN                    */
        .in = &(stobi_scan_in){ .base = 8 },
        .exp = { .type = BIGINT, .status = STR_INVALID_SIGN, .cap = 0, INVAL_BI() }
    }, { /* 17.     |   "-FGFF"             |   16      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 16 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 18.     |   "111222~"           |   11      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 11 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 19.     |   ".91234"            |   10      ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 20.     |   "     101 "         |   2       ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 2 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { /* 21.     |   "1234567"           |   7       ---->   STR_INVALID_DIGIT                   */
        .in = &(stobi_scan_in){ .base = 7 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    },
    /* ---------------------------------- BASE-SPECIFIC SUCCESSES --------------------------------- */
    { /* 22.        |   "000123456789"      |   10      ---->   STR_SUCCESS (123456789)             */
        .in = &(stobi_scan_in){ .base = 10 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_valb[0], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 23.     |   "-101010373"        |   8       ---->   STR_SUCCESS (17043707)              */
        .in = &(stobi_scan_in){ .base = 8 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0, 
            .data.bi =  { .limbs = &multi_valb[1], .n = 1, .cap = 1, .sign = -1 }
        }
    }, { /* 24.     |   "-ABABABABA"        |   12      ---->   STR_SUCCESS (4726791790)            */
        .in = &(stobi_scan_in){ .base = 12 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0, 
            .data.bi = { .limbs = &multi_valb[2], .n = 1, .cap = 1, .sign = -1 }
        }
    }, { /* 25.     |   "      -FFF"        |   16      ---->   STR_SUCCESS (4095)                  */
        .in = &(stobi_scan_in){ .base = 16 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0, 
            .data.bi = { .limbs = &multi_valb[3], .n = 1, .cap = 1, .sign = -1 }
        }
    }, { /* 26.     |   "  0000123654"      |   7       ---->   STR_SUCCESS (22971)                 */
        .in = &(stobi_scan_in){ .base = 7 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_valb[4], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { /* 27.     |   "10001101011110001" |   2       ---->   STR_SUCCESS (72433)                 */
        .in = &(stobi_scan_in){ .base = 2 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_valb[5], .n = 1, .cap = 1, .sign = 1 }
        }
    }, 
    /* -------------------------------------------------------------------------------------------- */
};


void setup_cases(FILE *nob, FILE *b, u8 ecount) {
    for (u8 i = 0; i < ecount; i++) {
        stobi_scan_in* nob_case = (stobi_scan_in*)ecases_bprefix[i].in;
        stobi_scan_in* b_case = (stobi_scan_in*)ecases_base[i].in;
        nob_case->stream = nob; b_case->stream = b;
    }
}


int main(int argc, char **argv) {
    //* ---------------------------------- PRE-TEST SETUP ---------------------------------- *//
    // Parse terminal args + Setup env constants
    u16 rcount = (argc >= 1) ? (u16)(_stou64(argv[1], strlen(argv[1]))) : 100;
    _dnml_output_mode init_omode; if (argc >= 2) {
        u8 sesh_count = _stou64(argv[2], strlen(argv[2]));
        init_omode = (sesh_count <= 3) ? DNML_VOUT : DNML_COUT;
    } else init_omode = DNML_VOUT;
    u8 scan_ecount = 27, scan_scount = 4;
    // Buffer and Test Cases Setup
    scan_in_nob = fopen("input_files/bi_scan_nob.txt", "r");
    scan_in_b = fopen("input_files/bi_scan_b.txt", "r");
    setup_cases(scan_in_nob, scan_in_b, scan_ecount);
    limb_t ectx_buf[19]; // Edge-case Memory Usage: 128 bytes
    rctx_t init_rctx = {0}; str_res *ebuf_slices[scan_scount];
    str_res fail_ebuf[(scan_ecount << 1) * scan_scount];
    strbump_t init_ectx = { .ctx = ectx_buf, .off = 0, .size = 19 };
    _dist_buf(ebuf_slices, fail_ebuf, scan_ecount << 1, scan_scount, sizeof(str_res));


    //* ---------------------------------- SUITE SETUP ---------------------------------- *//
    


    //* --------------------------------- SESSION SETUP --------------------------------- *//

    return 0;
}