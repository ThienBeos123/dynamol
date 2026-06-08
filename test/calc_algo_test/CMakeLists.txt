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

# /algo_base
add_executable(add_sub_test         algo_base/add_sub_test.c)
add_executable(algo_gcd_test        algo_base/algo_gcd_test.c)
add_executable(algo_mod_test        algo_base/algo_mod_test.c)
add_executable(algo_prime_test      algo_base/algo_prime_test.c)
target_link_libraries(add_sub_test         PRIVATE dnml::dnml_core)
target_link_libraries(algo_gcd_test        PRIVATE dnml::dnml_core)
target_link_libraries(algo_mod_test        PRIVATE dnml::dnml_core)
target_link_libraries(algo_prime_test      PRIVATE dnml::dnml_core)

# /int_algo
add_executable(crt_div_test         int_algo/crt_div_test.c)
add_executable(crt_modop_test       int_algo/crt_modop_test.c)
add_executable(crt_mul_test         int_algo/crt_mul_test.c)
add_executable(perf_div_test        int_algo/perf_div_test.c)
add_executable(perf_modop_test      int_algo/perf_modop_test.c)
add_executable(perf_mul_test        int_algo/perf_mul_test.c)
add_executable(perf_pow_root_test   int_algo/perf_pow_root_test.c)
target_link_libraries(crt_div_test         PRIVATE dnml::dnml_core)
target_link_libraries(crt_modop_test       PRIVATE dnml::dnml_core)
target_link_libraries(crt_mul_test         PRIVATE dnml::dnml_core)
target_link_libraries(perf_div_test        PRIVATE dnml::dnml_core)
target_link_libraries(perf_modop_test      PRIVATE dnml::dnml_core)
target_link_libraries(perf_mul_test        PRIVATE dnml::dnml_core)
target_link_libraries(perf_pow_root_test   PRIVATE dnml::dnml_core)
