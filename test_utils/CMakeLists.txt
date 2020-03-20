project(test_utils)

enable_testing()
find_package(GTest REQUIRED)
find_package(Boost REQUIRED)
add_executable(test_utils test_utils.cpp)
target_include_directories(test_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(test_utils 
	gtest 
	gtest_main 
	pthread 
	logger
)

