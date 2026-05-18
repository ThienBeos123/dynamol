cmake_minimum_required(VERSION 3.10)
project(Dynamol LANGUAGES C ASM)

# Set C standard (Updated to crisp C23)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set compiler flags for suppressing warnings during development
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")

# Add include directories globally
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/system
    ${CMAKE_SOURCE_DIR}/include/system/asm
    ${CMAKE_SOURCE_DIR}/sconfigs
    ${CMAKE_SOURCE_DIR}/sconfigs/memory
    ${CMAKE_SOURCE_DIR}/sconfigs/perf
    ${CMAKE_SOURCE_DIR}/sconfigs/settings
    ${CMAKE_SOURCE_DIR}/intrinsics
    ${CMAKE_SOURCE_DIR}/intrinsics/arm64
    ${CMAKE_SOURCE_DIR}/intrinsics/risc-v64
    ${CMAKE_SOURCE_DIR}/intrinsics/x86_64
    ${CMAKE_SOURCE_DIR}/intrinsics/zvanillc
    ${CMAKE_SOURCE_DIR}/util
    ${CMAKE_SOURCE_DIR}/calc_algo
    ${CMAKE_SOURCE_DIR}/calc_algo/header
    ${CMAKE_SOURCE_DIR}/adynamol/big_numbers
    ${CMAKE_SOURCE_DIR}/test_ui                 # Added so lowui.h is universally accessible
)

# =====================================================
# Core Static Library (dnml_core)
# =====================================================
add_library(dnml_core STATIC
    # 1a. Main Module Files - BigNum
    adynamol/big_numbers/bigInt.c
    adynamol/big_numbers/bigInt_io.c

    # 2. Calculational Algorithms Files - calc_algo
    calc_algo/src/add_sub.c
    calc_algo/src/mul.c
    calc_algo/src/div.c
    calc_algo/src/mod.c
    calc_algo/src/mod_op.c
    calc_algo/src/num_theory.c
    calc_algo/src/pow_root.c

    # 3. Utility Files
    util/bigNum_utils.c
    util/misc_utils.c
    util/str_parse.c
    util/sec_util.c

    # 4. Mathematical Intrinsic Operations Files
    intrinsics/intrinsics.c
    # 4a. ARM64 Intrinsic Files
    intrinsics/arm64/_arm64_alg.S
    intrinsics/arm64/_arm64_arith.S
    intrinsics/arm64/_arm64_bitops.S
    intrinsics/arm64/_arm64_marith.S
    # 4b. x86_64 Intrinsic Files
    intrinsics/x86_64/_x86_sysv_alg.S
    intrinsics/x86_64/_x86_sysv_arith.S
    intrinsics/x86_64/_x86_sysv_marith.S
    intrinsics/x86_64/_x86_sysv_bitops.S
    intrinsics/x86_64/_x86_win64_alg.S
    intrinsics/x86_64/_x86_win64_arith.S
    intrinsics/x86_64/_x86_win64_marith.S
    intrinsics/x86_64/_x86_win64_bitops.S
    # 4c. RISC-V 64 bit Intrinsics Files
    intrinsics/risc-v64/_rv64_alg.S
    intrinsics/risc-v64/_rv64_arith.S
    intrinsics/risc-v64/_rv64_marith.S
    intrinsics/risc-v64/_rv64_bitops.S

    # 5. System Assembly files
    include/system/__hwcaps.c
    include/system/asm/_dnml_cpuid.S
    # Character tables (lookup arrays)
    include/char_table.c
)

# FIXED: Target applied to dnml_core instead of non-existent "main"
target_compile_options(dnml_core PUBLIC -Wno-unused-function)

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
