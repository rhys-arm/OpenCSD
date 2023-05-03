set(src_files
    ${OPENCSD_SRCROOT_DIR}/decoder/tests/source/c_api_pkt_print_test.c
)

add_executable(c_api_pkt_print_test ${src_files})

target_include_directories(c_api_pkt_print_test PRIVATE
    ${OPENCSD_SRCROOT_DIR}/decoder/include
    ${OPENCSD_SRCROOT_DIR}/decoder/tests/ext_dcd_test_eg/c_api_echo_test
)

target_link_libraries(c_api_pkt_print_test PRIVATE
    opencsd_static
    opencsd_c_api
    echo_test_dcd
)

if(UNIX)
    target_link_options(c_api_pkt_print_test PRIVATE "LINKER:${COMMON_LINKER_FLAGS}")
endif()
