#include "gtest/gtest.h"

#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"
#include "boost/range/algorithm.hpp"
#include "boost/range/algorithm_ext.hpp"
#include "boost/range/adaptors.hpp"
#include "boost/range/numeric.hpp"
#include <algorithm>

#include <iostream>
#include <string>

using namespace ::testing;

namespace po = boost::program_options;

TEST(RangesTest, test_ranges)
{
  std::vector<int> v(5);
  std::iota(v.begin(), v.end(), 0);
  EXPECT_EQ(5, v.size());
  EXPECT_EQ(3, v[3]);
  std::vector<int> v_odd;
  auto is_odd = [](const auto &x) { return x % 2 == 1; };
  boost::range::copy(v | boost::adaptors::filtered(is_odd), std::inserter(v_odd, v_odd.end()));
  boost::range::remove_erase_if(v, [&is_odd](const auto &x) { return !is_odd(x); });
  const std::vector<int> expected_v_odd{1, 3};
  EXPECT_EQ(expected_v_odd, v);
  EXPECT_EQ(expected_v_odd, v_odd);
}

class OptionsTest : public Test
{
public:
  int add = 0;
  std::vector<std::string> sentence;

  auto getOptions()
  {
    po::options_description desc{"Options"};
    desc.add_options()("help,h", "Print help messages")(
        "word,w", po::value<std::vector<std::string>>(&sentence),
        "use this option to append to vector")(
        "necessary,n", po::value<std::string>()->required(), "required option")(
        "add", po::value<int>(&add)->required(), "positional option, int");
    return desc;
  }

  auto getPositional()
  {
    po::positional_options_description positionalOptions;
    positionalOptions.add("add", 1);
    return positionalOptions;
  }

  auto getParsedVariables(int argc, const char **argv)
  {
    po::variables_map vm;
    auto desc = getOptions();
    auto positionalOptions = getPositional();
    po::store(po::command_line_parser(argc, argv)
                  .options(desc)
                  .positional(positionalOptions)
                  .run(),
              vm);
    return vm;
  }
};

TEST_F(OptionsTest, shouldParseHelpLong)
{
  const char *argv[] = {"program", "--help"};
  int argc = 2;

  auto vm = getParsedVariables(argc, argv);

  EXPECT_EQ(1, vm.count("help"));
}

TEST_F(OptionsTest, shouldParseHelpShort)
{
  const char *argv[] = {"program", "-h"};
  int argc = 2;

  auto vm = getParsedVariables(argc, argv);

  EXPECT_EQ(1, vm.count("help"));
}

TEST_F(OptionsTest, shouldThrow_ifHelpIsDuplicated)
{
  const char *argv[] = {"program", "-h", "--help"};
  int argc = 3;

  EXPECT_THROW(getParsedVariables(argc, argv), std::exception);
}

TEST_F(OptionsTest, shouldParseVector_withoutNotify)
{
  const char *argv[] = {"program", "-w", "word1", "--word", "word2"};
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
  const char *argv[] = {"program", "-n", "nec", "-w", "word1",
                        "--word", "word2", "--add", "4"};
  int argc = 9;

  auto vm = getParsedVariables(argc, argv);
  po::notify(vm);
  ASSERT_EQ(2, sentence.size());
  EXPECT_EQ("word1", sentence[0]);
  EXPECT_EQ("word2", sentence[1]);

  EXPECT_EQ("nec", vm["necessary"].as<std::string>());
  EXPECT_EQ(4, add);
}
