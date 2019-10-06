cmake_minimum_required(VERSION 2.8.9)
project(cmakeProject)

file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*)
include(cmake/functions.cmake)

message(STATUS "Main project sources")
print_list("${SOURCES}")

include_directories(inc)
include_directories(logger)
include_directories(utils/inc)
 
add_executable(firstApp ${SOURCES})

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
