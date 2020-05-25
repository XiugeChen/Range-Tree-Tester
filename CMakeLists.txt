cmake_minimum_required(VERSION 3.15)
project(RangeTree)

include_directories(
    include
)

include(cmake/utils.cmake)

if(NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif()

set(CMAKE_CXX_STANDARD 20)

add_executable(RangeTree
    src/main.cpp
    src/data_generator.cpp
    src/org_range_tree.cpp
    src/fc_range_tree.cpp
    src/experiment_app.cpp)

spdlog_enable_warnings(RangeTree)
target_link_libraries(RangeTree PRIVATE spdlog::spdlog)