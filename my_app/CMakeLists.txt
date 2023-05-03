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
