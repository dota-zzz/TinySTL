#ifndef MYTINYSTL_VECTOR_H_
#define MYTINYSTL_VECTOR_H_

//待替换函数forward,uninitialized_copy,uninitialized_fill_n,copy_backward,copy
// std::enable_if ??
// assign??
// max_size
//为什么加noexcept
#include "allocator.h"
#include "exceptdef.h"
#include "iterator.h"
#include "uninitialized.h"
#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <type_traits>
#include <utility>

namespace tinystl {
template <typename T, typename alloc = tinystl::allocator<T>> class vector {

public:
  typedef T value_type;
  typedef T *pointer;
  typedef T &reference;
  typedef std::ptrdiff_t difference_type;
  typedef std::size_t size_type;
  typedef T *iterator;
  typedef const T *const_iterator;
  typedef tinystl::reverse_iterator<iterator> reverse_iterator;
  typedef const tinystl::reverse_iterator<iterator> const_reverse_iterator;
  typedef const T &const_reference;

protected:
  typedef alloc data_allocator;
  iterator allocate_construct_fill(size_type n, const T &t);
  void fill_init(size_type n, const T &t);
  void init(size_type n = 16);
  void allocate_push_back(const T &t);
  void destory_deallocate_recover();
  template <
      typename InputIterator,
      typename std::enable_if<
          tinystl::has_input_iterator_cat<InputIterator>::value, int>::type = 0>
  void range_init(InputIterator, InputIterator);

private:
  iterator start;
  iterator finish;
  iterator end_of_storage;

public:
  //初始化
  vector() { init(); }
  vector(size_type n, const T &t = T()) { fill_init(n, t); }
  explicit vector(size_type n) { init(n); }
  template <
      typename InputIterator,
      typename std::enable_if<
          tinystl::has_input_iterator_cat<InputIterator>::value, int>::type = 0>
  vector(InputIterator first, InputIterator last) {
    range_init(first, last);
  }
  vector(const vector &v) { range_init(v.begin(), v.end()); }
  vector(vector &&v);
  vector(std::initializer_list<value_type> l) {
    range_init(l.begin(), l.end());
  }

  vector &operator=(const vector &);
  vector &operator=(vector &&);
  vector &operator=(std::initializer_list<value_type> l);
  bool operator==(const vector &);
  bool operator!=(const vector &);
  //析构
  ~vector() { destory_deallocate_recover(); }

  //查询
  iterator begin() { return start; }
  iterator end() { return finish; }
  const_iterator begin() const { return start; }
  const_iterator end() const { return finish; }
  const_iterator cbegin() const { return start; }
  const_iterator cend() const { return finish; }
  reverse_iterator rbegin() { return reverse_iterator(finish); }
  const_reverse_iterator crbegin() { return reverse_iterator(finish); }
  reverse_iterator rend() { return reverse_iterator(start); }
  const_reverse_iterator crend() { return reverse_iterator(start); }

  size_type size() const { return end() - begin(); }
  size_type capacity() const { return end_of_storage - begin(); }
  bool empty() const { return begin() == end(); }

  //元素相关
  reference operator[](size_type n) {
    MY_DEBUG(n <= size());
    return *(begin() + n);
  }
  const_reference operator[](size_type n) const {
    MY_DEBUG(n <= size());
    return *(begin() + n);
  }
  reference at(size_type n) {
    THROW_OUT_OF_RANGE_IF(n > size(), "out of range");
    return (*this)[n];
  }
  reference front() {
    MY_DEBUG(!empty());
    return *start;
  }
  reference back() {
    MY_DEBUG(!empty());
    return *(finish - 1);
  }
  pointer data() { return start; }

  //修改容器
  void assign(int, size_type n, const value_type &);
  template <
      typename InputIterator,
      typename std::enable_if<
          tinystl::has_input_iterator_cat<InputIterator>::value, int>::type = 0>
  void assign(InputIterator first, InputIterator last);
  void assign(std::initializer_list<value_type>);
  void push_back(const value_type &);
  void push_back(value_type &&);
  void pop_back();
  template <typename... Args> void emplace_back(Args &&...args);
  template <typename... Args> iterator emplace(iterator, Args &&...args);
  iterator erase(iterator);
  iterator erase(iterator, iterator);
  iterator insert(iterator, size_type, const value_type &);
  iterator insert(iterator iter, const value_type &t) {
    return insert(iter, 1, t);
  };
  iterator insert(iterator iter, value_type &&t) {
    return emplace(iter, std::move(t));
  }
  template <
      typename InputIterator,
      typename std::enable_if<
          tinystl::has_input_iterator_cat<InputIterator>::value, int>::type = 0>
  iterator insert(iterator, InputIterator, InputIterator);
  void clear() { erase(begin(), end()); };
  void resize(size_type n) { resize(n, value_type()); }
  void resize(size_type, const value_type &);
  void swap(vector &);
  void swap(vector &&);
  void reverse(size_type);
  void shrink_to_fit();

  // debug使用
  void display() const {
    for (int i = 0; i < size(); ++i) {
      std::cout << *(begin() + i) << std::endl;
    }
  };
};
template <typename T, typename alloc>
typename vector<T, alloc>::iterator
vector<T, alloc>::allocate_construct_fill(size_type n, const T &t) {
  iterator result = data_allocator::allocate(n);
  std::uninitialized_fill_n(result, n, t);
  return result;
}
template <typename T, typename alloc>
void vector<T, alloc>::fill_init(size_type n, const T &t) {
  try {
    start = allocate_construct_fill(n, t);
    finish = start + n;
    end_of_storage = finish;
  } catch (...) {
    start = nullptr;
    finish = nullptr;
    end_of_storage = nullptr;
  }
}
template <typename T, typename alloc>
void vector<T, alloc>::allocate_push_back(const T &t) {
  const size_type new_size = std::max(size() * 2, static_cast<size_type>(16));
  iterator new_start = data_allocator::allocate(new_size);
  iterator new_finish = tinystl::uninitialized_move(begin(), end(), new_start);
  tinystl::construct(new_finish, t);
  ++new_finish;
  tinystl::destory(begin(), end());
  data_allocator::deallocate(start);
  start = new_start;
  finish = new_finish;
  end_of_storage = start + new_size;
}
template <typename T, typename alloc>
void vector<T, alloc>::push_back(value_type &&t) {
  emplace_back(std::forward<T>(t));
}
template <typename T, typename alloc> void vector<T, alloc>::pop_back() {
  if (!empty()) {
    --finish;
    tinystl::destory(finish);
  }
}

template <typename T, typename alloc>
void vector<T, alloc>::destory_deallocate_recover() {
  tinystl::destory(start, finish);
  data_allocator::deallocate(start);
  start = finish = end_of_storage = nullptr;
}
template <typename T, typename alloc>
template <typename InputIterator,
          typename std::enable_if<
              tinystl::has_input_iterator_cat<InputIterator>::value, int>::type>
void vector<T, alloc>::range_init(InputIterator first, InputIterator last) {
  const size_type distance = tinystl::distance(first, last);
  const size_type size = std::max(distance, static_cast<size_type>(16));
  iterator result = data_allocator::allocate(size);
  finish = std::uninitialized_copy(first, last, result);
  start = result;
  end_of_storage = start + size;
}
template <typename T, typename alloc> vector<T, alloc>::vector(vector &&v) {
  start = v.start;
  finish = v.finish;
  end_of_storage = v.end_of_storage;
  v.start = nullptr;
  v.finish = nullptr;
  v.end_of_storage = nullptr;
}

template <typename T, typename alloc> void vector<T, alloc>::init(size_type n) {
  try {
    start = data_allocator::allocate(n);
    finish = start;
    end_of_storage = start + n;
  } catch (...) {
    start = nullptr;
    finish = nullptr;
    end_of_storage = nullptr;
  }
}
template <typename T, typename alloc>
typename vector<T, alloc>::iterator vector<T, alloc>::erase(iterator iter) {
  if (iter != end() - 1) {
    std::copy(iter + 1, finish, iter);
  }
  --finish;
  tinystl::destory(finish);
  return iter;
}

// destory为什么析构i到finish
template <typename T, typename alloc>
typename vector<T, alloc>::iterator vector<T, alloc>::erase(iterator first,
                                                            iterator last) {
  iterator i = std::copy(last, finish, first);
  tinystl::destory(i, finish);
  finish = i;
  return first;
}
template <typename T, typename alloc>
typename vector<T, alloc>::iterator
vector<T, alloc>::insert(iterator iter, size_type n, const value_type &t) {
  if (n + size() <= capacity()) {
    if (finish - iter > n) {
      tinystl::uninitialized_move(finish - n, finish, finish);
      std::copy_backward(iter, finish - n, finish);
      std::fill(iter, iter + n, t);
      finish += n;
    } else {
      std::uninitialized_fill_n(finish, n - (finish - iter), t);
      tinystl::uninitialized_move(iter, finish, iter + n);
      std::fill(iter, finish, t);
      finish += n;
    }
    return iter + n;
  } else {
    auto len = std::max(size(), n) + size();
    iterator new_start = tinystl::allocator<T>::allocate(len);
    tinystl::uninitialized_move(start, iter, new_start);
    std::uninitialized_fill_n(new_start + (iter - start), n, t);
    iterator new_finish = tinystl::uninitialized_move(
        iter, finish, new_start + (iter - start) + n);
    destory_deallocate_recover();
    start = new_start;
    finish = new_finish;
    end_of_storage = start + len;
    return new_start + (iter - start) + n;
  }
}

template <typename T, typename alloc>
void vector<T, alloc>::resize(size_type n, const value_type &t) {
  if (n < size()) {
    erase(start + n, finish);
  } else {
    insert(finish, n - size(), t);
  }
}

template <typename T, typename alloc>
vector<T, alloc> &vector<T, alloc>::operator=(const vector &v) {
  if (*this != v) {
    const size_type len = v.size();
    if (len > capacity()) {
      vector tmp = v;
      swap(tmp);
    } else if (len < size()) {
      std::copy(v.begin(), v.end(), start);
      tinystl::destory(start + len, finish);
      finish = start + len;
    } else {
      std::copy(v.begin(), v.begin() + size(), start);
      std::uninitialized_copy(v.begin() + size(), v.end(), finish);
      finish = start + len;
    }
  }
  return *this;
}

template <typename T, typename alloc>
bool vector<T, alloc>::operator==(const vector &v) {
  if (size() != v.size()) {
    return false;
  }
  for (int i = 0; i < size(); ++i) {
    if ((*this)[i] != v[i]) {
      return false;
    }
  }
  return true;
}

template <typename T, typename alloc>
bool vector<T, alloc>::operator!=(const vector &v) {
  return !(this->operator==(v));
}
template <typename T, typename alloc>
vector<T, alloc> &vector<T, alloc>::operator=(vector &&v) {
  destory_deallocate_recover();
  start = v.start;
  finish = v.finish;
  end_of_storage = v.end_of_storage;
  v.start = nullptr;
  v.finish = nullptr;
  v.end_of_storage = nullptr;
  return *this;
}

template <typename T, typename alloc>
vector<T, alloc> &vector<T, alloc>::operator=(std::initializer_list<T> l) {
  vector v = l;
  swap(v);
  return *this;
}

template <typename T, typename alloc> void vector<T, alloc>::swap(vector &v) {
  std::swap(start, v.start);
  std::swap(finish, v.finish);
  std::swap(end_of_storage, v.end_of_storage);
}
template <typename T, typename alloc>
void vector<T, alloc>::reverse(size_type n) {
  if (n > capacity()) {
    const size_type len = std::max(n, static_cast<size_type>(16));
    iterator new_start = data_allocator::allocate(len);
    iterator new_finish = std::uninitialized_copy(start, finish, new_start);
    tinystl::destory(start, finish);
    data_allocator::deallocate(start);
    start = new_start;
    finish = new_finish;
    end_of_storage = start + len;
  }
}

template <typename T, typename alloc> void vector<T, alloc>::shrink_to_fit() {
  if (finish < end_of_storage) {
    iterator new_begin = data_allocator::allocate(size());
    // try {
    //   std::uninitialized_(InputIterator first, InputIterator last,
    //                       ForwardIterator result)
    // } catch (declaration) {
    //   statements
    // }
    tinystl::uninitialized_move(start, finish, new_begin);
    data_allocator::deallocate(start);
    start = new_begin;
    finish = start + size();
    end_of_storage = finish;
  }
}

template <typename T, typename alloc>
void vector<T, alloc>::assign(int, size_type n, const value_type &t) {
  if (n > capacity()) {
    vector v(n, t);
    swap(v);
  } else if (n < size()) {
    std::fill_n(start, n, t);
    tinystl::destory(start + n, finish);
    finish = start + n;
  } else {
    std::fill_n(start, n, t);
    std::uninitialized_fill_n(finish, n - size(), t);
    finish = start + n;
  }
}
template <typename T, typename alloc>
template <typename InputIterator,
          typename std::enable_if<
              tinystl::has_input_iterator_cat<InputIterator>::value, int>::type>
void vector<T, alloc>::assign(InputIterator first, InputIterator last) {
  MY_DEBUG(first <= last);
  iterator cur = start;
  for (; first < last && cur < finish; ++first, ++cur) {
    *cur = *first;
  }
  if (cur == finish) {
    insert(finish, first, last);
  } else {
    erase(cur, finish);
  }
}
template <typename T, typename alloc>
template <typename... Args>
void vector<T, alloc>::emplace_back(Args &&...args) {
  if (finish == end_of_storage) {
    const size_type new_size = std::max(static_cast<size_type>(16), 2 * size());
    iterator new_start = data_allocator::allocate(new_size);
    tinystl::uninitialized_move(start, finish, new_start);
    tinystl::construct(new_start + size(), std::forward<Args>(args)...);
    destory_deallocate_recover();
    start = new_start;
    finish = start + size() + 1;
    end_of_storage = start + new_size;
  } else {
    tinystl::construct(finish, std::forward<Args>(args)...);
    ++finish;
  }
}
template <typename T, typename alloc>
void vector<T, alloc>::push_back(const value_type &t) {
  if (finish == end_of_storage) {
    allocate_push_back(t);
  } else {
    tinystl::construct(finish, t);
    ++finish;
  }
}

template <typename T, typename alloc>
template <typename... Args>
typename vector<T, alloc>::iterator vector<T, alloc>::emplace(iterator iter,
                                                              Args &&...args) {
  MY_DEBUG(iter <= finish && iter >= start);
  const size_type n = iter - start;
  if (finish == end_of_storage) {
    const size_type new_size = std::max(static_cast<size_type>(16), 2 * size());
    iterator new_start = data_allocator::allocate(new_size);
    iterator new_iter = tinystl::uninitialized_move(start, iter, new_start);
    tinystl::construct(new_iter, std::forward<Args>(args)...);
    tinystl::uninitialized_move(iter, finish, new_iter + 1);
    destory_deallocate_recover();
    start = new_start;
    finish = start + size() + 1;
    end_of_storage = start + new_size;
  } else if (iter == finish) {
    tinystl::construct(iter, std::forward<Args>(args)...);
    ++finish;

  } else {
    std::uninitialized_copy(finish - 1, finish, finish);
    std::copy_backward(iter, finish - 1, finish);
    *iter = value_type(std::forward<Args>(args)...);
    ++finish;
  }
  return start + n;
}
template <typename T, typename alloc>
template <typename InputIterator,
          typename std::enable_if<
              tinystl::has_input_iterator_cat<InputIterator>::value, int>::type>
typename vector<T, alloc>::iterator
vector<T, alloc>::insert(iterator iter, InputIterator first,
                         InputIterator last) {
  const size_type distance = tinystl::distance(first, last);
  const size_type n = iter - start;
  if (distance + size() > capacity()) {
    const size_type new_size = std::max(static_cast<size_type>(16), distance);
    iterator new_start = data_allocator::allocate(new_size);
    iterator new_finish = tinystl::uninitialized_move(start, iter, new_start);
    new_finish = std::uninitialized_copy(first, last, new_finish);
    new_finish = tinystl::uninitialized_move(iter, finish, new_finish);
    destory_deallocate_recover();
    start = new_start;
    finish = new_finish;
    end_of_storage = start + new_size;
  } else {
    if (distance > finish - iter) {
      tinystl::uninitialized_move(iter, finish, iter + distance);
      std::uninitialized_fill_n(finish, distance - (finish - iter),
                                value_type());
      std::uninitialized_copy(first, last, iter);
      finish += distance;

    } else {
      tinystl::uninitialized_move(finish - distance, finish, finish);
      std::move_backward(iter, finish - distance, finish);
      std::uninitialized_copy(first, last, iter);
      finish += distance;
    }
  }
  return start + n;
}

template <typename T, typename alloc>
void vector<T, alloc>::assign(std::initializer_list<value_type> l) {
  assign(l.begin(), l.end());
}
} // namespace tinystl

#endif