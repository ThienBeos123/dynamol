cmake_minimum_required(VERSION 3.1...3.5)
project(lib-dnml VERSION 1.0.0 LANGUAGES C ASM)
# Handle non-ELF Mach-O linking overrides on macOS
if(APPLE)
    # Clear ELF-specific attributes so they don't break AppleClang
    set(CMAKE_EXE_LINKER_FLAGS "" CACHE STRING "" FORCE)
    set(CMAKE_SHARED_LINKER_FLAGS "" CACHE STRING "" FORCE)
endif()


# Set C standard (Updated to crisp C23)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# HARDWARE ARCHITECTURE (Fixed for cross-platform toolchains and compilers)
if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64|ARM64")
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        add_compile_options(-march=armv8-a)
        # Inject native ARM Branch Protection & Pointer Authentication instead of Intel CET
        add_compile_options(-mbranch-protection=standard)
    endif()
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "riscv64")
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-march=rv64i)
    endif()
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        add_compile_options(-march=x86-64)
        # Inject Intel Control-Flow protection safely here since the hardware supports it
        add_compile_options(-fcf-protection=full)
    elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
        add_compile_options(/arch:AVX2) 
    endif()
endif()




# Define the primary static library target
add_library(dnml_core STATIC
    # 2. System Assembly files
    include/dnml_sys/__hwcaps.c
    include/tables.c

    # 3. Mathematical Intrinsic Operations Files
    intrinsics/intrin_base.c
    intrinsics/intrinsics.c
    intrinsics/crt_intrin.c
    # 3a. VANILLA C Intrinsics Files
    intrinsics/zvanillc/_vanillc_arith.c
    intrinsics/zvanillc/_vanillc_bitops.c
    intrinsics/zvanillc/_vanillc_marith.c
    intrinsics/zvanillc/_vanillc_misc.c
    # 3b. VANILLA C Crypto Intrinsics Files
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
    # 5a. Calculational Algorithms Files - PERF-BASED + ARENA
    calc_algo/perf_based/mul_fft.c
    calc_algo/perf_based/mul_toom_45.c
    calc_algo/perf_based/mul_toom_p5.c
    calc_algo/perf_based/mul.c
    # Sub-5a. Matrix-Vector Multiplication helpers
    calc_algo/perf_based/mv_matmul/_matmul_fft.c
    calc_algo/perf_based/mv_matmul/_matmul_toom.c
    calc_algo/perf_based/mv_matmul/_mv_matmul_disp.c
    calc_algo/perf_based/mv_matmul/_mv_matmul_ws.c
    calc_algo/perf_based/div_newton.c
    calc_algo/perf_based/div.c
    calc_algo/perf_based/mod.c
    calc_algo/perf_based/mod_mulexp.c
    calc_algo/perf_based/pow_root.c
    calc_algo/perf_based/gcd_subq.c
    calc_algo/perf_based/gcd.c
    calc_algo/perf_based/ptest_bpsw.c
    calc_algo/perf_based/prime_test.c
    # 5b. Calculational Algorithms Files - PERF-BASED + HEAP
    calc_algo/heap_based/heap_mul_toom.c
    calc_algo/heap_based/heap_mul_fft.c
    calc_algo/heap_based/heap_mul.c
    # Sub-5b. Matrix-Vector Multiplication Helpers
    calc_algo/heap_based/hmv_matmul/_hmatmul_fft.c
    calc_algo/heap_based/hmv_matmul/_hmatmul_toom.c
    calc_algo/heap_based/hmv_matmul/_hmv_matmul.c
    calc_algo/heap_based/heap_div_newt.c
    calc_algo/heap_based/heap_div.c
    calc_algo/heap_based/heap_mod.c
    calc_algo/heap_based/heap_modmulexp.c
    calc_algo/heap_based/heap_pow_root.c
    calc_algo/heap_based/heap_gcd_subq.c
    calc_algo/heap_based/heap_gcd.c
    calc_algo/heap_based/heap_ptest_bpsw.c
    calc_algo/heap_based/heap_ptest.c
    # 5c. Calculational Algorithms Files - CRYPTO-BASED
    calc_algo/crt_based/crt_mul.c
    calc_algo/crt_based/crt_div.c
    calc_algo/crt_based/crt_mod.c
    calc_algo/crt_based/crt_modop.c
    calc_algo/crt_based/crt_num_theory.c

    # 1a. Main Module Files - Dynamol
    dynamol/_dynamol_arena.c
    dynamol/bigint/bigInt.c
    dynamol/bigint/bigInt_io.c
    # 1b. Main Module Files - Drypto
    drypto/crypt_int/crint_core.c
    drypto/crypt_int/crint_arith.c
    drypto/crypt_int/crint_bitops.c
    drypto/crypt_int/crint_num_theory.c
)
if(APPLE) 
    # If on Apple, Clang may propagate C compilation flags down into Assembly,
    # which causes early abortion on hand-written Assembly.
    set_property(SOURCE 
        include/dnml_sys/asm/_dnml_cpuid.S
        intrinsics/arm64/_arm64_arith.S
        intrinsics/arm64/_arm64_bitops.S
        intrinsics/arm64/_arm64_crt_alg.S
        intrinsics/arm64/_arm64_crt_equal.S
        intrinsics/arm64/_arm64_crt_i64cmp.S
        intrinsics/arm64/_arm64_crt_u64cmp.S
        intrinsics/arm64/_arm64_marith.S
        intrinsics/arm64/_arm64_sec.S
        PROPERTY COMPILE_OPTIONS "-Xarch_asm;-fno-stack-protector"
    )
endif()

# ------------ CPU-ARCH ASSEMBLY COMPILATION ------------
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    if(CMAKE_C_COMPILER_ID MATCHES "MSVC")
        target_sources(dnml_core PRIVATE
            intrinsics/x86_64/_x86_win64_arith.S
            intrinsics/x86_64/_x86_win64_bitops.S
            intrinsics/x86_64/_x86_win64_crt_alg.S
            intrinsics/x86_64/_x86_win64_crt_equal.S
            intrinsics/x86_64/_x86_win64_crt_i64cmp.S
            intrinsics/x86_64/_x86_win64_crt_u64cmp.S
            intrinsics/x86_64/_x86_win64_marith.S
            intrinsics/x86_64/_x86_win64_sec.S
        )
        target_compile_definitions(dnml_core PRIVATE __ABI_X64_WIN64__=1)
    else()
        target_sources(dnml_core PRIVATE
            include/dnml_sys/asm/_dnml_cpuid.S
            intrinsics/x86_64/_x86_sysv_arith.S
            intrinsics/x86_64/_x86_sysv_bitops.S
            intrinsics/x86_64/_x86_sysv_crt_alg.S
            intrinsics/x86_64/_x86_sysv_crt_equal.S
            intrinsics/x86_64/_x86_sysv_crt_i64cmp.S
            intrinsics/x86_64/_x86_sysv_crt_u64cmp.S
            intrinsics/x86_64/_x86_sysv_marith.S
            intrinsics/x86_64/_x86_sysv_sec.S
        )
        target_compile_definitions(dnml_core PRIVATE __ABI_X64_SYSV__=1)
    endif()
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64|ARM64")
    target_sources(dnml_core PRIVATE
        include/dnml_sys/asm/_dnml_cpuid.S
        intrinsics/arm64/_arm64_arith.S
        intrinsics/arm64/_arm64_bitops.S
        intrinsics/arm64/_arm64_crt_alg.S
        intrinsics/arm64/_arm64_crt_equal.S
        intrinsics/arm64/_arm64_crt_i64cmp.S
        intrinsics/arm64/_arm64_crt_u64cmp.S
        intrinsics/arm64/_arm64_marith.S
        intrinsics/arm64/_arm64_sec.S
    )
endif()



# -------- TARGET INCLUDE DIRECTORIES MAPPING --------
# 1. PRIVATE: Used strictly to resolve #include <...> safely inside this project repository.
# 2. INTERFACE: Exposed strictly to downstream consumers using the installed package tree folder structure.
target_include_directories(dnml_core
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR} # Resolves public root headers inside project modules
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dnml_sys
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dnml_sys/asm
        ${CMAKE_CURRENT_SOURCE_DIR}/include/_libdnml_config
        ${CMAKE_CURRENT_SOURCE_DIR}/include/_libdnml_mem
        ${CMAKE_CURRENT_SOURCE_DIR}/include/_libdnml_perf
        ${CMAKE_CURRENT_SOURCE_DIR}/intrinsics
        ${CMAKE_CURRENT_SOURCE_DIR}/intrinsics/arm64
        ${CMAKE_CURRENT_SOURCE_DIR}/intrinsics/riscv64
        ${CMAKE_CURRENT_SOURCE_DIR}/intrinsics/x86_64
        ${CMAKE_CURRENT_SOURCE_DIR}/intrinsics/zvanillc
        ${CMAKE_CURRENT_SOURCE_DIR}/intrinsics/crt_vanillc
        ${CMAKE_CURRENT_SOURCE_DIR}/util
        ${CMAKE_CURRENT_SOURCE_DIR}/calc_algo
        ${CMAKE_CURRENT_SOURCE_DIR}/calc_algo/perf_based
        ${CMAKE_CURRENT_SOURCE_DIR}/calc_algo/perf_based/mv_matmul
        ${CMAKE_CURRENT_SOURCE_DIR}/calc_algo/perf_float
        ${CMAKE_CURRENT_SOURCE_DIR}/calc_algo/heap_based
        ${CMAKE_CURRENT_SOURCE_DIR}/calc_algo/heap_based/hmv_matmul
        ${CMAKE_CURRENT_SOURCE_DIR}/calc_algo/heap_float
        ${CMAKE_CURRENT_SOURCE_DIR}/calc_algo/crt_based
        ${CMAKE_CURRENT_SOURCE_DIR}/calc_algo/crt_float
        ${CMAKE_CURRENT_SOURCE_DIR}/calc_algo/algo_base
        ${CMAKE_CURRENT_SOURCE_DIR}/dynamol
        ${CMAKE_CURRENT_SOURCE_DIR}/dynamol/bigint
        ${CMAKE_CURRENT_SOURCE_DIR}/dynamol/bigfloat
        ${CMAKE_CURRENT_SOURCE_DIR}/drypto/crypt_int
        ${CMAKE_CURRENT_SOURCE_DIR}/drypto/crypt_float
        ${CMAKE_CURRENT_SOURCE_DIR}/test # Added in for print_bi_limbs and print_ci_limbs
    INTERFACE
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}> # Allows downstreams to find <dnml/libdnml.h> safely
)




# ==============================================================================
# PACKAGE EXPORTATION STEPS FOR LIBRRARY
# ==============================================================================
# Create an alias target matching your installation namespace layout.
# This prevents find_package from crashing downstream internal tests.
add_library(dnml::dnml_core ALIAS dnml_core)
install(TARGETS dnml_core
    EXPORT dnml_coreTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install *ONLY* your primary root api header interfaces cleanly into a namespaced target folder
install(FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/libdnml.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/libdnml.hpp"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/dnml"
)
set(DNML_CORE_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/dnml_core")
install(EXPORT dnml_coreTargets
    FILE dnml_coreTargets.cmake
    NAMESPACE dnml::
    DESTINATION ${DNML_CORE_CONFIG_INSTALL_DIR}
)

# Create a build-tree export file as well, so the root build directory can be used directly
export(EXPORT dnml_coreTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/dnml_coreTargets.cmake"
    NAMESPACE dnml::
)

# Define a standard export package template directly within the build script
set(DNML_CONFIG_TEMPLATE_CONTENT [=[
@PACKAGE_INIT@

# Include the auto-generated export target mappings
include("${CMAKE_CURRENT_LIST_DIR}/dnml_coreTargets.cmake")

check_required_components(dnml_core)
]=])

# Write out the structural template dynamically to the build environment directory
set(DNML_CONFIG_IN_FILE "${CMAKE_CURRENT_BINARY_DIR}/dnml_coreConfig.cmake.in")
file(WRITE "${DNML_CONFIG_IN_FILE}" "${DNML_CONFIG_TEMPLATE_CONTENT}")

# Process the generated configuration script template cleanly
configure_package_config_file(
    "${DNML_CONFIG_IN_FILE}"
    "${CMAKE_CURRENT_BINARY_DIR}/dnml_coreConfig.cmake"
    INSTALL_DESTINATION "${DNML_CORE_CONFIG_INSTALL_DIR}"
    NO_SET_AND_CHECK_MACRO
)

# Build the basic version file asset tracker
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/dnml_coreConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Install both configuration targets into their system destinations
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/dnml_coreConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/dnml_coreConfigVersion.cmake"
    DESTINATION "${DNML_CORE_CONFIG_INSTALL_DIR}"
)

# Configure and Install pkg-config tracking structures
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/libdnml_core.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/libdnml_core.pc"
    @ONLY
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libdnml_core.pc"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")




# ==============================================================================
# DEVOLUTION TO TEST STRUCTURES
# ==============================================================================
# We define a global indicator to tell subdirectories that dnml_core is internal
set(DNML_INTERNAL_BUILD ON)
add_subdirectory(test/intrin_test)
add_subdirectory(test/util_test)
add_subdirectory(test/calc_algo_test)
add_subdirectory(test/io_test/bigint)
add_subdirectory(test/io_test/bigfloat)
add_subdirectory(test/main_test/bigint)
add_subdirectory(test/main_test/bigfloat)
add_subdirectory(test/main_test/crint)