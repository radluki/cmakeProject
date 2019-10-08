cmake_minimum_required(VERSION 2.8.9)
project(utils)

find_package(Boost REQUIRED COMPONENTS filesystem)
include(../cmake/functions.cmake)

file(GLOB SOURCES src/*)
print_list("${SOURCES}")
add_library(utils ${SOURCES})
target_include_directories(utils PUBLIC inc)
target_link_libraries(utils PRIVATE logger)
target_link_libraries(utils PRIVATE Boost::filesystem)
