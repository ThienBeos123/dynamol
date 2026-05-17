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
limb_t small_mulval[3] = {
    10, // For Case 4
    UINT16_MAX, // For case 5
    UINT64_MAX,  // For case 6
};
limb_t case_8[2] = { 0, UINT32_MAX + 1 }; // 2^96
limb_t case_9[2] = { UINT64_MAX, UINT64_MAX }; // 2^128 - 1
limb_t case_10[8] = { 0, 0, 0, 0, 0, 0, 0, 1 }; // SUPER SUPER SPRASE
limb_t case_11[4] = { // Alternating 1
    UINT64_C(0x5555555555555555), UINT64_C(0x5555555555555555), 
    UINT64_C(0x5555555555555555), UINT64_C(0x5555555555555555) 
};
limb_t case_12[6] = { // Alternating 2
    UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xAAAAAAAAAAAAAAAA),
    UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xAAAAAAAAAAAAAAAA)
};
limb_t case_13[4] = { // Double Alt
    UINT64_C(0x3333333333333333), UINT64_C(0x3333333333333333),
    UINT64_C(0x3333333333333333), UINT64_C(0x3333333333333333)
};
limb_t case_15[8] = { // All MSB
    UINT64_C(0x8000000000000000), UINT64_C(0x8000000000000000),
    UINT64_C(0x8000000000000000), UINT64_C(0x8000000000000000),
    UINT64_C(0x8000000000000000), UINT64_C(0x8000000000000000),
    UINT64_C(0x8000000000000000), UINT64_C(0x8000000000000000),
}; // Largely Dense
limb_t case_16[3] = { UINT64_C(0xEEEEEEEEEEEEEEEE), UINT64_C(0xEEEEEEEEEEEEEEEE), UINT64_C(0xEEEEEEEEEEEEEEEE) };
limb_t case_17[5] = { UINT64_MAX, UINT64_MAX, 0, 0, 1 }; // Dense then Sparse
limb_t case_18[4] = { 0, 0, 0, 1 }; // 2^192
limb_t case_19[4] = { UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX };
limb_t case_20[2] = { UINT64_MAX, UINT64_C(0x7FFFFFFFFFFFFFFF) };
limb_t case_21[16] = {
    UINT64_C(3871883061804059262), UINT64_C(3221185750163728262), UINT64_C(2598294283346557851),
    UINT64_C(12430469004817244324), UINT64_C(12808085984798916544), UINT64_C(13866222929514101260),
    UINT64_C(2887031538269898738), UINT64_C(17890322021139828271), UINT64_C(10533069378811781369),
    UINT64_C(18125661448742495921), UINT64_C(10906755342983384455), UINT64_C(18404811730781439589),
    UINT64_C(18053755223345613669), UINT64_C(6760901800924461203), UINT64_C(5244474428776404420),
    UINT64_C(1644707881994106644)
};
limb_t case_22[8] = { 
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
    UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX
};
limb_t case_23[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 1 };
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



scase ecases_strict[25] = { // 5994 bytes - 6kb ---> Rounded to 6016 bytes
    /* ---------------------------------------------------------------------------------------------------------------------------------------- */
    /* Case Number  | Input                                     | Base      | Capacity  | Needed    | Expected Ouput                            */
    /* ------------------------------------------------------------ TRIVIAL CASES ------------------------------------------------------------- */
    { /* 1          | 0 (n = 0, sign = 1)                       | 10        | 0         | 1         | STR_INVALID_CAP                           */
        .in = &(bitos_conv_in){
            .base = 10, .uppercase = false, .len = 0,
            .x = { .limbs = &zero, .n = 0, .cap = 1, .sign = 1 }
        }, INVAL_STR(STR_INVALID_CAP, 0),
    }, { /* 2       | 1 (n = 1, sign = -1)                      | 10        | 1         | 2         | STR_INVALID_CAP                           */
        .in = &(bitos_conv_in){
            .base = 10, .uppercase = true, .len = 1,
            .x = { .limbs = &one, .n = 1, .cap = 1, .sign = -1 }
        }, INVAL_STR(STR_INVALID_CAP, 1),
    }, { /* 3       | 1 (n = 1, sign = 1)                       | 10        | 10        | 1         | STR_SUCCESS ("1")                         */
        .in = &(bitos_conv_in){
            .base = 10, .uppercase = false, .len = 10,
            .x = { .limbs = &one, .n = 1, .cap = 1, .sign = 1 }
        },
        .exp = { 
            .type = STRING, .status = STR_SUCCESS,
            .cap = 10, .data.len = 1, .pstr = "1"
        },
    }, { /* 4       | -10 (n = 1, sign = -1)                    | 10        | 12        | 3         | STR_SUCCESS ("-10")                       */
        .in = &(bitos_conv_in){
            .base = 10, .uppercase = true, .len = 12,
            .x = { .limbs = &small_mulval[0], .n = 1, .cap = 1, .sign = -1 }
        },
        .exp = { 
            .type = STRING, .status = STR_SUCCESS,
            .cap = 12, .data.len = 3, .pstr = "-10"
        },
    }, { /* 5       | 65535 (n = 1, sign = 1)                   | 16        | 8         | 4         | STR_SUCCESS ("0XFF")                      */
        .in = &(bitos_conv_in){
            .base = 16, .uppercase = true, .len = 8,
            .x = { .limbs = &small_mulval[1], .n = 1, .cap = 1, .sign = 1 }
        },
        .exp = { 
            .type = STRING, .status = STR_SUCCESS,
            .cap = 8, .data.len = 4, .pstr = "0XFF"
        },
    }, { /* 6       | 2^64 - 1 (n = 1, sign = 1)                | 2         | 66        | 66        | STR_SUCCESS ("0b1111111111...1111111111") */
        .in = &(bitos_conv_in){
            .base = 2, .uppercase = false, .len = 66,
            .x = { .limbs = &small_mulval[2], .n = 1, .cap = 1, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 66, .data.len = 66, 
            .pstr = "0b11111111111111111111111111111111"
                    "11111111111111111111111111111111"
        },
    }, { /* 7       | -(2^64 - 1) (n = 1, sign = -1)            | 8         | 20        | 25        | STR_INVALID_CAP                           */
        .in = &(bitos_conv_in){
            .base = 8, .uppercase = false, .len = 20,
            .x = { .limbs = &small_mulval[2], .n = 1, .cap = 1, .sign = -1 }
        }, INVAL_STR(STR_INVALID_CAP, 20),
    },
    /* -------------------------------------------------------------- EDGE CASES -------------------------------------------------------------- */
    { /* 8          | -2^96 (n = 2, sign = -1)                  | 5         | 29        | 46        | STR_INVALID_CAP                           */
        .in = &(bitos_conv_in){
            .base = 5, .uppercase = false, .len = 29,
            .x = { .limbs = case_8, .n = 2, .cap = 2, .sign = -1 }
        }, INVAL_STR(STR_INVALID_CAP, 29),
    }, { /* 9       | 2^128 - 1 (n = 2)                         | 16        | 39        | 34        | STR_SUCCESS (0XFFFFFFFFFFF...FFFFFFFFFFF) */
        .in = &(bitos_conv_in){
            .base = 16, .uppercase = true, .len = 39,
            .x = { .limbs = case_9, .n = 2, .cap = 2, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 39, .data.len = 34, 
            .pstr = "0XFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
        },
    }, { /* 10      | SUPER SUPER SPARSE (n = 8)                | 2         | 451       | 451       | STR_SUCCESS (0B10000000000...00000000000) */
        .in = &(bitos_conv_in){
            .base = 2, .uppercase = true, .len = 451,
            .x = { .limbs = case_10, .n = 8, .cap = 8, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 451, .data.len = 451, 
            .pstr = "0B"
                    "1000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000"
                    "0"
        },
    }, { /* 11      | Alternating 1 (n = 4, sign = -1)          | 8         | 88        | 88        | STR_SUCCESS (-0o5252525252...52525252525) */
        .in = &(bitos_conv_in){
            .base = 8, .uppercase = false, .len = 88,
            .x = { .limbs = case_11, .n = 4, .cap = 4, .sign = -1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 88, .data.len = 88,
            .pstr = "-0o"
                    "52525252525252525252525252525252"
                    "52525252525252525252525252525252"
                    "525252525252525252525"
        },
    }, { /* 12      | Alternating 2 (n = 6)                     | 2         | 386       | 386       | STR_SUCCESS (0B10101010101...01010101010) */
        .in = &(bitos_conv_in){
            .base = 2, .uppercase = true, .len = 386,
            .x = { .limbs = case_12, .n = 6, .cap = 6, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 386, .data.len = 386,
            .pstr = "0B"
                    "1010101010101010101010101010101010101010101010101010101010101010"
                    "1010101010101010101010101010101010101010101010101010101010101010"
                    "1010101010101010101010101010101010101010101010101010101010101010"
                    "1010101010101010101010101010101010101010101010101010101010101010"
                    "1010101010101010101010101010101010101010101010101010101010101010"
                    "1010101010101010101010101010101010101010101010101010101010101010"
        },
    }, { /* 13      | Double Alternating (n = 4, sign = -1)     | 2         | 257       | 257       | STR_SUCCESS (-0b1100110011...01100110011) */
        .in = &(bitos_conv_in){
            .base = 2, .uppercase = false, .len = 257,
            .x = { .limbs = case_13, .n = 4, .cap = 4, .sign = -1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 257, .data.len = 257,
            .pstr = "-0b"
                    "1100110011001100110011001100110011001100110011001100110011001100"
                    "1100110011001100110011001100110011001100110011001100110011001100"
                    "1100110011001100110011001100110011001100110011001100110011001100"
                    "11001100110011001100110011001100110011001100110011001100110011"
        },
    }, { /* 14      | MSB All Limbs (n = 8)                     | 64        | 88        | 88        | STR_SUCCESS (0,20000000000...80000000000) */ //! CASE-SENSITIVE
        .in = &(bitos_conv_in){
            .base = 64, .uppercase = false, .len = 88,
            .x = { .limbs = case_12, .n = 8, .cap = 8, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 88, .data.len = 88,
            .pstr = "0,"
                    "2000000000080000000000W000000000"
                    "2000000000080000000000W000000000"
                    "2000000000080000000000" //! CASE-SENSITIVE
        },
    }, { /* 15      | SUPER SUPER SPARSE (n = 8) - fail edition | 13        | 120       | 127       | STR_INVALID_CAP                           */
         .in = &(bitos_conv_in){
            .base = 13, .uppercase = false, .len = 120,
            .x = { .limbs = case_10, .n = 8, .cap = 8, .sign = 1 }
        }, INVAL_STR(STR_INVALID_CAP, 120),
    }, { /* 16      | NEARLY DENSE (n = 3) - fail edition       | 6         | 50        | 79        | STR_INVALID_CAP                           */
         .in = &(bitos_conv_in){
            .base = 6, .uppercase = true, .len = 50,
            .x = { .limbs = case_16, .n = 3, .cap = 3, .sign = 1 }
        }, INVAL_STR(STR_INVALID_CAP, 50),
    }, { /* 17      | Dense -> Sparse (n = 5)                   | 22        | 40        | 63        | STR_INVALID_CAP                           */
         .in = &(bitos_conv_in){
            .base = 22, .uppercase = false, .len = 40,
            .x = { .limbs = case_17, .n = 5, .cap = 5, .sign = 1 }
        }, INVAL_STR(STR_INVALID_CAP, 40),
    }, { /* 18      | 2^192 (n = 4)                             | 36        | 25        | 43        | STR_INVALID_CAP                           */
         .in = &(bitos_conv_in){
            .base = 36, .uppercase = false, .len = 25,
            .x = { .limbs = case_18, .n = 4, .cap = 4, .sign = 1 }
        }, INVAL_STR(STR_INVALID_CAP, 25),
    }, { /* 19      | 2^256 - 1 (n = 4)                         | 4         | 77        | 132       | STR_INVALID_CAP                           */
         .in = &(bitos_conv_in){
            .base = 4, .uppercase = false, .len = 77,
            .x = { .limbs = case_19, .n = 4, .cap = 4, .sign = 1 }
        }, INVAL_STR(STR_INVALID_CAP, 77),
    }, { /* 20      | 2^127 - 1 (n = 2) (Mersenne prime)        | 2         | 129       | 129       | STR_SUCCESS (0b11111111111...11111111111) */
         .in = &(bitos_conv_in){
            .base = 2, .uppercase = false, .len = 129,
            .x = { .limbs = case_20, .n = 2, .cap = 2, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 129, .data.len = 129,
            .pstr = "0b"
                    "11111111111111111111111111111111"
                    "11111111111111111111111111111111"
                    "11111111111111111111111111111111"
                    "1111111111111111111111111111111"
        },
    }, { /* 21      | Absolutely Random (n = 16)                | 11        | 301       | 301       | STR_SUCCESS (0{11}235A75279...0090954A36) */
         .in = &(bitos_conv_in){
            .base = 11, .uppercase = true, .len = 301,
            .x = { .limbs = case_22, .n = 8, .cap = 8, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 301, .data.len = 301,
            .pstr = "0{11}"
                    "235A752799046623806113A0A4516A584156A00523343334109A965514374269"
                    "5945259992A9A836A460102035A924689894A4A5094359718175A43328708866"
                    "18409482812A87444A3674293A319250452339A1A07A31964A556277824A6286"
                    "952309A505683034053324953A07489A7727865835321160083493A93501AA98"
                    "2862A867A77A67406656483660A3460090954A36"
        },
    }, { /* 22      | 2^512 - 1 (n = 8)                         | 32        | 108       | 108       | STR_SUCCESS (0{32}3VVVVVVVV...VVVVVVVVVV) */ //! CASE-SENSITIVE
         .in = &(bitos_conv_in){
            .base = 32, .uppercase = false, .len = 108,
            .x = { .limbs = case_22, .n = 8, .cap = 8, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 108, .data.len = 108,
            .pstr = "0{32}"
                    "3VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV"
                    "VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV"
                    "VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV"
                    "VVVVVVV"
        },
    }, { /* 23      | -2^512 (n = 9, sign = -1)                 | 32        | 103       | 109       | STR_INVALID_CAP                           */
         .in = &(bitos_conv_in){
            .base = 32, .uppercase = false, .len = 103,
            .x = { .limbs = case_23, .n = 9, .cap = 9, .sign = -1 }
        }, INVAL_STR(STR_INVALID_CAP, 103),
    }, { /* 24      | -(2^3072 - 1) (n = 48, sign = -1)         | 2         | 3072      | 3075      | STR_INVALID_CAP                           */
         .in = &(bitos_conv_in){
            .base = 2, .uppercase = true, .len = 3072,
            .x = { .limbs = case_final, .n = 48, .cap = 48, .sign = -1 }
        }, INVAL_STR(STR_INVALID_CAP, 3072),
    }, { /* 25      | 2^3072 - 1 (n = 48)                       | 64        | 514       | 514       | STR_SUCCESS (0,///////////...///////////) */
        .in = &(bitos_conv_in){
            .base = 64, .uppercase = false, .len = 514,
            .x = { .limbs = case_final, .n = 48, .cap = 48, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 514, .data.len = 514,
            .pstr = "0,"
                    "////////////////////////////////////////////////////////////////"
                    "////////////////////////////////////////////////////////////////"
                    "////////////////////////////////////////////////////////////////"
                    "////////////////////////////////////////////////////////////////"
                    "////////////////////////////////////////////////////////////////"
                    "////////////////////////////////////////////////////////////////"
                    "////////////////////////////////////////////////////////////////"
                    "////////////////////////////////////////////////////////////////"
        },
    }, 
    /* ---------------------------------------------------------------------------------------------------------------------------------------- */
};
scase ecases_trunc[25] = { // 5994 bytes - 6kb ---> Rounded to 6016 bytes
    /* ---------------------------------------------------------------------------------------------------------------------------------------- */
    /* Case Number  | Input                                     | Base      | Capacity  | Needed    | Expected Ouput                            */
    /* ------------------------------------------------------------ TRIVIAL CASES ------------------------------------------------------------- */
    { /* 1          | 0 (n = 0, sign = 1)                       | 10        | 0         | 1         | STR_INVALID_CAP                           */
        .in = &(bitos_conv_in){
            .base = 10, .uppercase = false, .len = 0,
            .x = { .limbs = &zero, .n = 0, .cap = 1, .sign = 1 }
        }, INVAL_STR(STR_INVALID_CAP, 0),
    }, { /* 2       | 1 (n = 1, sign = -1)                      | 16        | 3         | 4         | STR_INVALID_CAP                           */
        .in = &(bitos_conv_in){
            .base = 16, .uppercase = true, .len = 3,
            .x = { .limbs = &one, .n = 1, .cap = 1, .sign = -1 }
        }, INVAL_STR(STR_INVALID_CAP, 3),
    }, { /* 3       | 1 (n = 1, sign = 1)                       | 10        | 10        | 1         | STR_SUCCESS ("1")                         */
        .in = &(bitos_conv_in){
            .base = 10, .uppercase = false, .len = 10,
            .x = { .limbs = &one, .n = 1, .cap = 1, .sign = 1 }
        },
        .exp = { 
            .type = STRING, .status = STR_SUCCESS,
            .cap = 10, .data.len = 1, .pstr = "1"
        },
    }, { /* 4       | -10 (n = 1, sign = -1)                    | 10        | 12        | 3         | STR_SUCCESS ("-10")                       */
        .in = &(bitos_conv_in){
            .base = 10, .uppercase = true, .len = 12,
            .x = { .limbs = &small_mulval[0], .n = 1, .cap = 1, .sign = -1 }
        },
        .exp = { 
            .type = STRING, .status = STR_SUCCESS,
            .cap = 12, .data.len = 3, .pstr = "-10"
        },
    }, { /* 5       | 65535 (n = 1, sign = 1)                   | 16        | 8         | 4         | STR_SUCCESS ("0XFF")                      */
        .in = &(bitos_conv_in){
            .base = 16, .uppercase = true, .len = 8,
            .x = { .limbs = &small_mulval[1], .n = 1, .cap = 1, .sign = 1 }
        },
        .exp = { 
            .type = STRING, .status = STR_SUCCESS,
            .cap = 8, .data.len = 4, .pstr = "0XFF"
        },
    }, { /* 6       | 2^64 - 1 (n = 1, sign = 1)                | 2         | 66        | 66        | STR_SUCCESS ("0b1111111111...1111111111") */
        .in = &(bitos_conv_in){
            .base = 2, .uppercase = false, .len = 66,
            .x = { .limbs = &small_mulval[2], .n = 1, .cap = 1, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 66, .data.len = 66, 
            .pstr = "0b"
                    "11111111111111111111111111111111"
                    "11111111111111111111111111111111"
        },
    }, { /* 7       | -(2^64 - 1) (n = 1, sign = -1)            | 8         | 20        | 25        | STR_TRUNC_SUCCESS                         */
        .in = &(bitos_conv_in){
            .base = 8, .uppercase = false, .len = 20,
            .x = { .limbs = &small_mulval[2], .n = 1, .cap = 1, .sign = -1 }
        },
        .exp = {
            .type = STRING, .status = STR_TRUNC_SUCCESS,
            .cap = 20, .data.len = 20, .pstr = "-0o77777777777777777"
        }
    },
    /* -------------------------------------------------------------- EDGE CASES -------------------------------------------------------------- */
    { /* 8          | -2^96 (n = 2, sign = -1)                  | 5         | 29        | 47        | STR_TRUNC_SUCCESS                         */
        .in = &(bitos_conv_in){
            .base = 5, .uppercase = false, .len = 29,
            .x = { .limbs = case_8, .n = 2, .cap = 2, .sign = -1 }
        },
        .exp = {
            .type = STRING, .status = STR_TRUNC_SUCCESS,
            .cap = 29, .data.len = 29, .pstr = "-0{5}424220224113034222402321"
        }
    }, { /* 9       | 2^128 - 1 (n = 2)                         | 16        | 39        | 34        | STR_SUCCESS (0XFFFFFFFFFFF...FFFFFFFFFFF) */
        .in = &(bitos_conv_in){
            .base = 16, .uppercase = true, .len = 39,
            .x = { .limbs = case_9, .n = 2, .cap = 2, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 39, .data.len = 34, 
            .pstr = "0XFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
        },
    }, { /* 10      | SUPER SUPER SPARSE (n = 8)                | 2         | 451       | 451       | STR_SUCCESS (0B10000000000...00000000000) */
        .in = &(bitos_conv_in){
            .base = 2, .uppercase = true, .len = 451,
            .x = { .limbs = case_10, .n = 8, .cap = 8, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 451, .data.len = 451, 
            .pstr = "0B"
                    "1000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000"
                    "0"
        },
    }, { /* 11      | Alternating 1 (n = 4, sign = -1)          | 8         | 88        | 88        | STR_SUCCESS (-0o5252525252...52525252525) */
        .in = &(bitos_conv_in){
            .base = 8, .uppercase = false, .len = 88,
            .x = { .limbs = case_11, .n = 4, .cap = 4, .sign = -1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 88, .data.len = 88,
            .pstr = "-0o"
                    "52525252525252525252525252525252"
                    "52525252525252525252525252525252"
                    "525252525252525252525"
        },
    }, { /* 12      | Alternating 2 (n = 6)                     | 2         | 386       | 386       | STR_SUCCESS (0B10101010101...01010101010) */
        .in = &(bitos_conv_in){
            .base = 2, .uppercase = true, .len = 386,
            .x = { .limbs = case_12, .n = 6, .cap = 6, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 386, .data.len = 386,
            .pstr = "0B"
                    "1010101010101010101010101010101010101010101010101010101010101010"
                    "1010101010101010101010101010101010101010101010101010101010101010"
                    "1010101010101010101010101010101010101010101010101010101010101010"
                    "1010101010101010101010101010101010101010101010101010101010101010"
                    "1010101010101010101010101010101010101010101010101010101010101010"
                    "1010101010101010101010101010101010101010101010101010101010101010"
        },
    }, { /* 13      | Double Alternating (n = 4, sign = -1)     | 2         | 257       | 257       | STR_SUCCESS (-0b1100110011...01100110011) */
        .in = &(bitos_conv_in){
            .base = 2, .uppercase = false, .len = 257,
            .x = { .limbs = case_13, .n = 4, .cap = 4, .sign = -1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 257, .data.len = 257,
            .pstr = "-0b"
                    "1100110011001100110011001100110011001100110011001100110011001100"
                    "1100110011001100110011001100110011001100110011001100110011001100"
                    "1100110011001100110011001100110011001100110011001100110011001100"
                    "11001100110011001100110011001100110011001100110011001100110011"
        },
    }, { /* 14      | MSB All Limbs (n = 8)                     | 64        | 88        | 88        | STR_SUCCESS (0,20000000000...80000000000) */ //! CASE-SENSITIVE
        .in = &(bitos_conv_in){
            .base = 64, .uppercase = false, .len = 88,
            .x = { .limbs = case_12, .n = 8, .cap = 8, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 88, .data.len = 88,
            .pstr = "0,"
                    "2000000000080000000000W000000000"
                    "2000000000080000000000W000000000"
                    "2000000000080000000000" //! CASE-SENSITIVE
        },
    }, { /* 15      | SUPER SUPER SPARSE (n = 8) - fail edition | 13        | 120       | 127       | STR_TRUNC_SUCCESS                         */
         .in = &(bitos_conv_in){
            .base = 13, .uppercase = false, .len = 120,
            .x = { .limbs = case_10, .n = 8, .cap = 8, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_TRUNC_SUCCESS, .cap = 120, .data.len = 120,
            .pstr = "0{13}"
                    "556b3872176b35b8a7780589529c6c37"
                    "2c93cb36295920466548584154629626"
                    "c6a0922041667b39b64341994740b641"
                    "2308a484b8912991893"
        }
    }, { /* 16      | NEARLY DENSE (n = 3) - fail edition       | 6         | 50        | 79        | STR_TRUNC_SUCCESS                         */
         .in = &(bitos_conv_in){
            .base = 6, .uppercase = true, .len = 50,
            .x = { .limbs = case_16, .n = 3, .cap = 3, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_TRUNC_SUCCESS, .cap = 50, .data.len = 50,
            .pstr = "0{6}0515404523521300451551534135200150220112420210"
        }
    }, { /* 17      | Dense -> Sparse (n = 5)                   | 22        | 40        | 63        | STR_TRUNC_SUCCESS                         */ //! CASE-SENSITIVE
         .in = &(bitos_conv_in){
            .base = 22, .uppercase = false, .len = 40,
            .x = { .limbs = case_17, .n = 5, .cap = 5, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_TRUNC_SUCCESS, .cap = 40, .data.len = 40,
            .pstr = "0{22}LHC6FG5FBLCLEIJ14F33536EAFJ37K359CB" //! CASE-SENSITIVE
        }
    }, { /* 18      | 2^192 (n = 4)                             | 36        | 25        | 43        | STR_TRUNC_SUCCESS                         */ //! CASE-SENSITIVE
         .in = &(bitos_conv_in){
            .base = 36, .uppercase = false, .len = 25,
            .x = { .limbs = case_18, .n = 4, .cap = 4, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_TRUNC_SUCCESS,
            .cap = 25, .data.len = 25, .pstr = "0{36}AZ14IVFB2OWPROUGRCHS" //! CSE-SENSITIVE
        }
    }, { /* 19      | 2^256 - 1 (n = 4)                         | 4         | 77        | 132       | STR_TRUNC_SUCCESS                         */
         .in = &(bitos_conv_in){
            .base = 4, .uppercase = false, .len = 77,
            .x = { .limbs = case_19, .n = 4, .cap = 4, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_TRUNC_SUCCESS, .cap = 77, .data.len = 77,
            .pstr = "0{4}3333333333333333333333333333333333333333333333333333333333333333333333333"
        }
    }, { /* 20      | 2^127 - 1 (n = 2) (Mersenne prime)        | 2         | 129       | 129       | STR_SUCCESS (0b11111111111...11111111111) */
         .in = &(bitos_conv_in){
            .base = 2, .uppercase = false, .len = 129,
            .x = { .limbs = case_20, .n = 2, .cap = 2, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 129, .data.len = 129,
            .pstr = "0b"
                    "11111111111111111111111111111111"
                    "11111111111111111111111111111111"
                    "11111111111111111111111111111111"
                    "1111111111111111111111111111111"
        },
    }, { /* 21      | Absolutely Random (n = 16)                | 11        | 301       | 301       | STR_SUCCESS (0{11}235A7527...60090954A36) */
         .in = &(bitos_conv_in){
            .base = 11, .uppercase = true, .len = 301,
            .x = { .limbs = case_22, .n = 8, .cap = 8, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 301, .data.len = 301,
            .pstr = "0{11}"
                    "235A752799046623806113A0A4516A584156A00523343334109A965514374269"
                    "5945259992A9A836A460102035A924689894A4A5094359718175A43328708866"
                    "18409482812A87444A3674293A319250452339A1A07A31964A556277824A6286"
                    "952309A505683034053324953A07489A7727865835321160083493A93501AA98"
                    "2862A867A77A67406656483660A3460090954A36"
        },
    }, { /* 22      | 2^512 - 1 (n = 8)                         | 32        | 108       | 108       | STR_SUCCESS (0{32}3VVVVVVV...VVVVVVVVVVV) */ //! CASE-SENSITIVE
         .in = &(bitos_conv_in){
            .base = 32, .uppercase = false, .len = 108,
            .x = { .limbs = case_22, .n = 8, .cap = 8, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 108, .data.len = 108,
            .pstr = "0{32}"
                    "3VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV"
                    "VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV"
                    "VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV"
                    "VVVVVVV"
        },
    }, { /* 23      | -2^512 (n = 9, sign = -1)                 | 32        | 103       | 109       | STR_TRUNC_SUCCESS                         */ //! CASE-SENSITIVE
        .in = &(bitos_conv_in){
            .base = 32, .uppercase = false, .len = 103,
            .x = { .limbs = case_23, .n = 9, .cap = 9, .sign = -1 }
        },
        .exp = {
            .type = STRING, .status = STR_TRUNC_SUCCESS, .cap = 103, .data.len = 103,
            .pstr = "-0{32}000000000000000000000000000"
                    "00000000000000000000000000000000"
                    "00000000000000000000000000000000"
                    "000000" //! CASE-SENSITIVE
        }
    }, { /* 24      | -(2^3072 - 1) (n = 48, sign = -1)         | 2         | 3072      | 3075      | STR_TRUNC_SUCCESS                         */
        .in = &(bitos_conv_in){
            .base = 2, .uppercase = true, .len = 3072,
            .x = { .limbs = case_final, .n = 48, .cap = 48, .sign = -1 }
        },
        .exp = {
            .type = STRING, .status = STR_TRUNC_SUCCESS, .cap = 3072, .data.len = 3072,
            .pstr = "-0B1111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111111111111111111"
        }
    }, { /* 25      | 2^3072 - 1 (n = 48)                       | 64        | 514       | 514       | STR_SUCCESS (0,///////////...///////////) */
        .in = &(bitos_conv_in){
            .base = 64, .uppercase = false, .len = 514,
            .x = { .limbs = case_final, .n = 48, .cap = 48, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 514, .data.len = 514,
            .pstr = "0,"
                    "////////////////////////////////////////////////////////////////"
                    "////////////////////////////////////////////////////////////////"
                    "////////////////////////////////////////////////////////////////"
                    "////////////////////////////////////////////////////////////////"
                    "////////////////////////////////////////////////////////////////"
                    "////////////////////////////////////////////////////////////////"
                    "////////////////////////////////////////////////////////////////"
                    "////////////////////////////////////////////////////////////////"
        },
    }, 
    /* ---------------------------------------------------------------------------------------------------------------------------------------- */
};


// Main Code
int main(int argc, char **argv) {
    //* ---------------------------------- PRE-TEST SETUP ---------------------------------- *//
    // Parse terminal args + Setup env constants
    u16 rcount = (argc >= 1) ? (u16)(_stou64(argv[1], strlen(argv[1]))) : 100;
    _dnml_output_mode conv_omode; if (argc >= 2) {
        u8 sesh_count = _stou64(argv[2], strlen(argv[2]));
        conv_omode = (sesh_count <= 3) ? DNML_VOUT : DNML_COUT;
    } else conv_omode = DNML_VOUT;
    u8 conv_ecount = 25, conv_scount = 2;

    // Edge-case Buffer Setup
    char ectx_buf[6016]; // Edge-case Memory Usage: 6016 bytes
    str_res *ebuf_slices[conv_scount], fail_ebuf[(conv_ecount << 1) * conv_scount];
    strbump_t conv_ectx = { .ctx = ectx_buf, .off = 0, .size = 6016 };
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
    bi_rand_mod conv_rconfig = {0}; // Base-prefix
    bigen_init_sesh(&conv_rconfig, &conv_rstate);


    //* ------------------------------------ SUITE SETUP ------------------------------------ *//
    // to_strf() - Size-aware Testing
    suite to_sstrf_suite = {0};
    create_str_suite(&to_sstrf_suite, "to_strf - BigInt Conversion", 
        conv_scount, rcount, ecases_strict, INVERSE, ebuf_slices[2], 
        "../logs/bi_logs/bigint_to_strf.txt", &conv_ectx, &conv_rcon,
        &conv_rconfig, &conv_rstate
    ); to_sstrf_suite.cap_mode = RANDOMIZED;
    fill_suite_rinv(&to_sstrf_suite,
        &_bitos_convf_ingen, &exec_bitos_to_strf,
        &inv_bitos_conv_b, &stat_bitos_conv_b,
        &cmp_inv_bitos_conv, &fmt_in_to_strf, &fmt_recon_bitos,
        &_bitos_conv_inlink, &_bitos_conv_insize,
        &_bitos_recon_linker, &_bitos_recon_size,
        &_bitos_outlink, &_bitos_aux2link
    );
    // tto_strf() - Size-aware Testing
    suite tto_sstrf_suite = {0};
    create_str_suite(&tto_sstrf_suite, "tto_strf - BigInt Conversion",
        conv_scount, rcount, ecases_trunc, INVERSE, ebuf_slices[3],
        "../logs/bi_logs/bigint_to_strf.txt", &conv_ectx, &conv_rcon,
        &conv_rconfig, &conv_rstate
    ); tto_sstrf_suite.cap_mode = RANDOMIZED;
    fill_suite_reval(&tto_sstrf_suite,
        &_bitos_convf_ingen, &exec_bitos_tto_strf, &eval_bitos_tto_strf,
        &stat_bitos_tconv_b, &cmp_eval_bitos, &fmt_in_to_strf,
        &_bitos_conv_inlink, &_bitos_conv_insize,
        &_bitos_outlink, &_bitos_aux2link
    );


    //* ---------------------------------- SESSION STARTUP ---------------------------------- *//
    _libdnml_str_suite conv_suite_arr[conv_scount];
    conv_suite_arr[0] = to_sstrf_suite; conv_suite_arr[1] = tto_sstrf_suite;
    _libdnml_session bi_conv_sesh = {0}; create_str_session(
        &bi_conv_sesh, "I/O - BigInt --> String Conversion (Formatted)",
        100, conv_scount, conv_suite_arr, conv_omode
    ); start_str_session(&bi_conv_sesh);
    return 0;
}