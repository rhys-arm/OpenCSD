set(src_files
    ${OPENCSD_SRCROOT_DIR}/decoder/source/c_api/ocsd_c_api.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/c_api/ocsd_c_api_custom_obj.cpp
)

if (UNIX)
    add_library(opencsd_c_api SHARED ${src_files})
endif()
add_library(opencsd_c_api_static STATIC ${src_files})

set(incl_dirs
    ${OPENCSD_SRCROOT_DIR}/decoder/include
)
if (UNIX)
    target_include_directories(opencsd_c_api PRIVATE
        ${incl_dirs}
    )
endif()
target_include_directories(opencsd_c_api_static PRIVATE
    ${incl_dirs}
)

target_link_libraries(opencsd_c_api_static PRIVATE
    opencsd_static
)

if(UNIX)
    target_link_libraries(opencsd_c_api PRIVATE
        opencsd
    )
    target_link_options(opencsd_c_api PRIVATE "LINKER:${COMMON_LINKER_FLAGS}")
    set_target_properties(opencsd_c_api_static PROPERTIES OUTPUT_NAME opencsd_c_api)
endif()
