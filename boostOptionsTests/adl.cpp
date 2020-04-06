#include "gtest/gtest.h"
#include "boost/optional.hpp"
#include <chrono>
// #include "boost/optional/optional_io.hpp"
#include <sstream>
#include <ostream>
#include "logger.h"

using namespace ::testing;

const boost::optional<std::chrono::seconds> optSec{3};
const std::chrono::seconds sec{4};

// Argument dependent lookup or definition before template use
// namespace std { namespace chrono {
std::ostream& operator<<(std::ostream& os, const std::chrono::seconds& s)
{
    return os << s.count() << "s";
}
// }}

template<class CharType, class CharTrait, class T>
inline
std::basic_ostream<CharType, CharTrait>&
operator<<(std::basic_ostream<CharType, CharTrait>& out, boost::optional<T> const& v)
{
  if (out.good())
  {
    if (!v)
         out << "--" ;
    else out << ' ' << *v ;
  }

  return out;
}


template<typename T>
std::string toString(const T& t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}


namespace boost
{
TEST(ADL, adl)
{
    EXPECT_EQ("4s", toString(sec));
    EXPECT_EQ(" 3s", toString(optSec));
}
}