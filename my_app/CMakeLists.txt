cmake_minimum_required(VERSION 3.24)
project("etmv4")

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

if(UNIX AND CMAKE_BUILD_TYPE STREQUAL "Release")
    set_compiler_release_optimisation_level(CMAKE_C_FLAGS_RELEASE "-O2")
    set_compiler_release_optimisation_level(CMAKE_CXX_FLAGS_RELEASE "-O2")
elseif(UNIX AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS_DEBUG "-g -O2")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O2")
endif()

include(CMakePrintHelpers)
cmake_print_variables(CMAKE_C_FLAGS_RELEASE)
cmake_print_variables(CMAKE_CXX_FLAGS_RELEASE)
cmake_print_variables(CMAKE_C_FLAGS_DEBUG)
cmake_print_variables(CMAKE_CXX_FLAGS_DEBUG)

set(OPENCSD_SRCROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
get_filename_component(OPENCSD_SRCROOT_DIR ${OPENCSD_SRCROOT_DIR} ABSOLUTE)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OPENCSD_SRCROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
get_filename_component(OPENCSD_SRCROOT_DIR ${OPENCSD_SRCROOT_DIR} ABSOLUTE)

if(APPLE)
    set(COMMON_LINKER_FLAGS
        "-undefined,error"
    )
elseif(UNIX)
    set(COMMON_LINKER_FLAGS
        "-z,defs"
    )
elseif(WIN32)
    set(COMMON_LINKER_FLAGS
        ""
    )
else()
    message(FATAL_ERROR "Unsupported arch")
endif()
cmake_print_variables(COMMON_LINKER_FLAGS)


add_executable(etmv4 etmv4.cpp)
add_executable(etmv4_c_api etmv4_c_api.cpp)

target_include_directories(etmv4 PRIVATE
    ${OPENCSD_SRCROOT_DIR}/decoder/include
    ${OPENCSD_SRCROOT_DIR}/decoder/include/common
)
target_include_directories(etmv4_c_api PRIVATE
    ${OPENCSD_SRCROOT_DIR}/decoder/include
    ${OPENCSD_SRCROOT_DIR}/decoder/include/common
    ${OPENCSD_SRCROOT_DIR}/decoder/include/opencsd/c_api
)

find_library(OPENCSD_LIB
    NAMES libopencsd.so libopencsd.a opencsd_static.lib
	PATHS ${OPENCSD_SRCROOT_DIR}/build/decoder/build/ref_trace_decode_lib ${OPENCSD_SRCROOT_DIR}/build/decoder/build/ref_trace_decode_lib/Release ${OPENCSD_SRCROOT_DIR}/build/decoder/build/ref_trace_decode_lib/Debug
)
cmake_print_variables(OPENCSD_LIB)

find_library(OPENCSD_C_API_LIB
    NAMES libopencsd_c_api.so libopencsd_c_api.a opencsd_c_api_static.lib
	PATHS ${OPENCSD_SRCROOT_DIR}/build/decoder/build/rctdl_c_api_lib ${OPENCSD_SRCROOT_DIR}/build/decoder/build/rctdl_c_api_lib/Release ${OPENCSD_SRCROOT_DIR}/build/decoder/build/rctdl_c_api_lib/Debug
)
cmake_print_variables(OPENCSD_C_API_LIB)

target_link_libraries(etmv4 PRIVATE
    ${OPENCSD_LIB}
)
target_link_libraries(etmv4_c_api PRIVATE
    ${OPENCSD_C_API_LIB}
    ${OPENCSD_LIB}
)

if(UNIX)
    target_link_options(etmv4 PRIVATE "LINKER:${COMMON_LINKER_FLAGS}")
    target_link_options(etmv4_c_api PRIVATE "LINKER:${COMMON_LINKER_FLAGS}")
endif()
