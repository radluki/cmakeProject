cmake_minimum_required(VERSION 2.8.9)
project(logger)

include_directories(".")
file(GLOB SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.cpp" "*.h")
include(../cmake/functions.cmake)
message(STATUS "Logger sources:")
print_list("${SOURCES}")
add_executable(logger_example ${SOURCES})

enable_testing()
find_package(GTest REQUIRED)
add_executable(loggerGTest test/test.cpp logger.h)
target_link_libraries(loggerGTest gtest gtest_main pthread)
add_custom_target(dummy DEPENDS loggerGTest COMMAND ./loggerGTest --gtest_filter="*LOG_BASE*")

list(REMOVE_ITEM SOURCES main.cpp)
add_library(logger INTERFACE)
target_include_directories(logger INTERFACE .)
