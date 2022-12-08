#ifndef MYTINYSTL_FUNCTIONAL_H_
#define MYTINYSTL_FUNCTIONAL_H_

namespace tinystl {

template <typename Arg1, typename ReturnArg> struct unarg_function {
  typedef Arg1 first_parameter_type;
  typedef ReturnArg return_parameter_type;
};

template <typename Arg1, typename Arg2, typename ReturnArg>
struct binary_function {
  typedef Arg1 first_parameter_type;
  typedef Arg2 second_parameter_type;
  typedef ReturnArg return_parameter_type;
};

// 函数对象：加法
template <class T> struct plus : public binary_function<T, T, T> {
  T operator()(const T &x, const T &y) const { return x + y; }
};

// 函数对象：减法
template <class T> struct minus : public binary_function<T, T, T> {
  T operator()(const T &x, const T &y) const { return x - y; }
};

// 函数对象：乘法
template <class T> struct multiplies : public binary_function<T, T, T> {
  T operator()(const T &x, const T &y) const { return x * y; }
};

// 函数对象：除法
template <class T> struct divides : public binary_function<T, T, T> {
  T operator()(const T &x, const T &y) const { return x / y; }
};

// 函数对象：模取
template <class T> struct modulus : public binary_function<T, T, T> {
  T operator()(const T &x, const T &y) const { return x % y; }
};

// 函数对象：等于
template <class T> struct equal_to : public binary_function<T, T, bool> {
  bool operator()(const T &x, const T &y) const { return x == y; }
};

// 函数对象：不等于
template <class T> struct not_equal_to : public binary_function<T, T, bool> {
  bool operator()(const T &x, const T &y) const { return x != y; }
};

// 函数对象：大于
template <class T> struct greater : public binary_function<T, T, bool> {
  bool operator()(const T &x, const T &y) const { return x > y; }
};

// 函数对象：小于
template <typename T> struct less : binary_function<T, T, bool> {
  bool operator()(const T &x, const T &y) const { return x < y; }
};

// 函数对象：逻辑与
template <class T> struct logical_and : public binary_function<T, T, bool> {
  bool operator()(const T &x, const T &y) const { return x && y; }
};

// 函数对象：逻辑或
template <class T> struct logical_or : public binary_function<T, T, bool> {
  bool operator()(const T &x, const T &y) const { return x || y; }
};

// 函数对象：逻辑非
template <class T> struct logical_not : public unarg_function<T, bool> {
  bool operator()(const T &x) const { return !x; }
};

} // namespace tinystl

#endif