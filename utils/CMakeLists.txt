cmake_minimum_required(VERSION 2.8.9)
project(utils)

file(GLOB SOURCES src/*)
include_directories(inc)
add_library(utils ${SOURCES})
target_include_directories(utils INTERFACE inc)
target_link_libraries(utils logger)
