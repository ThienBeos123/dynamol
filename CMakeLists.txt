cmake_minimum_required(VERSION 4.3)
project(lib-dnml LANGUAGES C ASM)

# Set C standard (Updated to crisp C23)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set compiler flags for suppressing warnings during development
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")

# HARDWARE ARCHITECTURE
# CMakeLists.txt
if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    add_compile_options(-march=armv8-a)    # Baseline for all ARM64
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "riscv64")
    add_compile_options(-march=rv64i)      # Baseline for all RISC-V
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    add_compile_options(-march=x86-64)     # Baseline for all x86-64
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
    ${CMAKE_SOURCE_DIR}/test_ui                 # Added so lowui.h is universally accessible
)

# =====================================================
# Core Static Library (dnml_core)
# =====================================================
add_library(dnml_core STATIC
    # 1a. Main Module Files - BigNum
    dynamol/big_numbers/bigInt.c
    dynamol/big_numbers/bigInt_io.c

    # 2. System Assembly files
    include/system/__hwcaps.c
    include/system/asm/_dnml_cpuid.S
    # Character tables (lookup arrays)
    include/char_table.c

    # 3. Mathematical Intrinsic Operations Files
    intrinsics/intrinsics.c
    # 3a. ARM64 Intrinsic Files
    intrinsics/arm64/_arm64_alg.S
    intrinsics/arm64/_arm64_arith.S
    intrinsics/arm64/_arm64_bitops.S
    intrinsics/arm64/_arm64_marith.S
    intrinsics/arm64/_arm64_sec.S
    intrinsics/arm64/_arm64_hw.S
    # 3b. x86_64 Intrinsic Files - SYSTEM-V
    intrinsics/x86_64/_x86_sysv_alg.S
    intrinsics/x86_64/_x86_sysv_arith.S
    intrinsics/x86_64/_x86_sysv_marith.S
    intrinsics/x86_64/_x86_sysv_bitops.S
    intrinsics/x86_64/_x86_sysv_sec.S
    intrinsics/x86_64/_x86_sysv_hw.S
    # 3c. x86_64 Intrinsic Files - WIN64
    intrinsics/x86_64/_x86_win64_alg.S
    intrinsics/x86_64/_x86_win64_arith.S
    intrinsics/x86_64/_x86_win64_marith.S
    intrinsics/x86_64/_x86_win64_bitops.S
    intrinsics/x86_64/_x86_win64_sec.S
    intrinsics/x86_64/_x86_win64_hw.S
    # 3d. RISC-V 64 bit Intrinsics Files
    intrinsics/risc-v64/_rv64_alg.S
    intrinsics/risc-v64/_rv64_arith.S
    intrinsics/risc-v64/_rv64_marith.S
    intrinsics/risc-v64/_rv64_bitops.S
    intrinsics/risc-v64/_rv64_sec.S
    intrinsics/risc-v64/_rv64_hw.S

    # 4. Calculational Algorithms Files - calc_algo
    calc_algo/perf_based/add_sub.c
    calc_algo/perf_based/mul.c
    calc_algo/perf_based/div.c
    calc_algo/perf_based/mod.c
    calc_algo/perf_based/mod_op.c
    calc_algo/perf_based/num_theory.c
    calc_algo/perf_based/pow_root.c

    # 5. Utility Files
    util/perf_util/bigNum_utils.c
    util/perf_util/misc_utils.c
    util/perf_util/str_parse.c
    util/perf_util/rng_util.c
)

# Extra commands for clearer debugging and cleaning up
target_compile_options(dnml_core PUBLIC
    # Add this to prevent GCC/Clang flags pollution into MSVC Build
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:
        -fsanitize=address,undefined -fno-omit-frame-pointer -g
        -Wall -Wextra -pedantic
    >
    # Add this to prevent MSVC flags pollution in GCC/Clang build
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4 /permissive- /fsanitize=address
    >
)

# Pass include path properties cleanly down to linking sub-targets
target_include_directories(dnml_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 3. LINK TO THE TEST FOLDER
add_subdirectory(test/low)
add_subdirectory(test/io/bigint)
add_subdirectory(test/io/bigfloat)
add_subdirectory(test/calc/arith)
add_subdirectory(test/calc/num_theory)
add_subdirectory(test/main_math)
