cmake_minimum_required(VERSION 4.3)
project(lib-dnml LANGUAGES C ASM)

# Redirect compiled binary targets directly into the folder expected by run_low.sh
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include directories specific to the local test folder itself
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
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
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build-crt/unix/install"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build-crt/unix"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build-crt/msvc/install"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build-crt/msvc"

    # Paths on testing release builds - Dynamol
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build-perf/unix/install"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build-perf/unix"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build-perf/msvc/install"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build-perf/msvc"
)
if(NOT TARGET dnml::dnml_core)
    find_package(dnml_core REQUIRED CONFIG)
endif()

# Add sanitization flags and debug symbols
add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer -g)
add_link_options(-fsanitize=address,undefined)

# /bnum_utils
add_executable(bnum_add_mul         bnum_utils/bnum_add_mul.c)
add_executable(bnum_cmp_test        bnum_utils/bnum_cmp_test.c)
add_executable(bnum_copy_test       bnum_utils/bnum_copy_test.c)
add_executable(bnum_count_digit     bnum_utils/bnum_count_digit.c)
add_executable(bnum_count_require   bnum_utils/bnum_count_require.c)
add_executable(bnum_div_test        bnum_utils/bnum_div_test.c)
add_executable(bnum_limb_shift      bnum_utils/bnum_limb_shift.c)
add_executable(bnum_shift_test      bnum_utils/bnum_shift_test.c)
add_executable(bnum_valid_test      bnum_utils/bnum_valid_test.c)
target_link_libraries(bnum_add_mul          PRIVATE dnml::dnml_core)
target_link_libraries(bnum_cmp_test         PRIVATE dnml::dnml_core)
target_link_libraries(bnum_copy_test        PRIVATE dnml::dnml_core)
target_link_libraries(bnum_count_digit      PRIVATE dnml::dnml_core)
target_link_libraries(bnum_count_require    PRIVATE dnml::dnml_core)
target_link_libraries(bnum_div_test         PRIVATE dnml::dnml_core)
target_link_libraries(bnum_limb_shift       PRIVATE dnml::dnml_core)
target_link_libraries(bnum_shift_test       PRIVATE dnml::dnml_core)
target_link_libraries(bnum_valid_test       PRIVATE dnml::dnml_core)


# /crt_bnum_utils
add_executable(cnum_cmp_test            crt_bnum_utils/cnum_cmp_test.c)
add_executable(cnum_correction_test     crt_bnum_utils/cnum_correction_test.c)
add_executable(cnum_limb_shift          crt_bnum_utils/cnum_limb_shift.c)
target_link_libraries(cnum_cmp_test         PRIVATE dnml::dnml_core)
target_link_libraries(cnum_correction_test  PRIVATE dnml::dnml_core)
target_link_libraries(cnum_limb_shift       PRIVATE dnml::dnml_core)


# /misc_utils
add_executable(crt_conv_test    misc_utils/crt_conv_test.c)
add_executable(crt_mem_test     misc_utils/crt_mem_test.c)
add_executable(crt_u64mem_test  misc_utils/crt_u64mem_test.c)
add_executable(misc_conv_test   misc_utils/misc_conv_test.c)
target_link_libraries(crt_conv_test     PRIVATE dnml::dnml_core)
target_link_libraries(crt_mem_test      PRIVATE dnml::dnml_core)
target_link_libraries(crt_u64mem_test   PRIVATE dnml::dnml_core)
target_link_libraries(misc_conv_test    PRIVATE dnml::dnml_core)


# /str_parse
add_executable(prefix_handle_test   str_parse/prefix_handle_test.c)
add_executable(sign_handle_test     str_parse/sign_handle_test.c)
add_executable(skip_char_test       str_parse/skip_char_test.c)
target_link_libraries(prefix_handle_test    PRIVATE dnml::dnml_core)
target_link_libraries(sign_handle_test      PRIVATE dnml::dnml_core)
target_link_libraries(skip_char_test        PRIVATE dnml::dnml_core)