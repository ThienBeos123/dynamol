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
        add_compile_options(/arch:AVX2) 
    endif()
endif()

# Define the primary static library target
add_library(dnml_core STATIC
    # System & Character tables
    include/dnml_sys/__hwcaps.c
    include/char_table.c

    # Core Primitives & Intrinsics
    intrinsics/intrin_base.c
    intrinsics/intrinsics.c
    intrinsics/crt_intrin.c

    # Vanilla Fallbacks
    intrinsics/zvanillc/_vanillc_arith.c
    intrinsics/zvanillc/_vanillc_bitops.c
    intrinsics/zvanillc/_vanillc_marith.c
    intrinsics/zvanillc/_vanillc_misc.c
    intrinsics/crt_vanillc/_crt_vanillc_alg.c
    intrinsics/crt_vanillc/_crt_vanillc_arith.c
    intrinsics/crt_vanillc/_crt_vanillc_bitops.c
    intrinsics/crt_vanillc/_crt_vanillc_cmp.c

    # Utilities
    util/perf_util/bigNum_utils.c
    util/perf_util/misc_utils.c
    util/perf_util/str_parse.c
    util/perf_util/rng_util.c
    util/crt_util/crt_bnum_util.c
    util/crt_util/crt_misc_util.c

    # Algorithmic layers
    calc_algo/algo_base/add_sub.c
    calc_algo/perf_based/mul.c
    calc_algo/perf_based/div.c
    calc_algo/perf_based/mod.c
    calc_algo/perf_based/mod_op.c
    calc_algo/perf_based/num_theory.c
    calc_algo/perf_based/pow_root.c

    # Constant-time layers
    calc_algo/crt_based/crt_mul.c
    calc_algo/crt_based/crt_div.c
    calc_algo/crt_based/crt_mod.c
    calc_algo/crt_based/crt_modop.c
    calc_algo/crt_based/crt_num_theory.c

    # High-level engine implementations
    dynamol/_dynamol_arena.c
    dynamol/bigint/bigInt.c
    dynamol/bigint/bigInt_io.c
    drypto/crypt_int/crint_core.c
    drypto/crypt_int/crint_arith.c
    drypto/crypt_int/crint_bitops.c
    drypto/crypt_int/crint_num_theory.c
)

# CONDITIONAL ARCHITECTURE ASSEMBLY COMPILATION
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

# TARGET INCLUDE DIRECTORIES MAPPING
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
        ${CMAKE_CURRENT_SOURCE_DIR}/calc_algo/crt_based
        ${CMAKE_CURRENT_SOURCE_DIR}/calc_algo/algo_base
        ${CMAKE_CURRENT_SOURCE_DIR}/dynamol
        ${CMAKE_CURRENT_SOURCE_DIR}/dynamol/bigint
        ${CMAKE_CURRENT_SOURCE_DIR}/dynamol/bigfloat
        ${CMAKE_CURRENT_SOURCE_DIR}/drypto/crypt_int
        ${CMAKE_CURRENT_SOURCE_DIR}/drypto/crypt_float
    INTERFACE
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}> # Allows downstreams to find <dnml/libdnml.h> safely
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(dnml_core PRIVATE 
        -Wall -Wextra -Wpedantic -Werror
        # Miscellanous Warning Suppression
        -Wno-misleading-indentation
        -Wno-bitwise-instead-of-logical
        # Unused-class Warning Supression
        -Wno-unused-parameter -Wno-unused-function 
        -Wno-unused-variable -Wno-unused-value 
        -Wno-unused-macros -Wno-unused-but-set-variable
        -Wno-unused-but-set-parameter
    )
elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
    target_compile_options(dnml_core PRIVATE 
        /W4 /WX
        /wd4100 # Unused parameter
        /wd4505 # Unused local function
        /wd4101 # Unused local variable
    )
endif()




# ==============================================================================
# PACKAGE EXPORTATION STEPS FOR LIBRRARY
# ==============================================================================
# Create an alias target matching your installation namespace layout.
# This prevents find_package from crashing downstream internal tests.
add_library(dnml::dnml_core ALIAS dnml_core)
# INSTALLATION RULES MAPPING
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
add_subdirectory(test/low)
add_subdirectory(test/io/bigint)
add_subdirectory(test/io/bigfloat)
add_subdirectory(test/calc/bigint)
add_subdirectory(test/calc/bigfloat)
add_subdirectory(test/calc/crint)
add_subdirectory(test/main_math/bigint)
add_subdirectory(test/main_math/bigfloat)
add_subdirectory(test/main_math/crint)