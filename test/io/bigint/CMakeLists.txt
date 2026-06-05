cmake_minimum_required(VERSION 4.3)
project(lib-dnml LANGUAGES C ASM)

if(NOT TARGET dnml::dnml_core)
    find_package(dnml_core REQUIRED CONFIG)
endif()