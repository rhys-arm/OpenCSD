set(src_files
    ${OPENCSD_SRCROOT_DIR}/decoder/tests/source/mem_buff_demo.cpp
)

add_executable(mem_buffer_eg ${src_files})

target_include_directories(mem_buffer_eg PRIVATE
    ${OPENCSD_SRCROOT_DIR}/decoder/include
)

target_link_libraries(mem_buffer_eg PRIVATE
    snapshot_parser
    opencsd_static
)

if(UNIX)
    target_link_options(mem_buffer_eg PRIVATE "LINKER:${COMMON_LINKER_FLAGS}")
endif()
