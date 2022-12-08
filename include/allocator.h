#ifndef MYTINYSTL_ALLOCATOR_H_
#define MYTINYSTL_ALLOCATOR_H_

#include "construct.h"
#include <cstddef>
#include <new>
#include <utility>

namespace tinystl {
template <typename T> class allocator {

public:
  using size_type = std::size_t;

public:
  static T *allocate();
  static T *allocate(size_type);

  static void deallocate(T *);

  static void construct(T *);
  template <typename... Args> static void construct(T *, Args &&...args);

  static void destory(T *);
};

template <typename T> T *allocator<T>::allocate() {
  return static_cast<T *>(::operator new(sizeof(T)));
}

template <typename T> T *allocator<T>::allocate(size_type n) {
  return static_cast<T *>(::operator new(n * sizeof(T)));
}

template <typename T> void allocator<T>::deallocate(T *ptr) {
  ::operator delete(ptr);
}

template <typename T> void allocator<T>::construct(T *ptr) {
  tinystl::construct(ptr);
}

template <typename T>
template <typename... Args>
void allocator<T>::construct(T *ptr, Args &&...args) {
  tinystl::construct(ptr, std::forward<Args>(args)...);
}

template <typename T> void allocator<T>::destory(T *ptr) {
  tinystl::destory(ptr);
}
} // namespace tinystl

#endif