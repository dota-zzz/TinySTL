#ifndef MYTINYSTL_CONSTRUCT_H_
#define MYTINYSTL_CONSTRUCT_H_

#include <iterator>
#include <new>
#include <type_traits>
#include <utility>

namespace tinystl {
template <typename T> void construct(T *ptr) { ::new ((void *)ptr) T(); }

template <typename T, typename... Args> void construct(T *ptr, Args &&...args) {
  ::new ((void *)ptr) T(std::forward<Args>(args)...);
}

template <typename T> void destory_one(T *ptr, std::true_type) {}

template <typename T> void destory_one(T *ptr, std::false_type) {
  if (ptr != nullptr) {
    ptr->~T();
  }
}

template <typename ForwardIter>
void destory_cat(ForwardIter first, ForwardIter last, std::true_type) {}

template <typename ForwardIter>
void destory_cat(ForwardIter first, ForwardIter last, std::false_type) {
  for (; first != last; ++first) {
    destory_one(&(*first));
  }
}

template <typename T> void destory(T *ptr) {
  destory_one(ptr, std::is_trivially_destructible<T>{});
}

template <typename ForwardIter>
void destory(ForwardIter first, ForwardIter last) {
  destory_cat(first, last,
              std::is_trivially_destructible<
                  typename std::iterator_traits<ForwardIter>::value_type>{});
}

} // namespace tinystl

#endif