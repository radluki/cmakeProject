cmake_minimum_required(VERSION 2.8.9)
project(cmakeProject)
 
#Bring the headers, such as Student.h into the project
include_directories(include)
include_directories(include/utils)

include_directories(${GTEST_INCLUDE_DIR})
include_directories(logger)
#Can manually add the sources using the set command as follows:
#set(SOURCES src/mainapp.cpp src/Student.cpp)
 
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "source/*.cpp" "include/*.h" "source/utils/*cpp")
 
add_executable(firstApp ${SOURCES})

file(GLOB LOGGER_SOURCES "logger/*.cpp" "logger/*.h")
add_executable(logger_example ${LOGGER_SOURCES})

enable_testing()
find_package(GTest REQUIRED)
add_executable(loggerGTest logger/test/test.cpp)
target_link_libraries(loggerGTest gtest gtest_main pthread)
add_custom_target(dummy DEPENDS loggerGTest COMMAND ./loggerGTest --gtest_filter="*LOG_BASE*")

