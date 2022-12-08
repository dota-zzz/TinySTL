#ifndef MYTINYSTL_QUEUE_H_
#define MYTINYSTL_QUEUE_H_

#include "functional.h"
#include "heap.h"
#include "vector.h"
namespace tinystl {
template <typename T, typename Container = tinystl::vector<T>,
          typename Compare = tinystl::less<typename Container::value_type>>
class priority_queue {
  typedef Container container_type;
  typedef Compare compare_type;

  typedef typename Container::value_type value_type;
  typedef typename Container::size_type size_type;
  typedef typename Container::reference reference;
  typedef typename Container::const_reference const_reference;

  static_assert(std::is_same<T, value_type>::value,
                "container_value_type is different from T");

private:
  container_type c_;
  compare_type compare_;

public:
  priority_queue() = default;
  priority_queue(const compare_type &compare) : compare_(compare) {}
  explicit priority_queue(size_type n) : c_(n) {
    tinystl::make_heap(c_.begin(), c_.end(), compare_);
  }
  priority_queue(size_type n, const value_type &t) : c_(n, t) {
    tinystl::make_heap(c_.begin(), c_.end(), compare_);
  }
  template <typename InputIterator>
  priority_queue(InputIterator first, InputIterator last) : c_(first, last) {
    tinystl::make_heap(c_.begin(), c_.end(), compare_);
  }
  priority_queue(std::initializer_list<value_type> l) : c_(l.begin(), l.end()) {
    tinystl::make_heap(c_.begin(), c_.end(), compare_);
  }
  priority_queue(const container_type &c) : c_(c) {
    tinystl::make_heap(c_.begin(), c_.end(), compare_);
  }
  priority_queue(container_type &&c) : c_(std::move(c)) {
    tinystl::make_heap(c_.begin(), c_.end(), compare_);
  }
  priority_queue(const priority_queue &p) : c_(p.c_), compare_(p.compare_) {
    tinystl::make_heap(c_.begin(), c_.end(), compare_);
  }
  priority_queue(priority_queue &&p)
      : c_(std::move(p.c_)), compare_(p.compare_) {
    tinystl::make_heap(c_.begin(), c_.end(), compare_);
  }

  priority_queue &operator=(const priority_queue &p) {
    c_ = p.c_;
    compare_ = p.compare_;
    tinystl::make_heap(c_.begin(), c_.end(), compare_);
    return *this;
  }
  priority_queue &operator=(priority_queue &&p) {
    c_ = std::move(p.c_);
    compare_ = p.compare_;
    tinystl::make_heap(c_.begin(), c_.end(), compare_);
    return *this;
  }
  priority_queue &operator=(std::initializer_list<value_type> l) {
    c_ = l;
    tinystl::make_heap(c_.begin(), c_.end(), compare_);
    return *this;
  }

  ~priority_queue() = default;

  bool empty() const noexcept { return c_.empty(); }
  size_type size() const noexcept { return c_.size(); }

  const_reference top() const { return c_.front(); }

  template <typename... Args> void emplace(Args... args) {
    c_.emplace_back(std::forward<Args>(args)...);
    tinystl::push_heap(c_.begin(), c_.end(), compare_);
  }
  void push(const value_type &t) {
    c_.push_back(t);
    tinystl::push_heap(c_.begin(), c_.end(), compare_);
  }
  void push(value_type &&t) {
    c_.push_back(std::move(t));
    tinystl::push_heap(c_.begin(), c_.end(), compare_);
  }
  void pop() {
    tinystl::pop_heap(c_.begin(), c_.end(), compare_);
    c_.pop_back();
  }
  void clear() {
    while (!empty()) {
      pop();
    }
  }
};
} // namespace tinystl

#endif