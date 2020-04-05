#include <iterator>
#include "logger.h"

template <typename T>
class LinearRangeIterator {
public:
  using difference_type = size_t;
  using value_type = T;
  using reference = T;
  using pointer = void;
  using iterator_category = std::bidirectional_iterator_tag;

  LinearRangeIterator(T start, T step_size, size_t idx)
  : start_{start}
  , step_size_{step_size}
  , idx_{idx}
  {}
  auto operator==(const LinearRangeIterator& lri) const{
    return idx_==lri.idx_;
  }
  auto operator!=(const LinearRangeIterator& lri) const{
    return !(*this==lri);
  }
  auto& operator++() { ++idx_; return *this; }
  auto& operator--() { --idx_; return *this; }
  auto operator*() const { return start_ + (idx_*step_size_); }
private:
  T start_{};
  T step_size_{};
  size_t idx_{};
};

int main()
{
    LinearRangeIterator<int> it{0, 10, 0};
    using ItType = LinearRangeIterator<int>;
    static_assert(std::is_same<std::iterator_traits<ItType>::iterator_category, std::bidirectional_iterator_tag>::value, "");
    for(; it != LinearRangeIterator<int>{0, 10, 4}; ++it)
    {
        LOG << *it;
    }
    return 0;
}
