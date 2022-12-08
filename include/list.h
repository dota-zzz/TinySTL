#ifndef MYTINYSTL_LIST_H
#define MYTINYSTL_LIST_H
#include "allocator.h"
#include "exceptdef.h"
#include "iterator.h"
#include <cstddef>
#include <initializer_list>
#include <type_traits>
#include <utility>

namespace tinystl {
template <typename T> struct list_node_base;
template <typename T> struct list_node;

template <typename T> struct list_node_base {
public:
  typedef list_node_base<T> *base_ptr;
  typedef list_node<T> *node_ptr;

  base_ptr pre;
  base_ptr next;

  list_node_base() = default;

  node_ptr as_node() { return static_cast<node_ptr>(&*this); }
  void un_link() {
    pre = self();
    next = self();
  }
  base_ptr self() { return &*this; }
};

template <typename T> struct list_node : public list_node_base<T> {
public:
  typedef list_node_base<T> *base_ptr;
  typedef list_node<T> *node_ptr;

  T value;

  list_node() = default;
  list_node(const T &t) : value(t) {}
  list_node(T &&t) : value(std::move(t)) {}

  base_ptr as_base() { return static_cast<base_ptr>(&*this); }
  node_ptr self() { return static_cast<node_ptr>(&(this)); }
};

template <typename T>
struct list_iterator
    : public tinystl::iterator<tinystl::bidirectional_iterator_tag, T> {

public:
  typedef list_node_base<T> *base_ptr;
  typedef list_node<T> *node_ptr;
  typedef T value_type;
  typedef T &reference;
  typedef T *pointer;
  typedef list_iterator<T> self;

  base_ptr node_;

  list_iterator();
  list_iterator(base_ptr t) : node_(t) {}
  list_iterator(node_ptr t) : node_(t->as_base()) {}
  list_iterator(const list_iterator &t) : node_(t.node_){};

  reference operator*() const { return node_->as_node()->value; }
  pointer operator->() const { return &*(*this); }

  self &operator++() {
    MY_DEBUG(node_ != nullptr);
    node_ = node_->next;
    return *this;
  }
  self operator++(int) {
    self tmp = *this;
    node_ = node_->next;
    return tmp;
  }
  self &operator--() {
    MY_DEBUG(node_ != nullptr);
    node_ = node_->pre;
    return *this;
  }
  self operator--(int) {
    self tmp = *this;
    node_ = node_->pre;
    return tmp;
  }

  bool operator==(const self &t) const { return node_ == t.node_; }
  bool operator!=(const self &t) const { return node_ != t.node_; }
};

template <typename T>
struct list_const_iterator
    : public tinystl::iterator<tinystl::bidirectional_iterator_tag, T> {

public:
  typedef list_node_base<T> *base_ptr;
  typedef list_node<T> *node_ptr;
  typedef T value_type;
  typedef const T &reference;
  typedef const T *pointer;
  typedef list_const_iterator<T> self;

  base_ptr node_;

  list_const_iterator();
  list_const_iterator(base_ptr t) : node_(t) {}
  list_const_iterator(node_ptr t) : node_(t->as_base()) {}
  list_const_iterator(const list_iterator<T> &t) : node_(t.node_){};
  list_const_iterator(const list_const_iterator &t) : node_(t.node_){};

  reference operator*() const { return node_->as_node()->value; }
  pointer operator->() const { return &*(*this); }

  self &operator++() {
    MY_DEBUG(node_ != nullptr);
    node_ = node_->next;
    return *this;
  }
  self operator++(int) {
    self tmp = *this;
    node_ = node_->next;
    return tmp;
  }
  self &operator--() {
    MY_DEBUG(node_ != nullptr);
    node_ = node_->pre;
    return *this;
  }
  self operator--(int) {
    self tmp = *this;
    node_ = node_->pre;
    return tmp;
  }

  bool operator==(const self &t) const { return node_ == t.node_; }
  bool operator!=(const self &t) const { return node_ != t.node_; }
};

template <typename T> class list {
public:
  typedef tinystl::allocator<T> data_allocator;
  typedef tinystl::allocator<list_node_base<T>> base_allocator;
  typedef tinystl::allocator<list_node<T>> node_allocator;

  typedef list_node_base<T> *base_ptr;
  typedef list_node<T> *node_ptr;

  typedef T value_type;
  typedef T *pointer;
  typedef T &reference;
  typedef std::ptrdiff_t difference_type;
  typedef std::size_t size_type;
  typedef list<T> self;
  typedef list<T> &self_reference;

  typedef list_iterator<T> iterator;
  typedef list_const_iterator<T> const_iterator;

  typedef tinystl::reverse_iterator<iterator> reverse_iterator;
  typedef tinystl::reverse_iterator<const_iterator> const_reverse_iterator;

private:
  base_ptr node_;
  size_type size_;

protected:
  template <typename... Args> node_ptr create_node(Args... args);
  void clear();
  void destory_node(node_ptr);
  void link_at_end(base_ptr, base_ptr);
  void link_at_front(base_ptr, base_ptr);
  void link_at_pos(const_iterator &, base_ptr, base_ptr);
  void fill_init(size_type n, const value_type &);
  template <typename InputIterator>
  void range_init(InputIterator first, InputIterator last);
  void fill_assign(size_type n, const value_type &);
  template <typename InputIterator>
  void range_assign(InputIterator first, InputIterator last);
  iterator fill_insert(const_iterator &pos, size_type n, const value_type &);
  template <typename InputIterator>
  iterator range_insert(const_iterator &pos, InputIterator first,
                        InputIterator last);
  void unlink_nodes(base_ptr, base_ptr);

public:
  list() { fill_init(0, value_type()); }
  ~list() noexcept {
    clear();
    base_allocator::deallocate(node_);
  }
  explicit list(size_type n) { fill_init(n, value_type()); }
  list(size_type n, const value_type &t) { fill_init(n, t); }
  list(const list &l) { range_init(l.begin(), l.end()); }
  list(std::initializer_list<value_type> l) { range_init(l.begin(), l.end()); }
  list(list &&l) noexcept : node_(l.node_), size_(l.size_) {
    l.node_ = nullptr;
    l.size_ = 0;
  }

  self_reference operator=(const list &l) { range_assign(l.begin(), l.end()); }
  self_reference operator=(list &&l) noexcept {
    node_ = l.node_;
    l.node_ = nullptr;
    size_ = l.size_;
    l.size_ = 0;
  }
  self_reference operator=(std::initializer_list<value_type> l) {
    range_assign(l.begin(), l.end());
  }

  // iterator

  iterator begin() noexcept { return node_->next; }
  iterator end() noexcept { return node_; }
  const_iterator begin() const noexcept { return node_->next; }
  const_iterator end() const noexcept { return node_; }

  reverse_iterator rbegin() noexcept { return reverse_iterator(node_); }
  reverse_iterator rend() noexcept { return reverse_iterator(node_->next); }
  const_reverse_iterator rbegin() const noexcept {
    return reverse_iterator(node_);
  }
  const_reverse_iterator rend() const noexcept {
    return reverse_iterator(node_->next);
  }

  bool empty() const noexcept { return node_->next == node_; }
  size_type size() const noexcept { return size_; }
  size_type max_size() const noexcept { return static_cast<size_type>(-1); }

  // assign
  // void assign() {}

  // erase

  iterator erase(const_iterator &first, const_iterator &last);

  // insert

  iterator insert(const_iterator &pos, const value_type &t) {
    THROW_OUT_OF_RANGE_IF(1 + size_ > max_size(), "out of maxsize");
    return fill_insert(pos, 1, t);
  }

  iterator insert(const_iterator &pos, size_type n, const value_type &t) {
    THROW_OUT_OF_RANGE_IF(n + size_ > max_size(), "out of maxsize");
    return fill_insert(pos, n, t);
  }

  iterator insert(const_iterator &pos, size_type n) {
    THROW_OUT_OF_RANGE_IF(n + size_ > max_size(), "out of maxsize");
    return fill_insert(pos, n, value_type());
  }

  iterator insert(const_iterator &pos, value_type &&t) {
    THROW_OUT_OF_RANGE_IF(1 + size_ > max_size(), "out of maxsize");
    base_ptr node = create_node(std::move(t));
    link_at_pos(pos, node, node);
    ++size_;
    return iterator(pos.node_);
  }

  template <
      typename InputIterator,
      typename std::enable_if<
          tinystl::has_input_iterator_cat<InputIterator>::value, int>::type = 0>
  iterator insert(const_iterator &pos, InputIterator first,
                  InputIterator last) {
    MY_DEBUG(first <= last);
    return range_insert(pos, first, last);
  }
};
template <typename T>
template <typename... Args>
typename list<T>::node_ptr list<T>::create_node(Args... args) {
  node_ptr p = node_allocator::allocate(1);
  try {
    tinystl::construct(&(p->value), std::forward<Args>(args)...);
    p->next = nullptr;
    p->pre = nullptr;
  } catch (...) {
    destory_node(p);
    throw;
  }
  return p;
}
template <typename T> void list<T>::destory_node(node_ptr p) {
  tinystl::destory(&(p->value));
  node_allocator::deallocate(p);
}
template <typename T> void list<T>::link_at_end(base_ptr first, base_ptr last) {
  node_->pre->next = first;
  first->pre = node_->pre;
  node_->pre = last;
  last->next = node_;
}
template <typename T>
void list<T>::link_at_front(base_ptr first, base_ptr last) {
  last->next = node_->next;
  node_->next->pre = last;
  node_->next = first;
  first->pre = node_;
}
template <typename T> void list<T>::clear() {
  if (size_ != 0) {
    base_ptr cur = node_->next;
    for (base_ptr nex = cur->next; cur != node_; cur = nex, nex = nex->next) {
      destory_node(cur->as_node());
    }
    node_->un_link();
    size_ = 0;
  }
}

template <typename T>
void list<T>::fill_init(size_type n, const value_type &t) {
  node_ = base_allocator::allocate(1);
  node_->un_link();
  size_ = n;
  try {
    for (; n > 0; --n) {
      base_ptr node = create_node(t);
      link_at_end(node, node);
    }
  } catch (...) {
    clear();
    base_allocator::deallocate(node_);
    node_ = nullptr;
    size_ = 0;
    throw;
  }
}

template <typename T>
template <typename InputIterator>
void list<T>::range_init(InputIterator first, InputIterator last) {
  size_ = tinystl::distance(first, last);
  node_ = base_allocator::allocate(1);
  node_->un_link();
  try {
    while (first++ != last) {
      base_ptr node = create_node(*first);
      link_at_end(node, node);
    }
  } catch (...) {
    clear();
    base_allocator::deallocate(node_);
    node_ = nullptr;
    size_ = 0;
  }
}
template <typename T>
template <typename InputIterator>
typename list<T>::iterator list<T>::range_insert(const_iterator &pos,
                                                 InputIterator first,
                                                 InputIterator last) {
  size_type n = tinystl::distance(first, last);
  THROW_OUT_OF_RANGE_IF(n + size_ > max_size(), "out of maxsize");
  if (n > 0) {
    base_ptr head = create_node(*first);
    base_ptr tail = head;
    try {
      while (++first != last) {
        base_ptr node = create_node(*first);
        tail->next = node;
        node->pre = tail;
        tail = node;
      }
    } catch (...) {
      base_ptr cur = head;
      while (cur != tail) {
        cur = cur->next;
        destory_node(head->as_node());
        head = cur;
      }
      destory_node(tail->as_node());
      throw;
    }
    link_at_pos(pos, head, tail);
    size_ += n;
  }
  return iterator(pos.node_);
}
template <typename T>
void list<T>::link_at_pos(const_iterator &pos, base_ptr first, base_ptr last) {
  base_ptr &node = pos.node_;
  first->pre = node->pre;
  node->pre->next = first;
  last->next = node;
  node->pre = last;
}
template <typename T>
template <typename InputIterator>
void list<T>::range_assign(InputIterator first, InputIterator last) {
  iterator cur = begin();
  for (; cur != end(), first != last; ++cur, ++first) {
    *cur = *first;
  }
  if (first == last) {
    erase(cur, end());
  } else {
    insert(cur, first, last);
  }
}

template <typename T>
typename list<T>::iterator
list<T>::fill_insert(const_iterator &pos, size_type n, const value_type &t) {
  if (n > 0) {
    base_ptr head = create_node(t);
    base_ptr tail = head;
    --n;
    try {
      for (; n > 0; --n) {
        base_ptr node = create_node(t);
        tail->next = node;
        node->pre = tail;
        tail = node;
      }
    } catch (...) {
      base_ptr cur = head;
      while (cur != tail) {
        cur = cur->next;
        destory_node(head->as_node());
        head = cur;
      }
      destory_node(tail->as_node());
      throw;
    }
    link_at_pos(pos, head, tail);
    size_ += n;
    return iterator(pos.node_);
  }
}
template <typename T>
void list<T>::unlink_nodes(base_ptr first, base_ptr last) {
  first->pre->next = last->next;
  last->next->pre = first->pre;
}
template <typename T>
typename list<T>::iterator list<T>::erase(const_iterator &first,
                                          const_iterator &last) {
  if (first != last) {
    unlink_nodes(first.node_, last.node_->pre);
    while (first != last) {
      ++first;
      destory_node(first.node_->pre->as_node());
      --size_;
    }
  }
  return iterator(last.node_);
}
} // namespace tinystl

#endif
