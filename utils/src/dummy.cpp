#include "logger.h"
#include "boost/optional.hpp"
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

void dummyFunction()
{
    boost::optional<int> opt = 4;
    LOG << "Dummyi!!!!!!!!!!!" << opt.value();
    path p{"a/b/c.txt"};
    LOG << p.stem();
    LOG << p.extension();
}
