cmake_minimum_required(VERSION 4.3)
project(lib-dnml LANGUAGES C ASM)
# Redirect compiled binary targets directly into the folder expected by run_bi_io.sh
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include directories specific to the local test folder itself
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../case_gen/
)

# Define your isolated test runners - BITOS
add_executable(binary_io                binary_io.c)
add_executable(bitos_conv_runner        bitos_conv_runner.c)
add_executable(bitos_conv_sa_runner     bitos_conv_sa_runner.c)
add_executable(bitos_convf_runner       bitos_convf_runner.c)
add_executable(bitos_convf_sa_runner    bitos_convf_sa_runner.c)
add_executable(bitos_print_runner       bitos_print_runner.c)
add_executable(bitos_printf_runner      bitos_printf_runner.c)
add_executable(bitos_tconv_runner       bitos_tconv_runner.c)
add_executable(bitos_tconv_sa_runner    bitos_tconv_sa_runner.c)
add_executable(bitos_util_runner        bitos_util_runner.c)


# Define your isolated test runners - BITOS
add_executable(stobi_assign_runner      stobi_assign_runner.c)
add_executable(stobi_conv_runner        stobi_conv_runner.c)
add_executable(stobi_fscan_runner       stobi_fscan_runner.c)
add_executable(stobi_fscan_strict       stobi_fscan_strict.c)
add_executable(stobi_fscan_trunc        stobi_fscan_trunc.c)
add_executable(stobi_init_runner        stobi_init_runner.c)
add_executable(stobi_sassign_runner     stobi_sassign_runner.c)
add_executable(stobi_sassign_sa_runner  stobi_sassign_sa_runner.c)
add_executable(stobi_tassign_runner     stobi_tassign_runner.c)
add_executable(stobi_tassign_sa_runner  stobi_tassign_sa_runner.c)


# Case generation helpers (random test-data generators)
add_library(test_casegen STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../case_gen/bi_casegen.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../case_gen/str_casegen.c
)
target_include_directories(test_casegen PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../case_gen
)
target_compile_options(test_casegen PRIVATE -O2)


# BIND THEM: Link the root library to your test runners - BITOS
target_link_libraries(binary_io PRIVATE dnml_core test_casegen)
target_link_libraries(bitos_conv_runner PRIVATE dnml_core test_casegen)
target_link_libraries(bitos_conv_sa_runner PRIVATE dnml_core test_casegen)
target_link_libraries(bitos_convf_runner PRIVATE dnml_core test_casegen)
target_link_libraries(bitos_convf_sa_runner PRIVATE dnml_core test_casegen)
target_link_libraries(bitos_print_runner PRIVATE dnml_core test_casegen)
target_link_libraries(bitos_printf_runner PRIVATE dnml_core test_casegen)
target_link_libraries(bitos_tconv_runner PRIVATE dnml_core test_casegen)
target_link_libraries(bitos_tconv_sa_runner PRIVATE dnml_core test_casegen)
target_link_libraries(bitos_util_runner PRIVATE dnml_core test_casegen)

# BIND THEM: Link the root library to your test runners - STOBI
target_link_libraries(stobi_assign_runner PRIVATE dnml_core test_casegen)
target_link_libraries(stobi_conv_runner PRIVATE dnml_core test_casegen)
target_link_libraries(stobi_fscan_runner PRIVATE dnml_core test_casegen)
target_link_libraries(stobi_fscan_strict PRIVATE dnml_core test_casegen)
target_link_libraries(stobi_fscan_trunc PRIVATE dnml_core test_casegen)
target_link_libraries(stobi_init_runner PRIVATE dnml_core test_casegen)
target_link_libraries(stobi_sassign_runner PRIVATE dnml_core test_casegen)
target_link_libraries(stobi_sassign_sa_runner PRIVATE dnml_core test_casegen)
target_link_libraries(stobi_tassign_runner PRIVATE dnml_core test_casegen)
target_link_libraries(stobi_tassign_sa_runner PRIVATE dnml_core test_casegen)