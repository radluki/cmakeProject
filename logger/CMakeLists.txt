cmake_minimum_required(VERSION 2.8.9)
project(logger)

include(../cmake/functions.cmake)

file(GLOB SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/*.cpp")
message(STATUS "Logger sources:")
print_list("${SOURCES}")
add_executable(logger_example ${SOURCES})
target_include_directories(logger_example PRIVATE inc)

add_library(logger INTERFACE)
target_include_directories(logger INTERFACE inc)

enable_testing()
find_package(GTest REQUIRED)
add_executable(loggerGTest test/test.cpp)
target_link_libraries(loggerGTest gtest gtest_main pthread logger)

add_custom_target(run DEPENDS loggerGTest logger_example COMMAND ./loggerGTest && ./logger_example)
