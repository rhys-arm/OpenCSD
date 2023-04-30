set(src_files
    ${OPENCSD_SRCROOT_DIR}/decoder/tests/source/trc_pkt_lister.cpp
)

add_executable(trc_pkt_lister ${src_files})

target_include_directories(trc_pkt_lister PRIVATE
    ${OPENCSD_SRCROOT_DIR}/decoder/include
    ${OPENCSD_SRCROOT_DIR}/decoder/tests/snapshot_parser_lib/include
)

target_link_libraries(trc_pkt_lister PRIVATE
    snapshot_parser
    opencsd_static
)

if(UNIX)
    target_link_options(trc_pkt_lister PRIVATE "LINKER:${COMMON_LINKER_FLAGS}")
endif()
