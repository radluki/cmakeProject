#include "gtest/gtest.h"

#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"

#include <iostream>
#include <string>


using namespace ::testing;

namespace po = boost::program_options;


class OptionsTest : public Test
{
public:
	int add = 0;
    std::vector<std::string> sentence;

	auto getOptions()
	{
		po::options_description desc{"Options"};
		desc.add_options()
          ("help,h", "Print help messages")
          ("word,w", po::value<std::vector<std::string> >(&sentence),
		  	"use this option to append to vector")
          ("necessary,n", po::value<std::string>()->required(), "required option")
          ("add", po::value<int>(&add)->required(), "positional option, int");
		return desc;
	}

	auto getPositional()
	{
        po::positional_options_description positionalOptions;
   		positionalOptions.add("add", 1);
		return positionalOptions;
	}

	auto getParsedVariables(int argc, const char** argv)
	{
	    po::variables_map vm;
		auto desc = getOptions();
		auto positionalOptions = getPositional();
      	po::store(po::command_line_parser(argc, argv)
		          .options(desc)
                  .positional(positionalOptions)
				  .run(), vm);
		return vm;
	}

};


TEST_F(OptionsTest, shouldParseHelpLong)
{
	const char* argv[] = {"program","--help"};
	int argc = 2;

	auto vm = getParsedVariables(argc, argv);

	EXPECT_EQ(1, vm.count("help"));
}


TEST_F(OptionsTest, shouldParseHelpShort)
{
	const char* argv[] = {"program","-h"};
	int argc = 2;

	auto vm = getParsedVariables(argc, argv);

	EXPECT_EQ(1, vm.count("help"));
}

TEST_F(OptionsTest, shouldThrow_ifHelpIsDuplicated)
{
	const char* argv[] = {"program","-h", "--help"};
	int argc = 3;

	EXPECT_THROW(getParsedVariables(argc, argv), std::exception);
}

TEST_F(OptionsTest, shouldParseVector_withoutNotify)
{
	const char* argv[] = {"program", "-w","word1","--word","word2"};
	int argc = 5;

	auto vm = getParsedVariables(argc, argv);
	EXPECT_EQ(1, vm.count("word"));
	EXPECT_EQ(0, sentence.size());
	auto vec = vm["word"].as<std::vector<std::string>>();
	ASSERT_EQ(2, vec.size());
	EXPECT_EQ("word1", vec[0]);
	EXPECT_EQ("word2", vec[1]);
}
TEST_F(OptionsTest, shouldParseVector_withNotify)
{
	const char* argv[] = {"program","-n","nec","-w","word1","--word","word2", "--add", "4"};
	int argc = 9;

	auto vm = getParsedVariables(argc, argv);
	po::notify(vm);
	ASSERT_EQ(2, sentence.size());
	EXPECT_EQ("word1", sentence[0]);
	EXPECT_EQ("word2", sentence[1]);

	EXPECT_EQ("nec", vm["necessary"].as<std::string>());
	EXPECT_EQ(4, add);
}
