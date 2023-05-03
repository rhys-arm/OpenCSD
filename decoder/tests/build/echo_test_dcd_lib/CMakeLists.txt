set(src_files
    ${OPENCSD_SRCROOT_DIR}/decoder/tests/ext_dcd_test_eg/c_api_echo_test/ext_dcd_echo_test.c
    ${OPENCSD_SRCROOT_DIR}/decoder/tests/ext_dcd_test_eg/c_api_echo_test/ext_dcd_echo_test_fact.c
)

add_library(echo_test_dcd STATIC ${src_files})

target_include_directories(echo_test_dcd PRIVATE
    ${OPENCSD_SRCROOT_DIR}/decoder/include
)

if(UNIX)
    target_link_options(echo_test_dcd PRIVATE "LINKER:${COMMON_LINKER_FLAGS}")
endif()
