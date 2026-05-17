// Functions to compare
#include "../../intrinsics/arm64/_arm64_conn.h"
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
    _dnml_output_mode arm64_omode; if (argc >= 2) {
        uint8_t sesh_count = _stou64(argv[2], strlen(argv[2]));
        arm64_omode = (sesh_count <= 3) ? DNML_VOUT : DNML_COUT;
    } else arm64_omode = DNML_VOUT;
    uint8_t arm64_scount = 8, arm64_mrin = 3;
    // Buffer Setup
    case ecases[20 * arm64_scount]; case* eslices[arm64_scount];
    _suite_slices(eslices, ecases, 20, sizeof(case), arm64_scount);
    case rcases[rcount * arm64_scount]; case* rslices[arm64_scount];
    _suite_slices(rslices, rcases, rcount, sizeof(case), arm64_scount);
    u64 rinbuf[rcount * arm64_scount][3]; u64* rinslices[arm64_scount];
    _suite_slices(rinslices, rinbuf, rcount, BYTES_IN_UINT64_T * arm64_mrin, arm64_scount);
    size_t ressizes = resbuf_size(20, rcount);
    pair resbuf[ressizes * arm64_scount]; pair* resslices[arm64_scount];
    _suite_slices(resslices, resbuf, ressizes, sizeof(pair), arm64_scount);

    // -------------- ARITHMETIC TEST SUITES -------------- //
    _libdnml_lsuite add64c_suite = {}; addc_setup(
        &add64c_suite, "add64c", eslices[0],
        rslices[0], rcount, rinslices[0], resslices[0],
        "../logs/arm64_arith.txt",
        _arm64_add64c, _cintrin_add64c
    );
    _libdnml_lsuite sub64b_suite = {}; subb_setup(
        &sub64b_suite, "sub64b", eslices[1], 
        rslices[1], rcount, rinslices[1], resslices[1], 
        "../logs/arm64_arith.txt",
        _arm64_sub64b, _cintrin_sub64b
    );
    _libdnml_lsuite wmul128_suite = {}; wmul_setup(
        &wmul128_suite, "wmul128", eslices[2],
        rslices[2], rcount, rinslices[2], resslices[2],
        "../logs/arm64_arith.txt",
        _arm64_wmul128, _cintrin_wmul128
    );
    // ----------- MODULAR ARITHETMIC TEST SUITES ---------- //
    _libdnml_lsuite modinv64_suite = {}; modinv_setup(
        &modinv64_suite, "modinv64", eslices[3],
        rslices[3], rcount, rinslices[3], resslices[3],
        "../logs/arm64_marith.txt",
        _arm64_modinv64, _cintrin_modinv64
    );
    // --- ------ BITWISE OPERATIONS TEST SUITES ---------- //
    _libdnml_lsuite clz64_suite = {}; clz_setup(
        &clz64_suite, "clz64", eslices[4],
        rslices[4], rcount, rinslices[4], resslices[4],
        "../logs/arm64_bitops.txt",
        _arm64_clz64, _cintrin_clz64
    );
    _libdnml_lsuite ctz64_suite = {}; ctz_setup(
        &ctz64_suite, "ctz64", eslices[5],
        rslices[5], rcount, rinslices[5], resslices[5],
        "../logs/arm64_bitops.txt",
        _arm64_ctz64, _cintrin_ctz64
    );
    _libdnml_lsuite bswap64_suite = {}; bswap_setup(
        &bswap64_suite, "bswap64", eslices[6],
        rslices[6], rcount, rinslices[6], resslices[6],
        "../logs/arm64_bitops.txt",
        _arm64_bswap64, _cintrin_bswap64
    );
    _libdnml_lsuite pcnt64_suite = {}; pcnt_setup(
        &pcnt64_suite, "pcnt64", eslices[7],
        rslices[7], rcount, rinslices[7], resslices[7],
        "../logs/arm64_bitops.txt",
        _arm64_pcnt64, _cintrin_clz64
    );


    // ---------- ARM64 SESSION INITIALIZATION ---------- //
    _libdnml_lsuite arm64_slist[arm64_scount];
    // Arithmetic
    arm64_slist[0] = add64c_suite;      arm64_slist[1] = sub64b_suite;
    arm64_slist[2] = wmul128_suite;
    // Modular Arithmetic
    arm64_slist[3] = modinv64_suite;
    // Bitwise Operations
    arm64_slist[4] = clz64_suite;       arm64_slist[5] = ctz64_suite;
    arm64_slist[6] = bswap64_suite;     arm64_slist[7] = pcnt64_suite;
    _libdnml_session arm64_sesh; create_lsession(
        &arm64_sesh, "Low-level ARM64 Intrinsics",
        100, arm64_scount, arm64_slist, arm64_omode
    ); start_low_session(&arm64_sesh);
    return 0;
}