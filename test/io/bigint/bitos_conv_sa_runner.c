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

scase ecases_nob[25] = { // Total: 3556 bytes - 3.556 KB
    /* ---------------------------------------------------------------------------------------------------------------------------- */
    /* Case Number  | Input                                     | Capacity  | Needed    | Expected Ouput                            */
    /* ------------------------------------------------------ TRIVIAL CASES ------------------------------------------------------- */
    { /* 1          | 0 (n = 0, sign = 1)                       | 0         | 1         | STR_INVALID_CAP                           */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 0,
            .x = { .limbs = &zero, .n = 0, .cap = 1, .sign = 1 }
        }, INVAL_STR(STR_INVALID_CAP, 0),
    }, { /* 2       | 1 (n = 1, sign = -1)                      | 1         | 2         | STR_INVALID_CAP                           */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 1,
            .x = { .limbs = &one, .n = 1, .cap = 1, .sign = -1 }
        }, INVAL_STR(STR_INVALID_CAP, 1),
    }, { /* 3       | 1 (n = 1, sign = 1)                       | 10        | 1         | STR_SUCCESS ("1")                         */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 10,
            .x = { .limbs = &one, .n = 1, .cap = 1, .sign = 1 }
        },
        .exp = { 
            .type = STRING, .status = STR_SUCCESS,
            .cap = 10, .data.len = 1, .pstr = "1"
        },
    }, { /* 4       | -10 (n = 1, sign = -1)                    | 12        | 3         | STR_SUCCESS ("-10")                       */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 12,
            .x = { .limbs = &small_mulval[0], .n = 1, .cap = 1, .sign = -1 }
        },
        .exp = { 
            .type = STRING, .status = STR_SUCCESS,
            .cap = 12, .data.len = 3, .pstr = "-10"
        },
    }, { /* 5       | 65535 (n = 1, sign = 1)                   | 8         | 5         | STR_SUCCESS ("65535")                     */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 8,
            .x = { .limbs = &small_mulval[1], .n = 1, .cap = 1, .sign = 1 }
        },
        .exp = { 
            .type = STRING, .status = STR_SUCCESS,
            .cap = 8, .data.len = 5, .pstr = "65535"
        },
    }, { /* 6       | 2^64 - 1 (n = 1, sign = 1)                | 24        | 20        | STR_SUCCESS ("18446744073709551615")      */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 24,
            .x = { .limbs = &small_mulval[2], .n = 1, .cap = 1, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS,
            .cap = 24, .data.len = 20, .pstr = "18446744073709551615"
        },
    }, { /* 7       | -(2^64 - 1) (n = 1, sign = -1)            | 20        | 21        | STR_INVALID_CAP                           */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 20,
            .x = { .limbs = &small_mulval[2], .n = 1, .cap = 1, .sign = -1 }
        }, INVAL_STR(STR_INVALID_CAP, 20),
    },
    /* ------------------------------------------------------- EDGE CASES --------------------------------------------------------- */
    { /* 8          | -2^96 (n = 2, sign = -1)                  | 29        | 30        | STR_INVALID_CAP                           */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 29,
            .x = { .limbs = case_8, .n = 2, .cap = 2, .sign = -1 }
        }, INVAL_STR(STR_INVALID_CAP, 29),
    }, { /* 9       | 2^128 - 1 (n = 2)                         | 39        | 39        | STR_SUCCESS (340282366920...431768211455) */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 39,
            .x = { .limbs = case_9, .n = 2, .cap = 2, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 39, .data.len = 39, 
            .pstr = "340282366920938463463374607431768211455"
        },
    }, { /* 10      | SUPER SUPER SPARSE (n = 8)                | 136       | 135       | STR_SUCCESS (726838724295...601628614656) */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 136,
            .x = { .limbs = case_10, .n = 8, .cap = 8, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 136, .data.len = 135, 
            .pstr = "7268387242956068905493238078880045343536413606873180602814901991"
                    "8063928811339792332619105071376356556076252160626617793353460162"
                    "8614656"
        },
    }, { /* 11      | Alternating 1 (n = 4, sign = -1)          | 78        | 78        | STR_SUCCESS (-38597363079...304376546645) */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 78,
            .x = { .limbs = case_11, .n = 4, .cap = 4, .sign = -1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 78, .data.len = 78,
            .pstr = "-3859736307910539847452366166956"
                    "26359510899948885468546798191946"
                    "69304376546645"
        },
    }, { /* 12      | Alternating 2 (n = 6)                     | 116       | 116       | STR_SUCCESS (262680041309...085326871210) */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 116,
            .x = { .limbs = case_12, .n = 6, .cap = 6, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 116, .data.len = 116,
            .pstr = "26268004130929652808186026733429"
                    "07587005315951364363111196552893"
                    "61638145143314737409428441699232"
                    "77093871085326871210"
        },
    }, { /* 13      | Double Alternating (n = 4, sign = -1)     | 80        | 78        | STR_SUCCESS (-23158417847...582625927987) */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 80,
            .x = { .limbs = case_13, .n = 4, .cap = 4, .sign = -1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 80, .data.len = 78,
            .pstr = "-23158417847463239084714197001737"
                    "58157065399693312811280789151680"
                    "1582625927987"
        },
    }, { /* 14      | MSB All Limbs (n = 8)                     | 154       | 154       | STR_SUCCESS (670390396497...861676929024) */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 154,
            .x = { .limbs = case_12, .n = 8, .cap = 8, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 154, .data.len = 154,
            .pstr = "6703903964971298550150431861250726509034045817338396195645808915"
                    "7725733071213138645241929506291410985147846790151556573451043297"
                    "83898672915800861676929024"
        },
    }, { /* 15      | SUPER SUPER SPARSE (n = 8) - fail edition | 130       | 135       | STR_INVALID_CAP                           */
         .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 130,
            .x = { .limbs = case_10, .n = 8, .cap = 8, .sign = 1 }
        }, INVAL_STR(STR_INVALID_CAP, 130),
    }, { /* 16      | NEARLY DENSE (n = 3) - fail edition       | 50        | 58        | STR_INVALID_CAP                           */
         .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 50,
            .x = { .limbs = case_16, .n = 3, .cap = 3, .sign = 1 }
        }, INVAL_STR(STR_INVALID_CAP, 50),
    }, { /* 17      | Dense -> Sparse (n = 5)                   | 60        | 78        | STR_INVALID_CAP                           */
         .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 60,
            .x = { .limbs = case_17, .n = 5, .cap = 5, .sign = 1 }
        }, INVAL_STR(STR_INVALID_CAP, 60),
    }, { /* 18      | 2^192 (n = 4)                             | 25        | 58        | STR_INVALID_CAP                           */
         .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 25,
            .x = { .limbs = case_18, .n = 4, .cap = 4, .sign = 1 }
        }, INVAL_STR(STR_INVALID_CAP, 25),
    }, { /* 19      | 2^256 - 1 (n = 4)                         | 77        | 78        | STR_INVALID_CAP                           */
         .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 77,
            .x = { .limbs = case_19, .n = 4, .cap = 4, .sign = 1 }
        }, INVAL_STR(STR_INVALID_CAP, 77),
    }, { /* 20      | 2^127 - 1 (n = 2) (Mersenne prime)        | 39        | 39        | STR_SUCCESS (170141183460...715884105727) */
         .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 39,
            .x = { .limbs = case_20, .n = 2, .cap = 2, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 39, .data.len = 39,
            .pstr = "170141183460469231731687303715884105727"
        },
    }, { /* 21      | Absolutely Random (n = 16)                | 308       | 308       | STR_SUCCESS (377327162527...736060049172) */
         .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 308,
            .x = { .limbs = case_22, .n = 8, .cap = 8, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 308, .data.len = 308,
            .pstr = "3773271625270142732830211682460292569886734255053243787387684224"
                    "5696572007955460249795484407291123875524183519257142994462331543"
                    "4261041757729495550270979431957656696110752280288379361628488551"
                    "9136534112743193512974542951732330061975550497185819627676947919"
                    "0505537006296187554591489261632207679795736060049172"
        },
    }, { /* 22      | 2^512 - 1 (n = 8)                         | 155       | 155       | STR_SUCCESS (134078079299...649006084095) */
         .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 155,
            .x = { .limbs = case_22, .n = 8, .cap = 8, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 155, .data.len = 155,
            .pstr = "1340780792994259709957402499820584612747936582059239337772356144"
                    "3721764030073546976801874298166903427690031858186486050853753882"
                    "811946569946433649006084095"
        },
    }, { /* 23      | -2^512 (n = 9, sign = -1)                 | 155       | 156       | STR_INVALID_CAP                           */
         .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 155,
            .x = { .limbs = case_23, .n = 9, .cap = 9, .sign = -1 }
        }, INVAL_STR(STR_INVALID_CAP, 155),
    }, { /* 24      | -(2^3072 - 1) (n = 48, sign = -1)         | 925       | 926       | STR_INVALID_CAP                           */
         .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 925,
            .x = { .limbs = case_final, .n = 48, .cap = 48, .sign = -1 }
        }, INVAL_STR(STR_INVALID_CAP, 925),
    }, { /* 25      | 2^3072 - 1 (n = 48)                       | 925       | 925       | STR_SUCCESS (580960599536...567329693695) */
        .in = &(bitos_conv_in){
            .base = 0, .uppercase = false, .len = 925,
            .x = { .limbs = case_final, .n = 48, .cap = 48, .sign = 1 }
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
        },
    }, 
    /* ---------------------------------------------------------------------------------------------------------------------------- */
};
scase ecases_b[25] = { // 5968 bytes - 6kb ---> Rounded to 6016 bytes
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
            .base = 10, .uppercase = false, .len = 1,
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
            .base = 10, .uppercase = false, .len = 12,
            .x = { .limbs = &small_mulval[0], .n = 1, .cap = 1, .sign = -1 }
        },
        .exp = { 
            .type = STRING, .status = STR_SUCCESS,
            .cap = 12, .data.len = 3, .pstr = "-10"
        },
    }, { /* 5       | 65535 (n = 1, sign = 1)                   | 16        | 8         | 2         | STR_SUCCESS ("FF")                        */
        .in = &(bitos_conv_in){
            .base = 16, .uppercase = false, .len = 8,
            .x = { .limbs = &small_mulval[1], .n = 1, .cap = 1, .sign = 1 }
        },
        .exp = { 
            .type = STRING, .status = STR_SUCCESS,
            .cap = 8, .data.len = 2, .pstr = "FF"
        },
    }, { /* 6       | 2^64 - 1 (n = 1, sign = 1)                | 2         | 64        | 64        | STR_SUCCESS ("11111111111...11111-11111") */
        .in = &(bitos_conv_in){
            .base = 2, .uppercase = false, .len = 64,
            .x = { .limbs = &small_mulval[2], .n = 1, .cap = 1, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 64, .data.len = 64, 
            .pstr = "11111111111111111111111111111111"
                    "11111111111111111111111111111111"
        },
    }, { /* 7       | -(2^64 - 1) (n = 1, sign = -1)            | 8         | 20        | 23        | STR_INVALID_CAP                           */
        .in = &(bitos_conv_in){
            .base = 8, .uppercase = false, .len = 20,
            .x = { .limbs = &small_mulval[2], .n = 1, .cap = 1, .sign = -1 }
        }, INVAL_STR(STR_INVALID_CAP, 20),
    },
    /* -------------------------------------------------------------- EDGE CASES -------------------------------------------------------------- */
    { /* 8          | -2^96 (n = 2, sign = -1)                  | 5         | 29        | 43        | STR_INVALID_CAP                           */
        .in = &(bitos_conv_in){
            .base = 5, .uppercase = false, .len = 29,
            .x = { .limbs = case_8, .n = 2, .cap = 2, .sign = -1 }
        }, INVAL_STR(STR_INVALID_CAP, 29),
    }, { /* 9       | 2^128 - 1 (n = 2)                         | 16        | 39        | 32        | STR_SUCCESS (ffffffffffff...ffffffffffff) */
        .in = &(bitos_conv_in){
            .base = 16, .uppercase = false, .len = 39,
            .x = { .limbs = case_9, .n = 2, .cap = 2, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 39, .data.len = 32, 
            .pstr = "ffffffffffffffffffffffffffffffff"
        },
    }, { /* 10      | SUPER SUPER SPARSE (n = 8)                | 2         | 449       | 449       | STR_SUCCESS (100000000000...000000000000) */
        .in = &(bitos_conv_in){
            .base = 2, .uppercase = false, .len = 449,
            .x = { .limbs = case_10, .n = 8, .cap = 8, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 449, .data.len = 449, 
            .pstr = "1000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000"
                    "0"
        },
    }, { /* 11      | Alternating 1 (n = 4, sign = -1)          | 8         | 86        | 86        | STR_SUCCESS (-52525252525...252525252525) */
        .in = &(bitos_conv_in){
            .base = 8, .uppercase = false, .len = 86,
            .x = { .limbs = case_11, .n = 4, .cap = 4, .sign = -1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 86, .data.len = 86,
            .pstr = "-52525252525252525252525252525252"
                    "52525252525252525252525252525252"
                    "525252525252525252525"
        },
    }, { /* 12      | Alternating 2 (n = 6)                     | 2         | 384       | 384       | STR_SUCCESS (101010101010...101010101010) */
        .in = &(bitos_conv_in){
            .base = 2, .uppercase = false, .len = 384,
            .x = { .limbs = case_12, .n = 6, .cap = 6, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 384, .data.len = 384,
            .pstr = "1010101010101010101010101010101010101010101010101010101010101010"
                    "1010101010101010101010101010101010101010101010101010101010101010"
                    "1010101010101010101010101010101010101010101010101010101010101010"
                    "1010101010101010101010101010101010101010101010101010101010101010"
                    "1010101010101010101010101010101010101010101010101010101010101010"
                    "1010101010101010101010101010101010101010101010101010101010101010"
        },
    }, { /* 13      | Double Alternating (n = 4, sign = -1)     | 2         | 255       | 255       | STR_SUCCESS (-11001100110...001100110011) */
        .in = &(bitos_conv_in){
            .base = 2, .uppercase = false, .len = 255,
            .x = { .limbs = case_13, .n = 4, .cap = 4, .sign = -1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 255, .data.len = 255,
            .pstr = "-1100110011001100110011001100110011001100110011001100110011001100"
                    "1100110011001100110011001100110011001100110011001100110011001100"
                    "1100110011001100110011001100110011001100110011001100110011001100"
                    "11001100110011001100110011001100110011001100110011001100110011"
        },
    }, { /* 14      | MSB All Limbs (n = 8)                     | 64        | 86        | 86        | STR_SUCCESS (200000000008...080000000000) */
        .in = &(bitos_conv_in){
            .base = 64, .uppercase = false, .len = 86,
            .x = { .limbs = case_12, .n = 8, .cap = 8, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 86, .data.len = 86,
            .pstr = "2000000000080000000000W000000000"
                    "2000000000080000000000W000000000"
                    "2000000000080000000000"
        },
    }, { /* 15      | SUPER SUPER SPARSE (n = 8) - fail edition | 13        | 120       | 122       | STR_INVALID_CAP                           */
         .in = &(bitos_conv_in){
            .base = 13, .uppercase = false, .len = 120,
            .x = { .limbs = case_10, .n = 8, .cap = 8, .sign = 1 }
        }, INVAL_STR(STR_INVALID_CAP, 120),
    }, { /* 16      | NEARLY DENSE (n = 3) - fail edition       | 6         | 50        | 75        | STR_INVALID_CAP                           */
         .in = &(bitos_conv_in){
            .base = 6, .uppercase = false, .len = 50,
            .x = { .limbs = case_16, .n = 3, .cap = 3, .sign = 1 }
        }, INVAL_STR(STR_INVALID_CAP, 50),
    }, { /* 17      | Dense -> Sparse (n = 5)                   | 22        | 40        | 58        | STR_INVALID_CAP                           */
         .in = &(bitos_conv_in){
            .base = 22, .uppercase = false, .len = 40,
            .x = { .limbs = case_17, .n = 5, .cap = 5, .sign = 1 }
        }, INVAL_STR(STR_INVALID_CAP, 40),
    }, { /* 18      | 2^192 (n = 4)                             | 36        | 25        | 38        | STR_INVALID_CAP                           */
         .in = &(bitos_conv_in){
            .base = 36, .uppercase = false, .len = 25,
            .x = { .limbs = case_18, .n = 4, .cap = 4, .sign = 1 }
        }, INVAL_STR(STR_INVALID_CAP, 25),
    }, { /* 19      | 2^256 - 1 (n = 4)                         | 4         | 77        | 128       | STR_INVALID_CAP                           */
         .in = &(bitos_conv_in){
            .base = 4, .uppercase = false, .len = 77,
            .x = { .limbs = case_19, .n = 4, .cap = 4, .sign = 1 }
        }, INVAL_STR(STR_INVALID_CAP, 77),
    }, { /* 20      | 2^127 - 1 (n = 2) (Mersenne prime)        | 2         | 127       | 127       | STR_SUCCESS (111111111111...111111111111) */
         .in = &(bitos_conv_in){
            .base = 2, .uppercase = false, .len = 127,
            .x = { .limbs = case_20, .n = 2, .cap = 2, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 127, .data.len = 127,
            .pstr = "11111111111111111111111111111111"
                    "11111111111111111111111111111111"
                    "11111111111111111111111111111111"
                    "1111111111111111111111111111111"
        },
    }, { /* 21      | Absolutely Random (n = 16)                | 11        | 296       | 296       | STR_SUCCESS (235A75279904...460090954A36) */
         .in = &(bitos_conv_in){
            .base = 11, .uppercase = false, .len = 296,
            .x = { .limbs = case_22, .n = 8, .cap = 8, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 296, .data.len = 296,
            .pstr = "235A752799046623806113A0A4516A584156A00523343334109A965514374269"
                    "5945259992A9A836A460102035A924689894A4A5094359718175A43328708866"
                    "18409482812A87444A3674293A319250452339A1A07A31964A556277824A6286"
                    "952309A505683034053324953A07489A7727865835321160083493A93501AA98"
                    "2862A867A77A67406656483660A3460090954A36"
        },
    }, { /* 22      | 2^512 - 1 (n = 8)                         | 32        | 103       | 103       | STR_SUCCESS (3VVVVVVVVVVV...VVVVVVVVVVVV) */
         .in = &(bitos_conv_in){
            .base = 32, .uppercase = false, .len = 103,
            .x = { .limbs = case_22, .n = 8, .cap = 8, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 103, .data.len = 103,
            .pstr = "3VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV"
                    "VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV"
                    "VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV"
                    "VVVVVVV"
        },
    }, { /* 23      | -2^512 (n = 9, sign = -1)                 | 32        | 103       | 104       | STR_INVALID_CAP                           */
         .in = &(bitos_conv_in){
            .base = 32, .uppercase = false, .len = 103,
            .x = { .limbs = case_23, .n = 9, .cap = 9, .sign = -1 }
        }, INVAL_STR(STR_INVALID_CAP, 103),
    }, { /* 24      | -(2^3072 - 1) (n = 48, sign = -1)         | 2         | 3072      | 3073      | STR_INVALID_CAP                           */
         .in = &(bitos_conv_in){
            .base = 2, .uppercase = false, .len = 3072,
            .x = { .limbs = case_final, .n = 48, .cap = 48, .sign = -1 }
        }, INVAL_STR(STR_INVALID_CAP, 3072),
    }, { /* 25      | 2^3072 - 1 (n = 48)                       | 64        | 512       | 512       | STR_SUCCESS (////////////...////////////) */
        .in = &(bitos_conv_in){
            .base = 64, .uppercase = false, .len = 512,
            .x = { .limbs = case_final, .n = 48, .cap = 48, .sign = 1 }
        },
        .exp = {
            .type = STRING, .status = STR_SUCCESS, .cap = 512, .data.len = 512,
            .pstr = "////////////////////////////////////////////////////////////////"
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
    u8 conv_ecount = 25, conv_scount = 4;

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
    __GET_ENTROPY_FAST(&side_mix, sizeof(u64));
    seed_xoshiro256(&conv_rstate, side_mix);
    bi_rand_mod conv_rconfig = {0}; // Non-base-prefix
    bigen_init_sesh(&conv_rconfig, &conv_rstate);


    //* ------------------------------------ SUITE SETUP ------------------------------------ *//
    // to_str() - Non-base-parameter, No length param
    suite to_str_suite = {0};
    create_str_suite(&to_str_suite, "to_str - BigInt Conversion", 
        conv_scount, rcount, ecases_nob, INVERSE, ebuf_slices[0], 
        "../logs/bi_logs/bigint_to_str_sa.txt", &conv_ectx, &conv_rcon,
        &conv_rconfig, &conv_rstate
    ); to_str_suite.cap_mode = RANDOMIZED;
    fill_suite_rinv(&to_str_suite,
        &_bitos_conv_ingen, &exec_bitos_to_str,
        &inv_bitos_conv_nob, &stat_bitos_conv_nob, 
        &cmp_inv_bitos_conv, &fmt_in_to_str, &fmt_recon_bitos,
        &_bitos_conv_inlink, &_bitos_conv_insize,
        &_bitos_recon_linker, &_bitos_recon_size,
        &_bitos_outlink, &_bitos_aux2link
    );
    // to_strb() - Base-parameter, No length param
    suite to_strb_suite = {0};
    create_str_suite(&to_strb_suite, "to_strb - BigInt Conversion", 
        conv_scount, rcount, ecases_b, INVERSE, ebuf_slices[1], 
        "../logs/bi_logs/bigint_to_str_sa.txt", &conv_ectx, &conv_rcon,
        &conv_rconfig, &conv_rstate
    ); to_strb_suite.cap_mode = RANDOMIZED;
    fill_suite_rinv(&to_strb_suite,
        &_bitos_conv_ingen, &exec_bitos_to_strb,
        &inv_bitos_conv_b, &stat_bitos_conv_b, 
        &cmp_inv_bitos_conv, &fmt_in_to_strb, &fmt_recon_bitos,
        &_bitos_conv_inlink, &_bitos_conv_insize,
        &_bitos_recon_linker, &_bitos_recon_size,
        &_bitos_outlink, &_bitos_aux2link
    );
    // to_strn() - Non-base-parameter, length param
    suite to_strn_suite = {0};
    create_str_suite(&to_strn_suite, "to_strn - BigInt Conversion", 
        conv_scount, rcount, ecases_nob, INVERSE, ebuf_slices[2], 
        "../logs/bi_logs/bigint_to_str_sa.txt", &conv_ectx, &conv_rcon,
        &conv_rconfig, &conv_rstate
    ); to_strn_suite.cap_mode = RANDOMIZED;
    fill_suite_rinv(&to_strn_suite,
        &_bitos_conv_ingen, &exec_bitos_to_strn,
        &inv_bitos_conv_nob, &stat_bitos_conv_nob,
        &cmp_inv_bitos_conv, &fmt_in_to_strn, &fmt_recon_bitos,
        &_bitos_conv_inlink, &_bitos_conv_insize,
        &_bitos_recon_linker, &_bitos_recon_size,
        &_bitos_outlink, &_bitos_aux2link
    );
    // to_strnb() - Base-parameter, length param
    suite to_strnb_suite = {0};
    create_str_suite(&to_strnb_suite, "to_strnb - BigInt Conversion",
        conv_scount, rcount, ecases_b, INVERSE, ebuf_slices[3],
        "../logs/bi_logs/bigint_to_str_sa.txt", &conv_ectx, &conv_rcon,
        &conv_rconfig, &conv_rstate
    ); to_strnb_suite.cap_mode = RANDOMIZED;
    fill_suite_rinv(&to_strnb_suite,
        &_bitos_conv_ingen, &exec_bitos_to_strnb,
        &inv_bitos_conv_b, &stat_bitos_conv_b, 
        &cmp_inv_bitos_conv, &fmt_in_to_strnb, &fmt_recon_bitos,
        &_bitos_conv_inlink, &_bitos_conv_insize,
        &_bitos_recon_linker, &_bitos_recon_size,
        &_bitos_outlink, &_bitos_aux2link
    );


    //* ---------------------------------- SESSION STARTUP ---------------------------------- *//
    _libdnml_str_suite conv_suite_arr[conv_scount];
    conv_suite_arr[0] = to_str_suite;  conv_suite_arr[1] = to_strb_suite;
    conv_suite_arr[2] = to_strn_suite; conv_suite_arr[3] = to_strnb_suite;
    _libdnml_session bi_conv_sesh = {0}; create_str_session(
        &bi_conv_sesh, "I/O - BigInt --> String Conversion (Size-aware)",
        100, conv_scount, conv_suite_arr, conv_omode
    ); start_str_session(&bi_conv_sesh);
    return 0;
}