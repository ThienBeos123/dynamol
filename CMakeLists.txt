cmake_minimum_required(VERSION 4.3)
 project(lib-dnml VERSION 1.0.0 LANGUAGES C ASM)

# Set C standard (Updated to crisp C23)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# HARDWARE ARCHITECTURE (Fixed for cross-platform toolchains and compilers)
if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64|ARM64")
    # Only apply -march flags if using a compiler that supports GCC/Clang syntax
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        add_compile_options(-march=armv8-a)
    endif()
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "riscv64")
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-march=rv64i)
    endif()
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        add_compile_options(-march=x86-64)
    elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
        add_compile_options(/arch:AVX2) # or your chosen default baseline for Windows x64
    endif()
endif()

# Add include directories globally
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/dnml_sys
    ${CMAKE_SOURCE_DIR}/include/dnml_sys/asm
    ${CMAKE_SOURCE_DIR}/include/_libdnml_config
    ${CMAKE_SOURCE_DIR}/include/_libdnml_mem
    ${CMAKE_SOURCE_DIR}/include/_libdnml_perf

    ${CMAKE_SOURCE_DIR}/intrinsics
    ${CMAKE_SOURCE_DIR}/intrinsics/arm64
    ${CMAKE_SOURCE_DIR}/intrinsics/risc-v64
    ${CMAKE_SOURCE_DIR}/intrinsics/x86_64
    ${CMAKE_SOURCE_DIR}/intrinsics/zvanillc

    ${CMAKE_SOURCE_DIR}/util
    ${CMAKE_SOURCE_DIR}/calc_algo
    ${CMAKE_SOURCE_DIR}/calc_algo/perf_based
    ${CMAKE_SOURCE_DIR}/calc_algo/crt_based
    ${CMAKE_SOURCE_DIR}/dynamol/bigint
    ${CMAKE_SOURCE_DIR}/dynamol/bigfloat
    ${CMAKE_SOURCE_DIR}/drypto/crypt_int
    ${CMAKE_SOURCE_DIR}/drypto/crypt_float
    ${CMAKE_SOURCE_DIR}/test_ui                 # Added so lowui.h is universally accessible
)

# =====================================================
# Core Static Library (dnml_core)
# =====================================================
add_library(dnml_core STATIC
    # 2. System Assembly files
    include/dnml_sys/__hwcaps.c
    include/dnml_sys/asm/_dnml_cpuid.S
    # Character tables (lookup arrays)
    include/char_table.c

    # 3. Mathematical Intrinsic Operations Files
    intrinsics/intrin_base.c
    intrinsics/intrinsics.c
    intrinsics/crt_intrin.c
    # 3a. ARM64 Intrinsic Files
    intrinsics/arm64/_arm64_arith.S
    intrinsics/arm64/_arm64_bitops.S
    intrinsics/arm64/_arm64_crt_alg.S
    intrinsics/arm64/_arm64_crt_equal.S
    intrinsics/arm64/_arm64_crt_i64cmp.S
    intrinsics/arm64/_arm64_crt_u64cmp.S
    intrinsics/arm64/_arm64_marith.S
    intrinsics/arm64/_arm64_sec.S
    # 3b. x86_64 Intrinsic Files - SYSTEM-V
    intrinsics/x86_64/_x86_sysv_arith.S
    intrinsics/x86_64/_x86_sysv_bitops.S
    intrinsics/x86_64/_x86_sysv_crt_alg.S
    intrinsics/x86_64/_x86_sysv_crt_equal.S
    intrinsics/x86_64/_x86_sysv_crt_i64cmp.S
    intrinsics/x86_64/_x86_sysv_crt_u64cmp.S
    intrinsics/x86_64/_x86_sysv_marith.S
    intrinsics/x86_64/_x86_sysv_sec.S
    # 3c. x86_64 Intrinsic Files - WIN64
    intrinsics/x86_64/_x86_win64_arith.S
    intrinsics/x86_64/_x86_win64_bitops.S
    intrinsics/x86_64/_x86_win64_crt_alg.S
    intrinsics/x86_64/_x86_win64_crt_equal.S
    intrinsics/x86_64/_x86_win64_crt_i64cmp.S
    intrinsics/x86_64/_x86_win64_crt_u64cmp.S
    intrinsics/x86_64/_x86_win64_marith.S
    intrinsics/x86_64/_x86_win64_sec.S
    # 3d. RISC-V 64 bit Intrinsics Files
    intrinsics/risc-v64/_rv64_arith.S
    intrinsics/risc-v64/_rv64_bitops.S
    intrinsics/risc-v64/_rv64_crt_alg.S
    intrinsics/risc-v64/_rv64_crt_equal.S
    intrinsics/risc-v64/_rv64_crt_i64cmp.S
    intrinsics/risc-v64/_rv64_crt_u64cmp.S
    intrinsics/risc-v64/_rv64_hw.S
    intrinsics/risc-v64/_rv64_marith.S
    intrinsics/risc-v64/_rv64_port_bitops.S
    intrinsics/risc-v64/_rv64_sec.S
    # 3e. VANILLA C Intrinsics Files
    intrinsics/zvanillc/_vanillc_arith.c
    intrinsics/zvanillc/_vanillc_bitops.c
    intrinsics/zvanillc/_vanillc_marith.c
    intrinsics/zvanillc/_vanillc_misc.c
    # 3f. VANILLA C Crypto Intrinsics Files
    intrinsics/crt_vanillc/_crt_vanillc_alg.c
    intrinsics/crt_vanillc/_crt_vanillc_arith.c
    intrinsics/crt_vanillc/_crt_vanillc_bitops.c
    intrinsics/crt_vanillc/_crt_vanillc_cmp.c

    # 4a. Utility Files - PERFORMANCE-BASED
    util/perf_util/bigNum_utils.c
    util/perf_util/misc_utils.c
    util/perf_util/str_parse.c
    util/perf_util/rng_util.c
    # 4b. Utility Files - CRYPTOGRAPY-BASED
    util/crt_util/crt_bnum_util.c
    util/crt_util/crt_misc_util.c
    
    calc_algo/algo_base/add_sub.c
    # 5a. Calculational Algorithms Files - PERF-BASED
    calc_algo/perf_based/mul.c
    calc_algo/perf_based/div.c
    calc_algo/perf_based/mod.c
    calc_algo/perf_based/mod_op.c
    calc_algo/perf_based/num_theory.c
    calc_algo/perf_based/pow_root.c
    # 5b. Calculational Algorithms Files - CRYPTO-BASED
    calc_algo/crt_based/crt_mul.c
    calc_algo/crt_based/crt_div.c
    calc_algo/crt_based/crt_mod.c
    calc_algo/crt_based/crt_modop.c
    calc_algo/crt_based/crt_num_theory.c

    # 1a. Main Module Files - Dynamol
    dynamol/bigint/bigInt.c
    dynamol/bigint/bigInt_io.c
    # 1b. Main Module Files - Drypto
    drypto/crypt_int/crint_core.c
    drypto/crypt_int/crint_arith.c
    drypto/crypt_int/crint_bitops.c
    drypto/crypt_int/crint_num_theory.c
)
add_library(dnml::dnml_core ALIAS dnml_core)

# Extra commands for clearer debugging and cleaning up
target_compile_options(dnml_core PUBLIC
    # Add this to prevent GCC/Clang flags pollution into MSVC Build
    $<$<C_COMPILER_ID:GNU,AppleClang,Clang>:
        -fsanitize=address,undefined -fno-omit-frame-pointer -g
        -Wall -Wextra -pedantic
    >
    # Add this to prevent MSVC flags pollution in GCC/Clang build
    $<$<C_COMPILER_ID:MSVC>: /W4 /permissive- >
)

# Pass include path properties cleanly down to linking sub-targets
target_include_directories(dnml_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Install/export package so downstream subprojects can use find_package(dnml_core)
set(DNML_CORE_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/dnml_core")

install(TARGETS dnml_core
    EXPORT dnml_coreTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

install(EXPORT dnml_coreTargets
    FILE dnml_coreTargets.cmake
    NAMESPACE dnml::
    DESTINATION ${DNML_CORE_CONFIG_INSTALL_DIR}
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/dnml_coreConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/dnml_coreConfig.cmake"
    INSTALL_DESTINATION "${DNML_CORE_CONFIG_INSTALL_DIR}"
    NO_SET_AND_CHECK_MACRO
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/dnml_coreConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/dnml_coreConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/dnml_coreConfigVersion.cmake"
    DESTINATION "${DNML_CORE_CONFIG_INSTALL_DIR}"
)

# 3. LINK TO THE TEST FOLDER
add_subdirectory(test/low)
add_subdirectory(test/io/bigint)
add_subdirectory(test/io/bigfloat)
add_subdirectory(test/calc/arith)
add_subdirectory(test/calc/num_theory)
add_subdirectory(test/main_math)
