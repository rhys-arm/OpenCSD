set(src_files
    ${OPENCSD_SRCROOT_DIR}/decoder/source/c_api/ocsd_c_api.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/c_api/ocsd_c_api_custom_obj.cpp
)

add_library(opencsd_c_api SHARED ${src_files})

target_include_directories(opencsd_c_api PRIVATE
    ${OPENCSD_SRCROOT_DIR}/decoder/include
)

target_compile_definitions(opencsd_c_api PRIVATE
	_OCSD_C_API_DLL_EXPORT
)

target_link_libraries(opencsd_c_api PRIVATE
    opencsd
)

if(UNIX)
    target_link_options(opencsd_c_api PRIVATE "LINKER:${COMMON_LINKER_FLAGS}")
endif()
