cmake_minimum_required(VERSION 4.3)
project(lib-dnml LANGUAGES C ASM)

# Redirect compiled binary targets directly into the folder expected by run_low.sh
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include directories specific to the local test folder itself
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
)

# When building test/low as a standalone subproject, look for a local root build
# and install tree so users can do "cmake .." inside test/low/build without
# needing an absolute path.
list(APPEND CMAKE_PREFIX_PATH
    # Basic paths for barebone builds
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/install"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../build"

    # Paths on standard Development builds
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/unix/install"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/unix"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/msvc/install"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/msvc"

    # Paths on testing release builds - Drypto
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../build-crt/unix/install"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../build-crt/unix"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../build-crt/msvc/install"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../build-crt/msvc"

    # Paths on testing release builds - Dynamol
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../build-perf/unix/install"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../build-perf/unix"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../build-perf/msvc/install"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../build-perf/msvc"
)
if(NOT TARGET dnml::dnml_core)
    find_package(dnml_core REQUIRED CONFIG)
endif()