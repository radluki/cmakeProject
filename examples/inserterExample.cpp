#include "logger.h"
#include <iterator>
#include <algorithm>
#include <list>

int main() {

auto vals=std::list<int>{
  1, 2, 3, 4};
auto squared=std::list<int>{1,2,3,4,5};
std::transform(
  vals.begin(),
  vals.end(),
  std::inserter(squared, --(--squared.end())),
  [](int v) { return v * v; }
);

for( auto&& x : squared)
    LOG << x;
}
