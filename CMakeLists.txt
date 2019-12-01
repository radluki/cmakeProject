cmake_minimum_required(VERSION 2.8.9)
project(cmakeProject)


option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." TRUE)
if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options(-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options(-fcolor-diagnostics)
    endif ()
endif ()
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*)
include(cmake/functions.cmake)


message(STATUS "Main project sources")
print_list("${SOURCES}")

add_executable(firstApp ${SOURCES})
target_include_directories(firstApp PRIVATE inc)

option(OWN_UTILS "Use own libutils.a or external" ON)
if(OWN_UTILS)
    message(STATUS "Linking to own library")
    add_subdirectory(utils)
    target_link_libraries(firstApp utils)
else()
    message(STATUS "Linking to exteranlly built library")
    target_link_libraries(firstApp ~/cmakeProject/utils/build/libutils.a)
endif()
add_subdirectory(logger)
target_link_libraries(firstApp logger)

add_custom_target(run_all
    DEPENDS
        firstApp
        loggerGTest
        logger_example
    COMMAND
        ./firstApp &&
        ./logger/loggerGTest --gtest_color=yes &&
        ./logger/logger_example
)

add_subdirectory(boostOptionsTests)
add_subdirectory(examples)
