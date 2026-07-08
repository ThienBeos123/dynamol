cmake_minimum_required(VERSION 3.1...3.5)
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

# Define isolated test runners
add_executable(bitops_test      bitops_test.c)
add_executable(equality_test    equality_test.c)
add_executable(i64cmp_test      i64cmp_test.c)
add_executable(u64cmp_test      u64cmp_test.c)
add_executable(arith_test       arith_test.c)
add_executable(div_test         div_test.c)
add_executable(top_lvl_marith   top_lvl_marith.c)
add_executable(top_lvl_arith    top_lvl_arith.c)
add_executable(top_lvl_bitops   top_lvl_bitops.c)
add_executable(top_lvl_div      top_lvl_div.c)

# BIND THEM: Link the root library to test runners.
target_link_libraries(bitops_test       PRIVATE dnml::dnml_core)
target_link_libraries(equality_test     PRIVATE dnml::dnml_core)
target_link_libraries(i64cmp_test       PRIVATE dnml::dnml_core)
target_link_libraries(u64cmp_test       PRIVATE dnml::dnml_core)
target_link_libraries(arith_test        PRIVATE dnml::dnml_core)
target_link_libraries(div_test          PRIVATE dnml::dnml_core)
target_link_libraries(top_lvl_marith    PRIVATE dnml::dnml_core)
target_link_libraries(top_lvl_arith     PRIVATE dnml::dnml_core)
target_link_libraries(top_lvl_bitops    PRIVATE dnml::dnml_core)
target_link_libraries(top_lvl_div       PRIVATE dnml::dnml_core)