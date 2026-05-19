cmake_minimum_required(VERSION 4.3)
project(lib-dnml LANGUAGES C ASM)

# Redirect compiled binary targets directly into the folder expected by run_low.sh
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include directories specific to the local test folder itself
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

# Define your isolated test runners
add_executable(arm64_runner arm64_runner.c)
add_executable(rv64_runner  rv64_runner.c)
add_executable(x86_runner   x86_runner.c)

# BIND THEM: Link the root library to your test runners.
target_link_libraries(arm64_runner PRIVATE dnml_core)
target_link_libraries(rv64_runner  PRIVATE dnml_core)
target_link_libraries(x86_runner   PRIVATE dnml_core)