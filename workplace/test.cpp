#include "gtest/gtest.h"

#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"
#include "boost/range/algorithm.hpp"
#include "boost/range/algorithm_ext.hpp"
#include "boost/range/adaptors.hpp"
#include "boost/range/numeric.hpp"
#include <algorithm>
#include "logger.h"

#include <iostream>
#include <string>

using namespace ::testing;

class Thrower
{
public:
  Thrower()
  {
    throw std::runtime_error("LUKI");
  }

  ~Thrower()
  {
    LOG << "Destructor";
  }
};

TEST(CTRS, throwingInConstructor)
{
  EXPECT_THROW(std::make_unique<Thrower>(), std::exception);
}
