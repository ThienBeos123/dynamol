#include "../../intrinsics/risc-v64/_rv64_conn.h"
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
    _dnml_output_mode rv64_omode; if (argc >= 2) {
        uint8_t sesh_count = _stou64(argv[2], strlen(argv[2]));
        rv64_omode = (sesh_count <= 3) ? DNML_VOUT : DNML_COUT;
    } else rv64_omode = DNML_VOUT;
    u8 rv64_scount = 4 + (4 * libdnml_caps.rv64_zbb), rv64_mrin = 4;
    // Buffer Setup
    case ecases[20 * rv64_scount]; case* eslices[rv64_scount];
    _suite_slices(eslices, ecases, 20, sizeof(case), rv64_scount);
    case rcases[rcount * rv64_scount]; case* rslices[rv64_scount];
    _suite_slices(rslices, rcases, rcount, sizeof(case), rv64_scount);
    u64 rinbuf[rcount * rv64_scount][3]; u64* rinslices[rv64_scount];
    _suite_slices(rinslices, rinbuf, rcount, BYTES_IN_UINT64_T * rv64_mrin, rv64_scount);
    size_t ressizes = resbuf_size(20, rcount);
    pair resbuf[ressizes * rv64_scount]; pair* resslices[rv64_scount];
    _suite_slices(resslices, resbuf, ressizes, sizeof(pair), rv64_scount);
    _libdnml_lsuite rv64_slist[rv64_scount]; u8 curr_slice = 0;


    // -------------- ARITHMETIC TEST SUITES -------------- //
    _libdnml_lsuite add64c_suite; addc_setup(
        &add64c_suite, "add64c", 
        eslices[curr_slice], rslices[curr_slice], rcount, 
        rinslices[curr_slice], resslices[curr_slice],
        "../logs/rv64_arith.txt",
        _rv64_add64c, _cintrin_add64c
    ); ++curr_slice; rv64_slist[curr_slice] = add64c_suite;
    _libdnml_lsuite sub64b_suite; subb_setup(
        &sub64b_suite, "sub64b", 
        eslices[curr_slice], rslices[curr_slice], rcount, 
        rinslices[curr_slice], resslices[curr_slice], 
        "../logs/rv64_arith.txt",
        _rv64_sub64b, _cintrin_sub64b
    ); ++curr_slice; rv64_slist[curr_slice] = sub64b_suite;
    _libdnml_lsuite wmul128_suite; wmul_setup(
        &wmul128_suite, "wmul128", 
        eslices[curr_slice], rslices[curr_slice], rcount,
        rinslices[curr_slice], resslices[curr_slice],
        "../logs/rv64_arith.txt",
        _rv64_wmul128, _cintrin_wmul128
    ); ++curr_slice; rv64_slist[curr_slice] = wmul128_suite;
    // -------------- MODULAR ARITHETMIC TEST SUITES -------------- //
    _libdnml_lsuite modinv64_suite; modinv_setup(
        &modinv64_suite, "modinv64", 
        eslices[curr_slice], rslices[curr_slice], rcount, 
        rinslices[curr_slice], resslices[curr_slice],
        "../logs/rv64_marith.txt",
        _rv64_modinv64, _cintrin_modinv64
    ); ++curr_slice; rv64_slist[curr_slice] = modinv64_suite;
    // -------------- BITWISE OPERATIONS TEST SUITES -------------- //
    if (libdnml_caps.rv64_zbb) {
        _libdnml_lsuite clz64_suite; clz_setup(
            &clz64_suite, "clz64e", 
            eslices[curr_slice], rslices[curr_slice], rcount, 
            rinslices[curr_slice], resslices[curr_slice],
            "../logs/rv64_bitops.txt",
            _rv64_clz64, _cintrin_clz64
        ); ++curr_slice; rv64_slist[curr_slice] = clz64_suite;
        _libdnml_lsuite ctz64_suite; ctz_setup(
            &ctz64_suite, "ctz64e", 
            eslices[curr_slice], rslices[curr_slice], rcount, 
            rinslices[curr_slice], resslices[curr_slice],
            "../logs/rv64_bitops.txt",
            _rv64_ctz64, _cintrin_ctz64
        ); ++curr_slice; rv64_slist[curr_slice] = ctz64_suite;
        _libdnml_lsuite bswap64_suite; bswap_setup(
            &bswap64_suite, "bswap64", 
            eslices[curr_slice], rslices[curr_slice], rcount, 
            rinslices[curr_slice], resslices[curr_slice],
            "../logs/rv64_bitops.txt",
            _rv64_bswap64, _cintrin_bswap64
        ); ++curr_slice; rv64_slist[curr_slice] = bswap64_suite;
        _libdnml_lsuite pcnt64_suite; pcnt_setup(
            &pcnt64_suite, "pcnt64", 
            eslices[curr_slice], rslices[curr_slice], rcount, 
            rinslices[curr_slice], resslices[curr_slice],
            "../logs/rv64_bitops.txt",
            _rv64_pcnt64, _cintrin_clz64
        ); ++curr_slice; rv64_slist[curr_slice] = pcnt64_suite;
    }

    // ---------- RISC-V 64 bit SESSION INITIALIZATION ---------- //
    _libdnml_session rv64_sesh; create_lsession(
        &rv64_sesh, "Low-level RISC-V 64 bit Intrinsics",
        100, rv64_scount, rv64_slist, rv64_omode
    ); start_low_session(&rv64_sesh);

    return 0;
}