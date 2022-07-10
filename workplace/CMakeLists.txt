project(workplace)

enable_testing()
find_package(GTest REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options filesystem regex)
add_executable(workplace_test
	test.cpp
)
add_executable(workplace
	main.cpp
)
target_link_libraries(workplace
	logger
	Boost::program_options
	Boost::filesystem
	Boost::regex
)

target_link_libraries(workplace_test
	logger
	gtest
	gtest_main
	pthread
)