cmake_minimum_required(VERSION 3.1...3.5)
project(lib-dnml LANGUAGES C ASM)

# Redirect compiled binary targets directly into the folder expected by run_low.sh
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include directories specific to the local test folder itself
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# When building test/low as a standalone subproject, look for a local root build
# and install tree so users can do "cmake .." inside test/low/build without
# needing an absolute path.
list(APPEND CMAKE_PREFIX_PATH
    # Basic paths for barebone builds
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build/install"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build"

    # Paths on standard Development builds
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build/unix/install"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build/unix"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build/msvc/install"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build/msvc"

    # Paths on testing release builds - Drypto
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build-drt/unix/install"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build-drt/unix"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build-drt/msvc/install"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build-drt/msvc"

    # Paths on testing release builds - Dynamol Max
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build-mdnml/unix/install"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build-mdnml/unix"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build-mdnml/msvc/install"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build-mdnml/msvc"

    # Paths on testing release builds - Dynamol Medium
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build-pdnml/unix/install"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build-pdnml/unix"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build-pdnml/msvc/install"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build-pdnml/msvc"

    # Paths on testing release builds - Dynamol Stable
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build-sdnml/unix/install"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build-sdnml/unix"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build-sdnml/msvc/install"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build-sdnml/msvc"
)
if(NOT TARGET dnml::dnml_core)
    find_package(dnml_core REQUIRED CONFIG)
endif()

# Add sanitization flags and debug symbols
add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer -g)
add_link_options(-fsanitize=address,undefined)
# /algo_base
add_executable(bi_add_test      algo_base/bi_add_test.c)
add_executable(bi_sub_test      algo_base/bi_sub_test.c)
add_executable(ci_add_sub       algo_base/ci_add_sub.c)
target_link_libraries(bi_add_test       PRIVATE dnml::dnml_core)
target_link_libraries(bi_sub_test       PRIVATE dnml::dnml_core)
target_link_libraries(ci_add_sub        PRIVATE dnml::dnml_core)



# /crt_algos
add_executable(crt_div_test             crt_algos/crt_div_test.c)
add_executable(crt_modop_test           crt_algos/crt_modop_test.c)
add_executable(crt_mul_test             crt_algos/crt_mul_test.c)
add_executable(top_cdiv_test            crt_algos/top_cdiv_test.c)
add_executable(top_cmodop_test          crt_algos/top_cmodop_test.c)
add_executable(top_cmul_test            crt_algos/top_cmul_test.c)
target_link_libraries(crt_div_test          PRIVATE dnml::dnml_core)
target_link_libraries(crt_modop_test        PRIVATE dnml::dnml_core)
target_link_libraries(crt_mul_test          PRIVATE dnml::dnml_core)
target_link_libraries(top_cdiv_test         PRIVATE dnml::dnml_core)
target_link_libraries(top_cmodop_test       PRIVATE dnml::dnml_core)
target_link_libraries(top_cmul_test         PRIVATE dnml::dnml_core)
# /crt_float





# /heap_algos
add_executable(_heap_matmul_asymxz_test         heap_algos/_heap_matmul_asymxz_test.c)
add_executable(_heap_matmul_asymyw_test         heap_algos/_heap_matmul_asymyw_test.c)
add_executable(_heap_matmul_sym_test            heap_algos/_heap_matmul_sym_test.c)
add_executable(_heap_matmul_test                heap_algos/_heap_matmul_test.c)
add_executable(heap_div_test            heap_algos/heap_div_test.c)
add_executable(heap_gcd_test            heap_algos/heap_gcd_test.c)
add_executable(heap_mod_test            heap_algos/heap_mod_test.c)
add_executable(heap_modmulexp_test      heap_algos/heap_modmulexp_test.c)
add_executable(heap_mul_asym_test       heap_algos/heap_mul_asym_test.c)
add_executable(heap_mul_test            heap_algos/heap_mul_test.c)
add_executable(heap_pow_root_test       heap_algos/heap_pow_root_test.c)
add_executable(heap_ptest_test          heap_algos/heap_ptest_test.c)
add_executable(top_hdiv_test        heap_algos/top_hdiv_test.c)
add_executable(top_hgcd_test        heap_algos/top_hgcd_test.c)
add_executable(top_hmod_test        heap_algos/top_hmod_test.c)
add_executable(top_hmodmulexp_test  heap_algos/top_hmodmulexp_test.c)
add_executable(top_hmul_test        heap_algos/top_hmul_test.c)
add_executable(top_hpow_root_test   heap_algos/top_hpow_root_test.c)
add_executable(top_hptest_test      heap_algos/top_hptest_test.c)
target_link_libraries(_heap_matmul_asymxz_test      PRIVATE dnml::dnml_core)
target_link_libraries(_heap_matmul_asymyw_test      PRIVATE dnml::dnml_core)
target_link_libraries(_heap_matmul_sym_test         PRIVATE dnml::dnml_core)
target_link_libraries(_heap_matmul_test             PRIVATE dnml::dnml_core)
target_link_libraries(heap_div_test         PRIVATE dnml::dnml_core)
target_link_libraries(heap_gcd_test         PRIVATE dnml::dnml_core)
target_link_libraries(heap_mod_test         PRIVATE dnml::dnml_core)
target_link_libraries(heap_modmulexp_test   PRIVATE dnml::dnml_core)
target_link_libraries(heap_mul_asym_test    PRIVATE dnml::dnml_core)
target_link_libraries(heap_mul_test         PRIVATE dnml::dnml_core)
target_link_libraries(heap_pow_root_test    PRIVATE dnml::dnml_core)
target_link_libraries(heap_ptest_test       PRIVATE dnml::dnml_core)
target_link_libraries(top_hdiv_test             PRIVATE dnml::dnml_core)
target_link_libraries(top_hgcd_test             PRIVATE dnml::dnml_core)
target_link_libraries(top_hmod_test             PRIVATE dnml::dnml_core)
target_link_libraries(top_hmodmulexp_test       PRIVATE dnml::dnml_core)
target_link_libraries(top_hmul_test             PRIVATE dnml::dnml_core)
target_link_libraries(top_hpow_root_test        PRIVATE dnml::dnml_core)
target_link_libraries(top_hptest_test           PRIVATE dnml::dnml_core)
# /heap_float






# /perf_algos
add_executable(_perf_matmul_asymxz_test         perf_algos/_perf_matmul_asymxz_test.c)
add_executable(_perf_matmul_asymyw_test         perf_algos/_perf_matmul_asymyw_test.c)
add_executable(_perf_matmul_sym_test            perf_algos/_perf_matmul_sym_test.c)
add_executable(_perf_matmul_test                perf_algos/_perf_matmul_test.c)
add_executable(perf_div_test            perf_algos/perf_div_test.c)
add_executable(perf_gcd_test            perf_algos/perf_gcd_test.c)
add_executable(perf_mod_test            perf_algos/perf_mod_test.c)
add_executable(perf_modmulexp_test      perf_algos/perf_modmulexp_test.c)
add_executable(perf_mul_asym_test       perf_algos/perf_mul_asym_test.c)
add_executable(perf_mul_test            perf_algos/perf_mul_test.c)
add_executable(perf_pow_root_test       perf_algos/perf_pow_root_test.c)
add_executable(perf_ptest_test          perf_algos/perf_ptest_test.c)
add_executable(top_pdiv_test        perf_algos/top_pdiv_test.c)
add_executable(top_pgcd_test        perf_algos/top_pgcd_test.c)
add_executable(top_pmod_test        perf_algos/top_pmod_test.c)
add_executable(top_pmodmulexp_test  perf_algos/top_pmodmulexp_test.c)
add_executable(top_pmul_test        perf_algos/top_pmul_test.c)
add_executable(top_ppow_root_test   perf_algos/top_ppow_root_test.c)
add_executable(top_pptest_test      perf_algos/top_pptest_test.c)
target_link_libraries(_perf_matmul_asymxz_test      PRIVATE dnml::dnml_core)
target_link_libraries(_perf_matmul_asymyw_test      PRIVATE dnml::dnml_core)
target_link_libraries(_perf_matmul_sym_test         PRIVATE dnml::dnml_core)
target_link_libraries(_perf_matmul_test             PRIVATE dnml::dnml_core)
target_link_libraries(perf_div_test         PRIVATE dnml::dnml_core)
target_link_libraries(perf_gcd_test         PRIVATE dnml::dnml_core)
target_link_libraries(perf_mod_test         PRIVATE dnml::dnml_core)
target_link_libraries(perf_modmulexp_test   PRIVATE dnml::dnml_core)
target_link_libraries(perf_mul_asym_test    PRIVATE dnml::dnml_core)
target_link_libraries(perf_mul_test         PRIVATE dnml::dnml_core)
target_link_libraries(perf_pow_root_test    PRIVATE dnml::dnml_core)
target_link_libraries(perf_ptest_test       PRIVATE dnml::dnml_core)
target_link_libraries(top_pdiv_test             PRIVATE dnml::dnml_core)
target_link_libraries(top_pgcd_test             PRIVATE dnml::dnml_core)
target_link_libraries(top_pmod_test             PRIVATE dnml::dnml_core)
target_link_libraries(top_pmodmulexp_test       PRIVATE dnml::dnml_core)
target_link_libraries(top_pmul_test             PRIVATE dnml::dnml_core)
target_link_libraries(top_ppow_root_test        PRIVATE dnml::dnml_core)
target_link_libraries(top_pptest_test           PRIVATE dnml::dnml_core)
# /perf_float
