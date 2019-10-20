project(boost_options)

enable_testing()
find_package(GTest REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options filesystem regex)
add_executable(optionsTests test.cpp)
target_link_libraries(optionsTests 
	gtest 
	gtest_main 
	pthread 
	logger 
	Boost::program_options 
	Boost::filesystem 
	Boost::regex
)

