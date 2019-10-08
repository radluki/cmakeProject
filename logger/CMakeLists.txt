cmake_minimum_required(VERSION 2.8.9)
project(logger)

file(GLOB SOURCES "src/*.cpp")
add_executable(logger_example ${SOURCES})
target_include_directories(logger_example PRIVATE inc)

add_library(logger INTERFACE)
target_include_directories(logger INTERFACE inc)

enable_testing()
find_package(GTest REQUIRED)
add_executable(loggerGTest test/test.cpp)
target_link_libraries(loggerGTest gtest gtest_main pthread logger)

add_custom_target(run DEPENDS loggerGTest logger_example COMMAND ./loggerGTest --gtest_color=yes && ./logger_example)
