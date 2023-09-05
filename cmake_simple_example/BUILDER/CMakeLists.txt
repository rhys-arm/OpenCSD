cmake_minimum_required(VERSION 3.24)
project("Logging example")

# set(CMAKE_SKIP_RPATH TRUE)

set(APP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../app)
set(LIB_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

add_executable(logging_example
    ${APP_SRC_DIR}/logging_example.cpp
)
add_library(logger SHARED
    ${LIB_SRC_DIR}/logger.cpp
)

if(WIN32)
    target_compile_definitions(logger PRIVATE
        logger_EXPORTS
    )
endif()

# set_target_properties(logger
#     PROPERTIES NO_SONAME 1
# )

target_include_directories(logging_example PRIVATE
    ${LIB_SRC_DIR}
)

target_link_libraries(logging_example PRIVATE
    logger
)
