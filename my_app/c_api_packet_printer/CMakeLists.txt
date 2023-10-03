cmake_minimum_required(VERSION 3.24)
project("c_api_packet_printer")

include(CMakePrintHelpers)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OPENCSD_SRCROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
get_filename_component(OPENCSD_SRCROOT_DIR ${OPENCSD_SRCROOT_DIR} ABSOLUTE)


add_executable(c_api_packet_printer c_api_packet_printer.cpp)

target_include_directories(c_api_packet_printer PRIVATE
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

target_link_libraries(c_api_packet_printer PRIVATE
    ${OPENCSD_C_API_LIB}
    ${OPENCSD_LIB}
)

target_link_options(c_api_packet_printer PRIVATE "LINKER:-z,defs")
