#ifndef MYTINYSTL_ITERATOR_H_
#define MYTINYSTL_ITERATOR_H_

#include "type_traits.h"
#include <cstddef>
#include <type_traits>

namespace tinystl {
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

template <typename Category, typename T, typename Distance = std::ptrdiff_t,
          typename Pointer = T *, typename Reference = T &>
struct iterator {
  typedef Category iterator_category;
  typedef T value_type;
  typedef Distance difference_type;
  typedef Pointer pointer;
  typedef Reference reference;
};

template <typename T> struct has_iterator_cat {
private:
  struct two {
    char a;
    char b;
  };
  template <typename U> static two test(...);
  template <typename U>
  static char test(typename U::iterator_category * = nullptr);

public:
  static constexpr bool value = sizeof(test<T>(nullptr)) == sizeof(char);
};

template <typename T, bool> struct iterator_traits_impl {};
template <typename T> struct iterator_traits_impl<T, true> {
  typedef typename T::iterator_category iterator_category;
  typedef typename T::value_type value_type;
  typedef typename T::difference_type difference_type;
  typedef typename T::pointer pointer;
  typedef typename T::reference reference;
};

template <typename T, bool> struct iterator_traits_helper {};
template <typename T>
struct iterator_traits_helper<T, true>
    : public iterator_traits_impl<
          T, std::is_convertible<typename T::iterator_category,
                                 input_iterator_tag>::value ||
                 std::is_convertible<typename T::iterator_category,
                                     output_iterator_tag>::value> {};

template <typename T>
struct iterator_traits
    : public iterator_traits_helper<T, tinystl::has_iterator_cat<T>::value> {};
// template <typename InputIterator> struct iterator_traits {

// };

template <typename T> struct iterator_traits<T *> {
  typedef random_access_iterator_tag iterator_category;
  typedef T value_type;
  typedef std::ptrdiff_t difference_type;
  typedef T *pointer;
  typedef T &reference;
};

template <typename T> struct iterator_traits<const T *> {
  typedef random_access_iterator_tag iterator_category;
  typedef T value_type;
  typedef std::ptrdiff_t difference_type;
  typedef const T *pointer;
  typedef const T &reference;
};

template <typename Iter, typename U,
          bool = has_iterator_cat<iterator_traits<Iter>>::value>
struct has_iterator_cat_of
    : public tinystl::bool_constant<std::is_convertible<
          typename tinystl::iterator_traits<Iter>::iterator_category,
          U>::value> {};
template <typename Iter, typename U>
struct has_iterator_cat_of<Iter, U, false>
    : public tinystl::bool_constant<false> {};
template <typename Iter>
struct has_input_iterator_cat
    : public has_iterator_cat_of<Iter, input_iterator_tag> {};

template <typename Iter>
struct has_output_iterator_cat
    : public has_iterator_cat_of<Iter, output_iterator_tag> {};
template <typename Iter>
struct has_forward_iterator_cat
    : public has_iterator_cat_of<Iter, forward_iterator_tag> {};
template <typename Iter>
struct has_bidirectional_iterator_cat
    : public has_iterator_cat_of<Iter, bidirectional_iterator_tag> {};
template <typename Iter>
struct has_random_access_iterator_cat
    : public has_iterator_cat_of<Iter, random_access_iterator_tag> {};

template <typename iterator> class reverse_iterator {
public:
  typedef
      typename iterator_traits<iterator>::iterator_category iterator_category;
  typedef typename iterator_traits<iterator>::value_type value_type;
  typedef typename iterator_traits<iterator>::difference_type difference_type;
  typedef typename iterator_traits<iterator>::pointer pointer;
  typedef typename iterator_traits<iterator>::reference reference;

private:
  iterator current;

public:
  iterator base() const { return current; }
  reverse_iterator() : current(nullptr) {}
  reverse_iterator(iterator iter) : current(iter) {}
  reverse_iterator(const reverse_iterator &riter) : current(riter.current) {}

public:
  reference operator*() { return *(current - 1); }
  pointer operator->() { return &*(current - 1); }
  reverse_iterator &operator++() {
    --current;
    return *this;
  }
  reverse_iterator &operator++(int) {
    reverse_iterator tmp = *this;
    --current;
    return tmp;
  }
  reverse_iterator &operator--() {
    ++current;
    return *this;
  }
  reverse_iterator &operator--(int) {
    reverse_iterator tmp = *this;
    ++current;
    return tmp;
  }
  reverse_iterator operator+(difference_type n) {
    current -= n;
    return *this;
  }
  reverse_iterator operator-(difference_type n) {
    current += n;
    return *this;
  }
  reverse_iterator &operator+=(difference_type n) {
    current -= n;
    return *this;
  }
  reverse_iterator &operator-=(difference_type n) {
    current += n;
    return *this;
  }
};

template <typename iterator>
typename reverse_iterator<iterator>::difference_type
operator-(const reverse_iterator<iterator> &iter1,
          const reverse_iterator<iterator> &iter2) {
  return iter2.base() - iter1.base();
}

template <typename iterator>
bool operator==(const reverse_iterator<iterator> &iter1,
                const reverse_iterator<iterator> &iter2) {
  return iter1.base() == iter2.base();
}

template <typename iterator>
bool operator!=(const reverse_iterator<iterator> &iter1,
                const reverse_iterator<iterator> &iter2) {
  return iter1.base() != iter2.base();
}

template <typename iterator>
bool operator>(const reverse_iterator<iterator> &iter1,
               const reverse_iterator<iterator> &iter2) {
  return iter1.base() < iter2.base();
}

template <typename iterator>
bool operator>=(const reverse_iterator<iterator> &iter1,
                const reverse_iterator<iterator> &iter2) {
  return iter1.base() <= iter2.base();
}
template <typename iterator>
bool operator<(const reverse_iterator<iterator> &iter1,
               const reverse_iterator<iterator> &iter2) {
  return iter1.base() > iter2.base();
}
template <typename iterator>
bool operator<=(const reverse_iterator<iterator> &iter1,
                const reverse_iterator<iterator> &iter2) {
  return iter1.base() >= iter2.base();
}

template <typename InputIterator>
typename iterator_traits<InputIterator>::difference_type
distance_patch(InputIterator first, InputIterator last,
               random_access_iterator_tag) {
  return last - first;
}
template <typename InputIterator>
typename iterator_traits<InputIterator>::difference_type
distance_patch(InputIterator first, InputIterator last, input_iterator_tag) {
  typename iterator_traits<InputIterator>::difference_type n = 0;
  while (first != last) {
    ++first;
    ++n;
  }
  return n;
}
template <typename InputIterator>
typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last) {
  return distance_patch(
      first, last,
      typename iterator_traits<InputIterator>::iterator_category{});
}

template <typename InputIterator>
typename iterator_traits<InputIterator>::iterator_category
iterator_category(const InputIterator &) {
  typedef typename iterator_traits<InputIterator>::iterator_category Category;
  return Category();
}

template <typename InputIterator>
typename iterator_traits<InputIterator>::difference_type *
distance_type(const InputIterator &) {
  return static_cast<
      typename iterator_traits<InputIterator>::difference_type *>(0);
}

template <typename InputIterator, typename Distance>
void advance_patch(InputIterator &i, Distance n, input_iterator_tag) {
  while (n--) {
    ++i;
  }
}
template <typename InputIterator, typename Distance>
void advance_patch(InputIterator &i, Distance n, bidirectional_iterator_tag) {
  if (n >= 0)
    while (n--)
      ++i;
  else
    while (n++)
      --i;
}
template <typename InputIterator, typename Distance>
void advance_patch(InputIterator &i, Distance n, random_access_iterator_tag) {
  i += n;
}

template <typename InputIterator, typename Distance>
void advance(InputIterator &i, Distance n) {
  advance_patch(i, n, iterator_category(i));
}

} // namespace tinystl
#endif