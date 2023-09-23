cmake_minimum_required(VERSION 3.24)
project("Logging example")

# set(CMAKE_SKIP_RPATH TRUE)

add_executable(logging_example
    app/logging_example.cpp
)
add_library(logger SHARED
    lib/logger.cpp
)

# set_target_properties(logger
#     PROPERTIES NO_SONAME 1
# )

target_include_directories(logging_example PRIVATE
    lib
)

target_link_libraries(logging_example PRIVATE
    logger
)
