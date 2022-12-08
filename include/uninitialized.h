#include "construct.h"
#include "iterator.h"
#include <algorithm>
#include <memory>
#include <type_traits>

namespace tinystl {

template <typename InputIterator, typename ForwardIterator>
ForwardIterator uninit_move(InputIterator first, InputIterator last,
                            ForwardIterator result, std::false_type) {
  for (; first != last; ++first, ++result) {
    tinystl::construct(&*result, std::move(*first));
  }
  return result;
}

template <typename InputIterator, typename ForwardIterator>
ForwardIterator uninit_move(InputIterator first, InputIterator last,
                            ForwardIterator result, std::true_type) {
  return std::move(first, last, result);
}

template <typename InputIterator, typename ForwardIterator>
ForwardIterator uninitialized_move(InputIterator first, InputIterator last,
                                   ForwardIterator result) {
  return uninit_move(
      first, last, result,
      std::is_trivially_move_assignable<
          typename iterator_traits<ForwardIterator>::value_type>{});
}

} // namespace tinystl