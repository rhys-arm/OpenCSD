cmake_minimum_required(VERSION 3.25)
project("OpenCSD")

function(set_compiler_release_optimisation_level flags_var level)
    set(EXPECTED_INPUT_VARS CMAKE_C_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELEASE)
    if(NOT flags_var IN_LIST EXPECTED_INPUT_VARS)
        message(FATAL_ERROR "flags_var must be one of ${EXPECTED_INPUT_VARS}")
    endif()

    # https://discourse.cmake.org/t/how-to-override-compiler-optimisation-level/5153
    set(flags_var_contents ${${flags_var}})
    set(REGEX_EXPR "-O[0-9]")
    string(REGEX MATCHALL ${REGEX_EXPR} match_var ${flags_var_contents})
    list(LENGTH match_var num_matches)
    if(NOT num_matches EQUAL 1 OR NOT MATCHES "${REGEX_EXPR}")
        message(FATAL_ERROR "Expected ${flags_var} to contain one -On optimisation flag, but it contained: ${flags_var_contents}")
    endif()
    string(REGEX REPLACE ${REGEX_EXPR} ${level} flags_var_contents ${flags_var_contents})
    set(${flags_var} ${flags_var_contents} PARENT_SCOPE)
endfunction()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_compiler_release_optimisation_level(CMAKE_C_FLAGS_RELEASE "-O2")
    set_compiler_release_optimisation_level(CMAKE_CXX_FLAGS_RELEASE "-O2")
endif()

set(OPENCSD_SRCROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
get_filename_component(OPENCSD_SRCROOT_DIR ${OPENCSD_SRCROOT_DIR} ABSOLUTE)

include(CMakePrintHelpers)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# -std=c++XX rather than -std=gnu++XX
set(CMAKE_CXX_EXTENSIONS OFF)
# rpath can be convenient for running binaries locally without needing LD_LIBRARY_PATH, however because it places the path inside the binaries, it harms build reproducibility
set(CMAKE_SKIP_RPATH TRUE)

if(APPLE)
    set(COMMON_LINKER_FLAGS
        "-undefined,error"
    )
elseif(UNIX)
    set(COMMON_LINKER_FLAGS
        "-z,defs"
    )
else()
    message(FATAL_ERROR "Unsupported arch")
endif()

add_subdirectory(decoder/build/ref_trace_decode_lib)
