#ifndef MYTINYSTL_TYPE_TRAITS_H_
#define MYTINYSTL_TYPE_TRAITS_H_

#include <type_traits>

namespace tinystl {
template <typename T, T t> struct integral_constant {
  static constexpr T value = t;
};

template <bool b> using bool_constant = integral_constant<bool, b>;

template <class T1, class T2> struct pair;
// --- forward declaration end

template <class T> struct is_pair : std::false_type {};

template <class T1, class T2>
struct is_pair<tinystl::pair<T1, T2>> : std::true_type {};

} // namespace tinystl

#endif