cmake_minimum_required(VERSION 2.8.9)
project(utils)

file(GLOB SOURCES src/*)
add_library(utils ${SOURCES})
target_include_directories(utils PUBLIC inc)
target_link_libraries(utils logger)
