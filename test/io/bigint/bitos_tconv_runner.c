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


scase ecases_nob[25] = { // 2636 bytes of memory usage ---> ROunded to 2688 bytes
    /* ------------------------------------------------------------------------------------------------------------ */
    /* Case Number  | Input                                     | Expected Ouput                                    */
    /* ---------------------------------------------- TRIVIAL CASES ----------------------------------------------- */
    { /* 1          | 0 (n = 0)                                 | "0"                                               */
        .in = &(bitos_conv_in){ 
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &zero, .n = 0, .cap = 1, .sign = 1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .data.len = 1, .cap = 1, .pstr = "0" }
    }, { /* 2       | 1 (n = 1)                                 | "1"                                               */
        .in = &(bitos_conv_in){ 
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &one, .n = 1, .cap = 1, .sign = 1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .data.len = 1, .cap = 1, .pstr = "1" }
    }, { /* 3       | -1 (n = 1, sign = -1)                     | "-1"                                              */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &one, .n = 1, .cap = 1, .sign = -1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .data.len = 2, .cap = 2, .pstr = "-1" }
    }, { /* 4       | 255 (n = 1)                               | "255"                                             */
        .in = &(bitos_conv_in){ 
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &small_mulval[0], .n = 1, .cap = 1, .sign = 1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .data.len = 3, .cap = 3, .pstr = "255" }
    }, { /* 5       | -999 (n = 1, sign = -1)                   | "-999"                                            */
        .in = &(bitos_conv_in){ 
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &small_mulval[1], .n = 1, .cap = 1, .sign = -1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .data.len = 4, .cap = 4, .pstr = "-999" }
    }, { /* 6       | 2^64 - 1 (n = 1)                          | "18446744073709551615"                            */
        .in = &(bitos_conv_in){ 
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &small_mulval[2], .n = 1, .cap = 1, .sign = 1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .data.len = 20, .cap = 20, .pstr = "18446744073709551615" }
    }, { /* 7       | -(2^64 - 1) (n = 1, sign = -1)            | "-18446744073709551615"                           */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &small_mulval[2], .n = 1, .cap = 1, .sign = -1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .data.len = 21, .cap = 21, .pstr = "-18446744073709551615" }
    },
    /* ----------------------------------------------- EDGE CASES ------------------------------------------------- */
    { /* 8          | 2^64 (n = 2)                              | "18446744073709551616"                            */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_8, .cap = 2, .n = 2, .sign = 1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .cap = 20, .data.len = 20, .pstr = "18446744073709551616" }
    }, { /* 9       | 2^64 + 1 (n = 2)                          | "18446744073709551617"                            */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_9, .cap = 2, .n = 2, .sign = 1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .cap = 20, .data.len = 20, .pstr = "18446744073709551617" }
    }, { /* 10      | 2^128 - 1 (n = 2)                         | "340282366920938463463374607431768211455"         */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_10, .cap = 2, .n = 2, .sign = 1 }
        },
        .exp = { 
            .type = STRING, .status = STR_SUCCESS, .cap = 39, .data.len = 39, 
            .pstr = "340282366920938463463374607431768211455" 
        }
    }, { /* 11      | -(2^128 - 1) (n = 2)                      | "-340282366920938463463374607431768211455"        */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_10, .cap = 2, .n = 2, .sign = -1 }
        },
        .exp = { 
            .type = STRING, .status = STR_SUCCESS, .cap = 40, .data.len = 40, 
            .pstr = "-340282366920938463463374607431768211455" 
        }
    }, { /* 12      | idk Random ig (n = 3)                     | "679052356442...456469093871" (truncated)         */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_12, .cap = 3, .n = 3, .sign = 1 }
        },
        .exp = { 
            .type = STRING, .status = STR_SUCCESS, .cap = 39, .data.len = 39, 
            .pstr = "679052356442327393940567539456469093871" 
        }
    }, { /* 13      | 1000000000 (n = 1) - Power of 10          | "1000000000"                                      */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &small_mulval[3], .cap = 1, .n = 1, .sign = 1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .cap = 10, .data.len = 10,  .pstr = "1000000000" }
    }, { /* 14      | -1000000000 (n = 1, sign = -1)            | "-1000000000"                                     */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &small_mulval[3], .cap = 1, .n = 1, .sign = -1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .cap = 11, .data.len = 11,  .pstr = "-1000000000" }
    }, { /* 15      | Alt Limbs (n = 2)                         | "226854911280...263275623765" (truncated)         */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_15, .cap = 2, .n = 2, .sign = 1 }
        },
        .exp = { 
            .type = STRING, .status = STR_SUCCESS, .cap = 39, .data.len = 39,
            .pstr = "226854911280625642302767490263275623765"
        }
    }, { /* 16      | MSB Limbs (n = 2)                         | "170141183460...752738881536" (truncated)         */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_16, .cap = 2, .n = 2, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 39, .data.len = 39,
            .pstr = "170141183460469231740910675752738881536"
        }
    }, { /* 17      | 2^96 (n = 2)                              | "79228162514264337593543950336"                   */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_17, .cap = 2, .n = 2, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 29, .data.len = 29,
            .pstr = "79228162514264337593543950336"
        }
    }, { /* 18      | 2^192 (n = 4)                             | "-627710173538...464034512896" (truncated)        */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_18, .cap = 4, .n = 4, .sign = -1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 59, .data.len = 59,
            .pstr = "-6277101735386680763835789423207666416102355444464034512896"
        }
    }, { /* 19      | idk (n = 5) - LARGELY SPARSE              | "115792089237...344897851391" (truncated)         */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_19, .cap = 5, .n = 5, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 78, .data.len = 78,
            .pstr = "11579208923731619542357098500868"
                    "79078536102670325615025029209586"
                    "15344897851391"
        }
    }, { /* 20      | [0, 1, 2, 3, 4] (n = 5, sign = -1)        | "-463168356949...981868072960" (truncated)        */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_20, .cap = 5, .n = 5, .sign = -1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 79, .data.len = 79,
            .pstr = "-46316835694926478171311524524091"
                    "16737052678716660271323330825983"
                    "23981868072960"
        }
    }, { /* 21      | 2^256 - 1 (n = 5, sign = -1)              | "-115792089237...913129639935" (truncated)        */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_21, .cap = 4, .n = 4, .sign = -1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 79, .data.len = 79,
            .pstr = "-11579208923731619542357098500868"
                    "79078532699846656405640394575840"
                    "07913129639935"
        }
    }, { /* 22      | 2^127 - 1 (n = 2) - MERSENNE PRIME        | "170141183460...715884105727" (truncated)         */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_22, .cap = 2, .n = 2, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 39, .data.len = 39,
            .pstr = "170141183460469231731687303715884105727"
        }
    }, { /* 23      | Absolutely random (n = 6)                 | "744153080953...881801009584" (truncated)         */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_23, .cap = 6, .n = 6, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 113, .data.len = 113,
            .pstr = "74415308095378740388572238211385"
                    "16023944234383361231936207324957"
                    "66344647588606305592985742043724"
                    "78387881801009584"
        }
    }, { /* 24      | 2^3072 - 1 (n = 48)                       | "580960599536...567329693695" (truncated)         */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_final, .cap = 48, .n = 48, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 925, .data.len = 925,
            .pstr = "5809605995369958062859502533304574370686975176362895236661486152"
                    "2872037309971102257373360445331184072513261577549805174439905295"
                    "9454004712166288567218703240103211163970644049884404985098905162"
                    "7200244765807041812394729680540024104827976584369381522292361208"
                    "7790447698927432257517380769795688113095791255113330932435195537"
                    "8481630638158016186020024749256844815024251530444957718760413642"
                    "8738580990172551573934146255830366405915000869643732053218566832"
                    "5452911079037228316341385995864066903259597251874471690595408050"
                    "1231020963901175074876001709536073423494575741627299485601330861"
                    "6958529958304677637019181594088528345061285863898271763457294883"
                    "5466388795543116154464463301992543823400162920570907511755338881"
                    "6191898729559153153669870129226768546551743791579082315484463478"
                    "0260102891718032495396075041899485513811126977307478969074857043"
                    "7107161501213159220245567592412390131529197109564684063794429149"
                    "41614357107914462567329693695"
        }
    }, { /* 25      | -(2^3072 - 1) (n = 48, sign = -1          | "-580960599536...567329693695" (truncated)        */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_final, .cap = 48, .n = 48, .sign = -1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 926, .data.len = 926,
            .pstr = "-5809605995369958062859502533304574370686975176362895236661486152"
                    "2872037309971102257373360445331184072513261577549805174439905295"
                    "9454004712166288567218703240103211163970644049884404985098905162"
                    "7200244765807041812394729680540024104827976584369381522292361208"
                    "7790447698927432257517380769795688113095791255113330932435195537"
                    "8481630638158016186020024749256844815024251530444957718760413642"
                    "8738580990172551573934146255830366405915000869643732053218566832"
                    "5452911079037228316341385995864066903259597251874471690595408050"
                    "1231020963901175074876001709536073423494575741627299485601330861"
                    "6958529958304677637019181594088528345061285863898271763457294883"
                    "5466388795543116154464463301992543823400162920570907511755338881"
                    "6191898729559153153669870129226768546551743791579082315484463478"
                    "0260102891718032495396075041899485513811126977307478969074857043"
                    "7107161501213159220245567592412390131529197109564684063794429149"
                    "41614357107914462567329693695"
        }
    },
    /* ------------------------------------------------------------------------------------------------------------ */
};
scase ecases_b[25] = { // 2215 bytes of memory usage + 65 bytes = 2280 bytes
    /* -------------------------------------------------------------------------------------------------------------------- */
    /* Case Number  | Input                             | Base          | Expected Ouput                                    */
    /* -------------------------------------------------- TRIVIAL CASES --------------------------------------------------- */
    { /* 1          | 0 (n = 0)                         | 2             | "0"                                               */
        .in = &(bitos_conv_in){ 
            .base = 2, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &zero, .n = 0, .cap = 1, .sign = 1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .data.len = 1, .cap = 1, .pstr = "0" }
    }, { /* 2       | 1 (n = 1)                         | 8             | "1"                                               */
        .in = &(bitos_conv_in){ 
            .base = 8, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &one, .n = 1, .cap = 1, .sign = 1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .data.len = 1, .cap = 1, .pstr = "1" }
    }, { /* 3       | -1 (n = 1, sign = -1)             | 16            | "-1"                                              */
        .in = &(bitos_conv_in){
            .base = 16, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &one, .n = 1, .cap = 1, .sign = -1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .data.len = 2, .cap = 2, .pstr = "-1" }
    }, { /* 4       | 255 (n = 1)                       | 10            | "255"                                             */
        .in = &(bitos_conv_in){ 
            .base = 10, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &small_mulval[0], .n = 1, .cap = 1, .sign = 1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .data.len = 3, .cap = 3, .pstr = "255" }
    }, { /* 5       | -999 (n = 1, sign = -1)           | 10            | "-999"                                            */
        .in = &(bitos_conv_in){ 
            .base = 10, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &small_mulval[1], .n = 1, .cap = 1, .sign = -1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .data.len = 4, .cap = 4, .pstr = "-999" }
    }, { /* 6       | 2^64 - 1 (n = 1)                  | 16            | "FFFFFFFFFFFFFFFF"                                */
        .in = &(bitos_conv_in){ 
            .base = 16, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &small_mulval[2], .n = 1, .cap = 1, .sign = 1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .data.len = 16, .cap = 16, .pstr = "FFFFFFFFFFFFFFFF" }
    }, { /* 7       | -(2^64 - 1) (n = 1, sign = -1)    | 16            | "-FFFFFFFFFFFFFFFF"                               */
        .in = &(bitos_conv_in){
            .base = 16, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &small_mulval[2], .n = 1, .cap = 1, .sign = -1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .data.len = 17, .cap = 17, .pstr = "-FFFFFFFFFFFFFFFF" }
    },
    /* --------------------------------------------------- EDGE CASES ----------------------------------------------------- */
    { /* 8          | 2^64 (n = 2)                      | 2             | "100000000000...000000000000" (truncated)         */
        .in = &(bitos_conv_in){
            .base = 2, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_8, .cap = 2, .n = 2, .sign = 1 }
        },
        .exp = { 
            .type = STRING, .status = STR_SUCCESS, .cap = 65, .data.len = 65, 
            .pstr = "10000000000000000000000000000000000000000000000000000000000000000" 
        }
    }, { /* 9       | 2^64 + 1 (n = 2)                  | 8             | "2000000000000000000001"                          */
        .in = &(bitos_conv_in){
            .base = 8, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_9, .cap = 2, .n = 2, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 22, .data.len = 22,
            .pstr = "2000000000000000000001"
        }
    }, { /* 10      | 2^128 - 1 (n = 2)                 | 7             | "311551216212...356026315303" (truncated)         */
            .in = &(bitos_conv_in){
            .base = 7, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_10, .cap = 2, .n = 2, .sign = 1 }
        },
        .exp = { 
            .type = STRING, .status = STR_SUCCESS, .cap = 46, .data.len = 46, 
            .pstr = "3115512162124626343001006330151620356026315303" 
        }
    }, { /* 11      | -(2^128 - 1) (n = 2)              | 16            | "-FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"               */
        .in = &(bitos_conv_in){
            .base = 16, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_10, .cap = 2, .n = 2, .sign = -1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 33, .data.len = 33,
            .pstr = "-FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF" 
        }
    }, { /* 12      | idk Random ig (n = 3)             | 3             | "111210010100...202201200001" (truncated)         */
        .in = &(bitos_conv_in){
            .base = 3, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_12, .cap = 3, .n = 3, .sign = 1 }
        },
        .exp = { 
            .type = STRING, .status = STR_SUCCESS, .cap = 82, .data.len = 82,
            .pstr = "11121001010011210211100011210110"
                    "20000002010100200221200112200222"
                    "020221202201200001" 
        }
    }, { /* 13      | 1000000000 (n = 1) - Power of 10  | 10            | "1000000000"                                      */
        .in = &(bitos_conv_in){
            .base = 10, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &small_mulval[3], .cap = 1, .n = 1, .sign = 1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .cap = 10, .data.len = 10,  .pstr = "1000000000" }
    }, { /* 14      | -1000000000 (n = 1, sign = -1)    | 10            | "-1000000000"                                     */
        .in = &(bitos_conv_in){
            .base = 10, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = &small_mulval[3], .cap = 1, .n = 1, .sign = -1 }
        },
        .exp = { .type = STRING, .status = STR_SUCCESS, .cap = 11, .data.len = 11,  .pstr = "-1000000000" }
    }, { /* 15      | Alt Limbs (n = 2)                 | 16            | "AAAAAAAAAAAAAAAA5555555555555555"                */
        .in = &(bitos_conv_in){
            .base = 16, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_15, .cap = 2, .n = 2, .sign = 1 }
        },
        .exp = { 
            .type = STRING, .status = STR_SUCCESS, .cap = 32, .data.len = 32,
            .pstr = "AAAAAAAAAAAAAAAA5555555555555555"
        }
    }, { /* 16      | MSB Limbs (n = 2)                 | 2             | "100000000000...000000000000" (truncated)         */
        .in = &(bitos_conv_in){
            .base = 2, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_16, .cap = 2, .n = 2, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 128, .data.len = 128,
            .pstr = "10000000000000000000000000000000"
                    "00000000000000000000000000000000"
                    "10000000000000000000000000000000"
                    "00000000000000000000000000000000"
        }
    }, { /* 17      | 2^96 (n = 2)                      | 8             | "100000000000000000000000000000000"               */
        .in = &(bitos_conv_in){
            .base = 8, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_17, .cap = 2, .n = 2, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 33, .data.len = 33,
            .pstr = "100000000000000000000000000000000"
        }
    }, { /* 18      | 2^192 (n = 4)                     | 32            | "-400000000000000000000000000000000000000"        */
        .in = &(bitos_conv_in){
            .base = 32, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_18, .cap = 4, .n = 4, .sign = -1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 40, .data.len = 40,
            .pstr = "-400000000000000000000000000000000000000"
        }
    }, { /* 19      | idk (n = 5) - LARGELY SPARSE      | 18            | "31G025HE8916...8224E90HA311" (truncated)         */
        .in = &(bitos_conv_in){
            .base = 18, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_19, .cap = 5, .n = 5, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 62, .data.len = 62,
            .pstr = "31G025HE891652FC25EED6DG159AHB8HD9DC46856F404H5GAG8224E90HA311"
        }
    }, { /* 20      | [0, 1, 2, 3, 4] (n = 5, sign = -1)| 16            | "-400000000000...000000000000" (truncated)        */
        .in = &(bitos_conv_in){
            .base = 16, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_20, .cap = 5, .n = 5, .sign = -1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 66, .data.len = 66,
            .pstr = "-4000000000000000300000000000000"
                    "02000000000000000100000000000000"
                    "00"
        }
    }, { /* 21      | 2^256 - 1 (n = 5, sign = -1)      | 16            | "-ffffffffffff...ffffffffffff" (truncated)        */
        .in = &(bitos_conv_in){
            .base = 16, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_21, .cap = 4, .n = 4, .sign = -1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 65, .data.len = 65,
            .pstr = "ffffffffffffffffffffffffffffffff"
                    "ffffffffffffffffffffffffffffffff"
        }
    }, { /* 22      | 2^127 - 1 (n = 2) - MERSENNE PRIME| 2             | "111111111111...111111111111" (truncated)         */
        .in = &(bitos_conv_in){
            .base = 2, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_22, .cap = 2, .n = 2, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 127, .data.len = 127,
            .pstr = "11111111111111111111111111111111"
                    "11111111111111111111111111111111"
                    "11111111111111111111111111111111"
                    "1111111111111111111111111111111"
        }
    }, { /* 23      | Absolutely random (n = 6)         | 48            | "1XBCG0M8HWlH...j6URTfakGMJW" (truncated)         */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_23, .cap = 6, .n = 6, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 68, .data.len = 68,
            .pstr = "1XBCG0M8HWlH2Z41QAPQFHAIcILZBLgR"
                    "M7EA6c7Sab58AMlgSUj37diZj6URTfak"
                    "GMJW"
        }
    }, { /* 24      | 2^3072 - 1 (n = 48)               | 16            | "ffffffffffff...ffffffffffff" (truncated)         */
        .in = &(bitos_conv_in){
            .base = 16, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_final, .cap = 48, .n = 48, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 768, .data.len = 768,
            .pstr = "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
                    "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
                    "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
                    "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
                    "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
                    "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
                    "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
                    "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
                    "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
                    "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
                    "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
                    "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
        }
    }, { /* 25      | -(2^3072 - 1) (n = 48, sign = -1) | 64            | "-////////////...////////////" (truncated)        */
        .in = &(bitos_conv_in){
            .base = 64, .uppercase = false, .len = STR_OUT_CAP,
            .x = { .limbs = case_final, .cap = 48, .n = 48, .sign = -1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 513, .data.len = 513,
            .pstr = "-////////////////////////////////////////////////////////////////"
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
    _dnml_output_mode conv_omode; if (argc >= 2) {
        u8 sesh_count = _stou64(argv[2], strlen(argv[2]));
        conv_omode = (sesh_count <= 3) ? DNML_VOUT : DNML_COUT;
    } else conv_omode = DNML_VOUT;
    u8 conv_ecount = 25, conv_scount = 4;

    // Edge-case Buffer Setup
    char ectx_buf[2688]; // Edge-case Memory Usage: 2688 bytes
    str_res *ebuf_slices[conv_scount], fail_ebuf[(conv_ecount << 1) * conv_scount];
    strbump_t conv_ectx = { .ctx = ectx_buf, .off = 0, .size = 2688 };
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
    bi_rand_mod conv_rconfig = {0}; // Non-base-prefix
    bigen_init_sesh(&conv_rconfig, &conv_rstate);


    //* ------------------------------------ SUITE SETUP ------------------------------------ *//
    // tto_str() - Non-base-parameter, No length param
    suite tto_str_suite = {0};
    create_str_suite(&tto_str_suite, "tto_str - BigInt Conversion", 
        conv_scount, rcount, ecases_nob, INVERSE, ebuf_slices[0], 
        "../logs/bi_logs/bigint_tto_str.txt", &conv_ectx, &conv_rcon,
        &conv_rconfig, &conv_rstate
    ); tto_str_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&tto_str_suite,
        &_bitos_conv_ingen, &exec_bitos_tto_str,
        &inv_bitos_conv_nob, &stat_bitos_tconv_nob, 
        &cmp_inv_bitos_conv, &fmt_in_to_str, &fmt_recon_bitos,
        &_bitos_conv_inlink, &_bitos_conv_insize,
        &_bitos_recon_linker, &_bitos_recon_size,
        &_bitos_outlink, &_bitos_aux2link
    );
    // tto_strb() - Base-parameter, No length param
    suite tto_strb_suite = {0};
    create_str_suite(&tto_strb_suite, "tto_strb - BigInt Conversion", 
        conv_scount, rcount, ecases_b, INVERSE, ebuf_slices[0], 
        "../logs/bi_logs/bigint_tto_str.txt", &conv_ectx, &conv_rcon,
        &conv_rconfig, &conv_rstate
    ); tto_strb_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&tto_strb_suite,
        &_bitos_conv_ingen, &exec_bitos_tto_strb,
        &inv_bitos_conv_b, &stat_bitos_tconv_b, 
        &cmp_inv_bitos_conv, &fmt_in_to_strb, &fmt_recon_bitos,
        &_bitos_conv_inlink, &_bitos_conv_insize,
        &_bitos_recon_linker, &_bitos_recon_size,
        &_bitos_outlink, &_bitos_aux2link
    );
    // tto_strn() - Non-base-parameter, length param
    suite tto_strn_suite = {0};
    create_str_suite(&tto_strn_suite, "tto_strn - BigInt Conversion", 
        conv_scount, rcount, ecases_nob, INVERSE, ebuf_slices[0], 
        "../logs/bi_logs/bigint_tto_str.txt", &conv_ectx, &conv_rcon,
        &conv_rconfig, &conv_rstate
    ); tto_strn_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&tto_strn_suite,
        &_bitos_conv_ingen, &exec_bitos_tto_strn,
        &inv_bitos_conv_nob, &stat_bitos_tconv_nob,
        &cmp_inv_bitos_conv, &fmt_in_to_strn, &fmt_recon_bitos,
        &_bitos_conv_inlink, &_bitos_conv_insize,
        &_bitos_recon_linker, &_bitos_recon_size,
        &_bitos_outlink, &_bitos_aux2link
    );
    // tto_strnb() - Base-parameter, length param
    suite tto_strnb_suite = {0};
    create_str_suite(&tto_strnb_suite, "tto_strnb - BigInt Conversion",
        conv_scount, rcount, ecases_b, INVERSE, ebuf_slices[0],
        "../logs/bi_logs/bigint_tto_str.txt", &conv_ectx, &conv_rcon,
        &conv_rconfig, &conv_rstate
    ); tto_strnb_suite.cap_mode = ENOUGH;
    fill_suite_rinv(&tto_strnb_suite,
        &_bitos_conv_ingen, &exec_bitos_tto_strnb,
        &inv_bitos_conv_b, &stat_bitos_tconv_b,
        &cmp_inv_bitos_conv, &fmt_in_to_strnb, &fmt_recon_bitos,
        &_bitos_conv_inlink, &_bitos_conv_insize,
        &_bitos_recon_linker, &_bitos_recon_size,
        &_bitos_outlink, &_bitos_aux2link
    );


    //* ---------------------------------- SESSION STARTUP ---------------------------------- *//
    _libdnml_str_suite tconv_suite_arr[conv_scount];
    tconv_suite_arr[0] = tto_str_suite;  tconv_suite_arr[1] = tto_strb_suite;
    tconv_suite_arr[2] = tto_strn_suite; tconv_suite_arr[3] = tto_strnb_suite;
    _libdnml_session bi_conv_sesh = {0}; create_str_session(
        &bi_conv_sesh, "I/O - BigInt --> String Conversion",
        100, conv_scount, tconv_suite_arr, conv_omode
    ); start_str_session(&bi_conv_sesh);
    return 0;
}