add_library(jopencsd SHARED jopencsd_wrap.cxx)

set(jdk_incl_dirs
	$ENV{JAVA_HOME}/include
)

if(UNIX)
	list(APPEND jdk_incl_dirs $ENV{JAVA_HOME}/include/linux)
else()
	list(APPEND jdk_incl_dirs $ENV{JAVA_HOME}/include/win32)
endif()

target_include_directories(jopencsd PRIVATE
	${jdk_incl_dirs}
	../decoder/include
	../decoder/include/opencsd
	../decoder/include/opencsd/c_api
	../decoder/include/common
	../decoder/include/interfaces
	../decoder/include/opencsd/etmv4
	../decoder/include/opencsd/etmv3
	../decoder/include/pkt_printers
)

target_compile_definitions(jopencsd PRIVATE
	_OCSD_C_API_DLL_EXPORT
)

target_link_libraries(jopencsd PRIVATE
	opencsd_c_api
)
