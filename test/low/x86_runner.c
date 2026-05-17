#include "../../intrinsics/x86_64/_x86_conn.h"
#include "../../intrinsics/zvanillc/_vanillc_conn.h"
// Utilities
#include "../../util/util.h"
#include "../../test_ui/lowui.h"
#include "low_setup.h"
// STDLIB utilities
#include <system/sys.h>
#include <stdint.h>
#include <stdio.h>



int main(int argc, char **argv) {
    // ----------------- PRE-TEST SETUP ----------------- //
    // Parse terminal args + Setup env constants
    u16 rcount = (argc >= 1) ? (u16)(_stou64(argv[1], strlen(argv[1]))) : 100;
    _dnml_output_mode x86_omode; if (argc >= 2) {
        uint8_t sesh_count = _stou64(argv[2], strlen(argv[2]));
        x86_omode = (sesh_count <= 3) ? DNML_VOUT : DNML_COUT;
    } else x86_omode = DNML_VOUT;
    u8 x86_scount = 8 + (
        libdnml_caps.x86_abm 
     + libdnml_caps.x86_bmi1 
     + libdnml_caps.x86_sse4_2
    );
    u8 x86_mrin = 4;
    // Buffer Setup
    case ecases[20 * x86_scount]; case* eslices[x86_scount];
    _suite_slices(eslices, ecases, 20, sizeof(case), x86_scount);
    case rcases[rcount * x86_scount]; case* rslices[x86_scount];
    _suite_slices(rslices, rcases, rcount, sizeof(case), x86_scount);
    u64 rinbuf[rcount * x86_scount][3]; u64* rinslices[x86_scount];
    _suite_slices(rinslices, rinbuf, rcount, BYTES_IN_UINT64_T * x86_mrin, x86_scount);
    size_t ressizes = resbuf_size(20, rcount);
    pair resbuf[ressizes * x86_scount]; pair* resslices[x86_scount];
    _suite_slices(resslices, resbuf, ressizes, sizeof(pair), x86_scount);
    _libdnml_lsuite x86_slist[x86_scount]; u8 curr_slice = 0;


    // -------------- ARITHMETIC TEST SUITES -------------- //
    _libdnml_lsuite add64c_suite = {}; addc_setup(              // Slice 0
        &add64c_suite, "add64c", 
        eslices[curr_slice], rslices[curr_slice], rcount, 
        rinslices[curr_slice], resslices[curr_slice], 
        "../logs/x86_arith.txt",
        _x86_add64c, _cintrin_add64c
    ); ++curr_slice; x86_slist[curr_slice] = add64c_suite;
    _libdnml_lsuite sub64b_suite = {}; subb_setup(                   // Slice 1
        &sub64b_suite, "sub64b", 
        eslices[curr_slice], rslices[curr_slice], rcount, 
        rinslices[curr_slice], resslices[curr_slice], 
        "../logs/x86_arith.txt",
        _x86_sub64b, _cintrin_sub64b
    ); ++curr_slice; x86_slist[curr_slice] = sub64b_suite;
    _libdnml_lsuite wmul128_suite = {}; wmul_setup(                  // Slice 2
        &wmul128_suite, "wmul128", 
        eslices[curr_slice], rslices[curr_slice], rcount, 
        rinslices[curr_slice], resslices[curr_slice],
        "../logs/x86_arith.txt",
        _x86_wmul128, _cintrin_wmul128
    ); ++curr_slice; x86_slist[curr_slice] = wmul128_suite;
    _libdnml_lsuite wdiv128_suite = {}; wdiv_setup(                  // Slice 3
        &wdiv128_suite, "wdiv128", 
        eslices[curr_slice], rslices[curr_slice], rcount,
        rinslices[curr_slice], resslices[curr_slice],
        "../logs/x86_arith.txt",
        _x86_wdiv128, _cintrin_wdiv128
    ); ++curr_slice; x86_slist[curr_slice] = wdiv128_suite;
    // -------------- MODULAR ARITHETMIC TEST SUITES -------------- //
    _libdnml_lsuite modinv64_suite = {}; modinv_setup(               // Slice 4
        &modinv64_suite, "modinv64", 
        eslices[curr_slice], rslices[curr_slice], rcount, 
        rinslices[curr_slice], resslices[curr_slice],
        "../logs/x86_marith.txt",
        _x86_modinv64, _cintrin_modinv64
    ); ++curr_slice; x86_slist[curr_slice] = modinv64_suite;
    // -------------- BITWISE OPERATIONS TEST SUITES -------------- //
    if (libdnml_caps.x86_abm) { _libdnml_lsuite clz64e_suite = {}; clz_setup(    // Slice 5
            &clz64e_suite, "clz64e", 
            eslices[curr_slice], rslices[curr_slice], rcount,
            rinslices[curr_slice], resslices[curr_slice],
            "../logs/x86_bitops.txt",
            _x86_clz64e, _cintrin_clz64
        ); ++curr_slice; x86_slist[curr_slice] = clz64e_suite;
    } if (libdnml_caps.x86_bmi1) { _libdnml_lsuite ctz64e_suite = {}; ctz_setup( // Slice 6
            &ctz64e_suite, "ctz64e", 
            eslices[curr_slice], rslices[curr_slice], rcount, 
            rinslices[curr_slice], resslices[curr_slice],
            "../logs/x86_bitops.txt",
            _x86_ctz64e, _cintrin_ctz64
        ); ++curr_slice; x86_slist[curr_slice] = ctz64e_suite;
    } if (libdnml_caps.x86_sse4_2) { _libdnml_lsuite pcnt64_suite = {}; pcnt_setup(  // Slice 7
            &pcnt64_suite, "pcnt64", 
            eslices[curr_slice], rslices[curr_slice], rcount, 
            rinslices[curr_slice], resslices[curr_slice],
            "../logs/x86_bitops.txt",
            _x86_pcnt64e, _cintrin_clz64
        ); ++curr_slice; x86_slist[curr_slice] = pcnt64_suite;
    } 
    _libdnml_lsuite bswap64_suite = {}; bswap_setup(             // Slice 5/8
        &bswap64_suite, "bswap64", 
        eslices[curr_slice], rslices[curr_slice], rcount, 
        rinslices[curr_slice], resslices[curr_slice],
        "../logs/x86_bitops.txt",
        _x86_bswap64, _cintrin_bswap64
    ); ++curr_slice; x86_slist[curr_slice] = bswap64_suite;
    _libdnml_lsuite clz64s_suite = {}; clz_setup(                // Slice 6/9
        &clz64s_suite, "clz64e", 
        eslices[curr_slice], rslices[curr_slice], rcount, 
        rinslices[curr_slice], resslices[curr_slice],
        "../logs/x86_bitops.txt",
        _x86_clz64s, _cintrin_clz64
    ); ++curr_slice; x86_slist[curr_slice] = clz64s_suite;
    _libdnml_lsuite ctz64s_suite = {}; ctz_setup(                // Slice 7/10
        &ctz64s_suite, "ctz64e", 
        eslices[curr_slice], rslices[curr_slice], rcount, 
        rinslices[curr_slice], rinslices[curr_slice],
        "../logs/x86_bitops.txt",
        _x86_ctz64s, _cintrin_ctz64
    ); ++curr_slice; x86_slist[curr_slice] = ctz64s_suite;


    // ---------- X86_64 SESSION INITIALIZATION ---------- //
    _libdnml_session x86_sesh; create_lsession(
        &x86_sesh, "Low-level x86_64 Intrinsics",
        100, x86_scount, x86_slist, x86_omode
    ); start_low_session(&x86_sesh);

    return 0;
}