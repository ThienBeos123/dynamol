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
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build/install"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build"
)

if(NOT TARGET dnml::dnml_core)
    find_package(dnml_core REQUIRED CONFIG)
endif()

# Add sanitization flags and debug symbols
add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer -g)
add_link_options(-fsanitize=address,undefined)

# Define your isolated test runners
add_executable(bitops_test      bitops_test.c)
add_executable(equality_test    equality_test.c)
add_executable(i64_cmp_test     i64cmp_test.c)
add_executable(u64_cmp_test     u64cmp_test.c)

# BIND THEM: Link the root library to your test runners.
target_link_libraries(bitops_test   PRIVATE dnml::dnml_core)
target_link_libraries(equality_test PRIVATE dnml::dnml_core)
target_link_libraries(i64_cmp_test  PRIVATE dnml::dnml_core)
target_link_libraries(u64_cmp_test  PRIVATE dnml::dnml_core)