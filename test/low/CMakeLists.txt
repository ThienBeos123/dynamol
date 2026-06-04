cmake_minimum_required(VERSION 4.3)
project(lib-dnml LANGUAGES C ASM)

# Redirect compiled binary targets directly into the folder expected by run_low.sh
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include directories specific to the local test folder itself
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

# When building test/low as a standalone subproject, prefer the root project's
# local install tree at ../../build/install so users can do "cmake .." inside
# test/low/build without needing an absolute path.
list(APPEND CMAKE_PREFIX_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build/install"
)

if(NOT TARGET dnml::dnml_core)
    find_package(dnml_core REQUIRED CONFIG)
endif()

# Add sanitization flags and debug symbols
add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer -g)
add_link_options(-fsanitize=address,undefined)

# Define your isolated test runners
add_executable(bitops   bitops_test.c)
add_executable(i64_cmp  i64cmp_test.c)
add_executable(u64_cmp  u64cmp_test.c)

# BIND THEM: Link the root library to your test runners.
target_link_libraries(bitops    PRIVATE dnml::dnml_core)
target_link_libraries(i64_cmp   PRIVATE dnml::dnml_core)
target_link_libraries(u64_cmp   PRIVATE dnml::dnml_core)