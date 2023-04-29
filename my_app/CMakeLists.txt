cmake_minimum_required(VERSION 3.24)
project("etmv4")

set(OPENCSD_SRCROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
get_filename_component(OPENCSD_SRCROOT_DIR ${OPENCSD_SRCROOT_DIR} ABSOLUTE)

include(CMakePrintHelpers)

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


set(src_files
    etmv4.cpp
)

add_executable(etmv4 ${src_files})

target_include_directories(etmv4 PRIVATE
    ${OPENCSD_SRCROOT_DIR}/decoder/include
    ${OPENCSD_SRCROOT_DIR}/decoder/include/common
)

find_library(OPENCSD_LIB
    NAMES libopencsd.so libopencsd.a opencsd_static.lib
	PATHS ${OPENCSD_SRCROOT_DIR}/build/decoder/build/ref_trace_decode_lib ${OPENCSD_SRCROOT_DIR}/build/decoder/build/ref_trace_decode_lib/Release ${OPENCSD_SRCROOT_DIR}/build/decoder/build/ref_trace_decode_lib/Debug
)
cmake_print_variables(OPENCSD_LIB)

target_link_libraries(etmv4 PRIVATE
    ${OPENCSD_LIB}
)

if(UNIX)
    target_link_options(etmv4 PRIVATE "LINKER:${COMMON_LINKER_FLAGS}")
endif()
