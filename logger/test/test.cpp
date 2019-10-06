#include "gtest/gtest.h"
#include "logger.h"
#include <ostream>
#include <sstream>

using namespace testing;

struct LoggerTest : public Test
{
	std::stringstream ss;
};

TEST_F(LoggerTest, test_Logger_getStream)
{
	Logger(ss).getStream("file", "func", 99) << "message";
	EXPECT_EQ("file:func():99 message\n", ss.str());
}

TEST_F(LoggerTest, test_LOG_BASE_macro_test)
{
	LOG_BASE(ss) << "message";
	EXPECT_EQ("test.cpp:TestBody():21 message\n", ss.str());
}
