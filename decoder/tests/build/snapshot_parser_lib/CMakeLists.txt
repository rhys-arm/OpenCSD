set(src_files
    ${OPENCSD_SRCROOT_DIR}/decoder/tests/snapshot_parser_lib/source/device_info.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/tests/snapshot_parser_lib/source/device_parser.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/tests/snapshot_parser_lib/source/snapshot_parser.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/tests/snapshot_parser_lib/source/snapshot_parser_util.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/tests/snapshot_parser_lib/source/snapshot_reader.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/tests/snapshot_parser_lib/source/ss_to_dcdtree.cpp
)

add_library(snapshot_parser STATIC ${src_files})

target_include_directories(snapshot_parser PRIVATE
    ${OPENCSD_SRCROOT_DIR}/decoder/include
    ${OPENCSD_SRCROOT_DIR}/decoder/tests/snapshot_parser_lib/include
)

target_link_options(snapshot_parser PRIVATE "LINKER:${COMMON_LINKER_FLAGS}")
