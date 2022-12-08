#ifndef MYTINYSTL_RB_TREE_H_
#define MYTINYSTL_RB_TREE_H_

#include "allocator.h"
#include "iterator.h"
#include "type_traits.h"
#include <type_traits>

// static constexpr rb_tree_color_type re_tree_red = false;

namespace tinystl {
typedef bool rb_tree_color_type;

static constexpr rb_tree_color_type rb_tree_red = false;
static constexpr rb_tree_color_type rb_tree_black = true;

template <typename T> struct rb_tree_node_base;
template <typename T> struct rb_tree_node;
template <typename T> struct rb_tree_iterator;
template <typename T> struct rb_tree_const_iterator;

template <typename T, bool> struct rb_tree_node_traits_imp {
  typedef T value_type;
  typedef T key_type;
  typedef T mapped_type;
};

template <typename T> struct rb_tree_node_traits_imp<T, true> {
  typedef T value_type;
  typedef typename std::remove_cv<typename T::first_type>::type key_type;
  typedef typename T::second_type mapped_type;
};

template <typename T> struct rb_tree_value_traits {
  static constexpr bool is_map = tinystl::is_pair<T>::value;
  typedef rb_tree_node_traits_imp<T, is_map> value_traits;
  typedef typename value_traits::value_type value_type;
  typedef typename value_traits::key_type key_type;
  typedef typename value_traits::mapped_type mapped_type;
};

template <class T> struct rb_tree_traits {
  typedef rb_tree_color_type color_type;

  typedef rb_tree_value_traits<T> value_traits;
  typedef typename value_traits::key_type key_type;
  typedef typename value_traits::mapped_type mapped_type;
  typedef typename value_traits::value_type value_type;
  typedef value_type *pointer;
  typedef value_type &reference;
  typedef const value_type *const_pointer;
  typedef const value_type &const_reference;

  typedef rb_tree_node_base<T> *base_ptr;
  typedef rb_tree_node<T> *node_ptr;
};

template <typename T> struct rb_tree_node_base {
  typedef rb_tree_color_type color_type;
  typedef rb_tree_node_base<T> *base_ptr;
  typedef rb_tree_node<T> *node_ptr;

  base_ptr parent;
  base_ptr left;
  base_ptr right;
  color_type color;

  base_ptr get_base_ptr() { return &*this; }
  node_ptr get_node_ptr() { return static_cast<node_ptr>(&*this); }
  node_ptr &get_node_ref() { return static_cast<node_ptr &>(&*this); }
};

template <typename T> struct rb_tree_node : public rb_tree_node_base<T> {
  typedef rb_tree_node_base<T> *base_ptr;
  typedef rb_tree_node<T> *node_ptr;

  T value;

  base_ptr get_base_ptr() { return static_cast<base_ptr>(&*this); }
  node_ptr get_node_ptr() { return &*this; }
};

template <typename T>
struct rb_tree_iterator_base
    : tinystl::iterator<bidirectional_iterator_tag, T> {
  typedef rb_tree_node_base<T> *base_ptr;

  base_ptr node;

  rb_tree_iterator_base() : node(nullptr) {}
  void inc() {
    if (node->right != nullptr) {
      node = rb_tree_min(node->left);
    } else {
      auto p = node->parent;
      while (p->right == node) {
        node = p;
        p = p->parent;
      }
      if (node->right != p) {
        node = p; // 应对根节点无右节点执行inc的情况
      }
    }
  }
  void dec() {
    if (node->parent->parent == node && rb_tree_is_red(node)) {
      node = node->right;
    } else if (node->left != nullptr) {
      node = rb_tree_max(node->left);
    } else {
      auto p = node->parent;
      while (p->left == node) {
        node = p;
        p = p->parent;
      }
      node = p;
    }
  }

  bool operator==(const rb_tree_iterator_base<T> &t) { return node == t.node; }
  bool operator!=(const rb_tree_iterator_base<T> &t) { return node != t.node; }
};

template <typename T>
struct rb_tree_iterator : public rb_tree_iterator_base<T> {
  typedef rb_tree_node_base<T> *base_ptr;
  typedef rb_tree_node<T> *node_ptr;
  typedef rb_tree_iterator<T> self;
  typedef rb_tree_traits<T> tree_traits;
  typedef typename tree_traits::value_type value_type;
  typedef typename tree_traits::reference reference;
  typedef typename tree_traits::pointer pointer;

  using rb_tree_iterator_base<T>::node;

  rb_tree_iterator() {}
  rb_tree_iterator(base_ptr ptr) { node = ptr; }
  rb_tree_iterator(node_ptr ptr) { node = ptr->get_base_ptr(); }
  rb_tree_iterator(const rb_tree_iterator &t) { node = t.node; }
  rb_tree_iterator(const rb_tree_const_iterator<T> &t) { node = t.node; }

  self &operator++() {
    node->inc();
    return *this;
  }
  self &operator++(int) {
    auto tmp = *this;
    node->inc();
    return tmp;
  }
  self &operator--() {
    node->dec();
    return *this;
  }
  self &operator--(int) {
    auto tmp = *this;
    node->dec();
    return tmp();
  }
  reference operator*() const { return node->get_node_ptr()->value; }
  pointer operator->() const { return &*(*this); }
};

template <typename T> struct rb_tree_const_iterator : rb_tree_iterator_base<T> {
  typedef rb_tree_node_base<T> *base_ptr;
  typedef rb_tree_node<T> *node_ptr;
  typedef rb_tree_const_iterator<T> self;
  typedef rb_tree_traits<T> tree_traits;
  typedef typename tree_traits::const_reference const_reference;
  typedef typename tree_traits::const_pointer const_pointer;

  using rb_tree_iterator_base<T>::node;
  rb_tree_const_iterator(base_ptr ptr) { node = ptr; }
  rb_tree_const_iterator(node_ptr ptr) { node = ptr->get_base_ptr(); }
  rb_tree_const_iterator(const rb_tree_iterator<T> &t) { node = t.node; }
  rb_tree_const_iterator(const rb_tree_const_iterator<T> &t) { node = t.node; }
  self &operator++() {
    node->inc();
    return *this;
  }
  self &operator++(int) {
    auto tmp = *this;
    node->inc();
    return tmp;
  }
  self &operator--() {
    node->dec();
    return *this;
  }
  self &operator--(int) {
    auto tmp = *this;
    node->dec();
    return tmp();
  }
  const_reference operator*() const { return node->get_node_ptr()->value; }
  const_pointer operator->() const { return &*(*this); }
};

template <typename Nodeptr> Nodeptr rb_tree_max(Nodeptr ptr) {
  while (ptr->right != nullptr) {
    ptr = ptr->right;
  }
  return ptr;
}
template <typename Nodeptr> Nodeptr rb_tree_min(Nodeptr ptr) {
  while (ptr->left != nullptr) {
    ptr = ptr->left;
  }
  return ptr;
}
template <typename Nodeptr> bool rb_tree_is_red(Nodeptr ptr) {
  return ptr->color == rb_tree_red;
}
template <typename Nodeptr> void rb_tree_set_black(Nodeptr ptr) {
  ptr->color = rb_tree_black;
}
template <typename Nodeptr> void rb_tree_set_red(Nodeptr ptr) {
  ptr->color = rb_tree_red;
}
template <typename Nodeptr> bool rb_tree_is_lchild(Nodeptr ptr) {
  return ptr->parent->left == ptr;
}
template <typename Nodeptr>
void rb_tree_left_rotate(Nodeptr ptr, Nodeptr &root) {
  auto y = ptr->right;
  ptr->right = y->left;
  if (y->left != nullptr) {
    y->left->parent = ptr;
  }
  y->parent = ptr->parent;
  if (ptr == root) {
    root = y;
  } else if (rb_tree_is_lchild(ptr)) {
    y->parent->left = y;
  } else {
    y->parent->right = y;
  }
  y->left = ptr;
  ptr->parent = y;
}
template <typename Nodeptr>
void rb_tree_right_rotate(Nodeptr ptr, Nodeptr &root) {
  auto y = ptr->left;
  ptr->right = y->left;
  if (y->left != nullptr) {
    y->left->parent = ptr;
  }
  y->parent = ptr->parent;
  if (ptr == root) {
    root == y;
  } else if (rb_tree_is_lchild(ptr)) {
    y->parent->left = y;
  } else {
    y->parent->right = y;
  }
  y->right = ptr;
  ptr->parent = y;
}

template <typename Nodeptr>
void rb_tree_insert_rebalance(Nodeptr ptr, Nodeptr &root) {
  rb_tree_set_red(ptr);
  while (ptr != root && rb_tree_is_red(ptr->parent)) {
    if (rb_tree_is_lchild(ptr->parent)) {
      // 父节点是祖父节点的左节点
      auto uncle = ptr->parent->parent->right;
      if (uncle != nullptr && rb_tree_is_red(uncle)) {
        rb_tree_set_black(ptr->parent);
        rb_tree_set_black(uncle);
        ptr = ptr->parent->parent;
        rb_tree_set_red(ptr);
      } else {
        if (rb_tree_is_lchild(ptr)) {
          ptr = ptr->parent;
          rb_tree_left_rotate(ptr, root);
        }
        rb_tree_set_black(ptr->parent);
        rb_tree_set_red(ptr->parent->parent);
        rb_tree_right_rotate(ptr->parent->parent);
        break;
      }
    } else {
      // 父节点是祖父节点的右节点
      auto uncle = ptr->parent->parent->left;
      if (uncle != nullptr && rb_tree_is_red(uncle)) {
        rb_tree_set_black(ptr->parent);
        rb_tree_set_black(uncle);
        ptr = ptr->parent->parent;
        rb_tree_set_red(ptr);
      } else {
        if (!rb_tree_is_lchild(ptr)) {
          ptr = ptr->parent;
          rb_tree_right_rotate(ptr, root);
        }
        rb_tree_set_black(ptr->parent);
        rb_tree_set_red(ptr->parent->parent);
        rb_tree_left_rotate(ptr->parent->parent);
        break;
      }
    }
  }
  rb_tree_set_black(root);
}

template <class NodePtr> NodePtr rb_tree_next(NodePtr node) noexcept {
  if (node->right != nullptr)
    return rb_tree_min(node->right);
  while (!rb_tree_is_lchild(node))
    node = node->parent;
  return node->parent;
}

template <typename Nodeptr>
Nodeptr rb_tree_erase_rebalance(Nodeptr ptr, Nodeptr &root, Nodeptr &leftmost,
                                Nodeptr &rightmost) {
  auto y =
      (ptr->left != nullptr || ptr->right != nullptr) ? ptr : rb_tree_next(ptr);
  auto x = (y->left != nullptr) ? y->left : y->right;
  Nodeptr xp = nullptr;
  if (y != ptr) {
    y->left = ptr->left;
    ptr->left->parent = y;
    if (ptr->right == y) {
      xp = y;
    } else {
      xp = y->parent;
      y->parent->left = x;
      if (x != nullptr) {
        x->parent = y->parent;
      }
      y->right = ptr->right;
      ptr->right->parent = y;
    }

    y->parent = ptr->parent;
    if (ptr == root) {
      root = y;
    } else {
      if (rb_tree_is_lchild(ptr)) {
        ptr->parent->left = y;
      } else {
        ptr->parent->right = y;
      }
    }
    std::swap(y->color, ptr->color);
    y = ptr;
  } else {
    xp = y->parent;
    if (y == root) {
      root = x;
    } else {
      if (rb_tree_is_lchild(y)) {
        y->parent->left = x;
      } else {
        y->parent->right = x;
      }
    }
    if (x != nullptr) {
      x->parent = y->parent;
    }

    if (leftmost == y) {
      leftmost = x == nullptr ? xp : rb_tree_min(x);
    }
    if (rightmost == y) {
      rightmost = x == nullptr ? xp : rb_tree_max(x);
    }
  }

  if (!rb_tree_is_red(y)) {
    while (x != root && (x == nullptr || !rb_tree_is_red(x))) {
      if (x == xp->left) {
        auto bro = xp->right;
        if (rb_tree_is_red(bro)) {
          rb_tree_set_red(xp);
          rb_tree_set_black(bro);
          rb_tree_left_rotate(xp, root);
          bro = xp->right;
        }
        if ((bro->left == nullptr || !rb_tree_is_red(bro->left)) &&
            (bro->right == nullptr || !rb_tree_is_red(bro->right))) {
          rb_tree_set_red(bro);
          x = xp;
          xp = xp->parent;
        } else {
          if ((bro->right == nullptr || !rb_tree_is_red(bro->right))) {
            rb_tree_set_red(bro);
            if (bro->left != nullptr) {
              rb_tree_set_black(bro->left);
            }
            rb_tree_right_rotate(bro, root);
            bro = xp->right;
          }
          bro->color = xp->color;
          rb_tree_set_black(xp);
          if (bro->right != nullptr) {
            rb_tree_set_balck(bro->right);
          }
          rb_tree_left_rotate(xp, root);
          break;
        }
      } else // x 为右子节点，对称处理
      {
        auto bro = xp->left;
        if (rb_tree_is_red(bro)) { // case 1
          rb_tree_set_black(bro);
          rb_tree_set_red(xp);
          rb_tree_right_rotate(xp, root);
          bro = xp->left;
        }
        if ((bro->left == nullptr || !rb_tree_is_red(bro->left)) &&
            (bro->right == nullptr || !rb_tree_is_red(bro->right))) { // case 2
          rb_tree_set_red(bro);
          x = xp;
          xp = xp->parent;
        } else {
          if (bro->left == nullptr || !rb_tree_is_red(bro->left)) { // case 3
            if (bro->right != nullptr)
              rb_tree_set_black(bro->right);
            rb_tree_set_red(bro);
            rb_tree_left_rotate(bro, root);
            bro = xp->left;
          }
          // 转为 case 4
          bro->color = xp->color;
          rb_tree_set_black(xp);
          if (bro->left != nullptr)
            rb_tree_set_black(bro->left);
          rb_tree_right_rotate(xp, root);
          break;
        }
      }
      if (x != nullptr) {
        rb_tree_set_black(x);
      }
    }
    return y;
  }
}

template <typename T, typename Compare> class rb_tree {
public:
  typedef rb_tree_node<T> node_type;
  typedef rb_tree_node_base<T> base_type;
  typedef node_type *node_ptr;
  typedef base_type *base_ptr;

  typedef tinystl::allocator<T> data_allocator;
  typedef tinystl::allocator<node_type> node_allocator;
  typedef tinystl::allocator<base_type> base_allocator;

  typedef typename data_allocator::size_type size_type;
  typedef typename data_allocator::value_type value_type;
  typedef typename data_allocator::pointer pointer;
  typedef typename data_allocator::const_pointer const_pointer;
  typedef typename data_allocator::reference reference;
  typedef typename data_allocator::const_reference const_reference;

  typedef Compare key_compare;

private:
  base_ptr header_;
  size_type node_count_;
  key_compare key_comp_;

public:
  base_ptr &root() const { return header_->parent; }
  base_ptr &leftmost() const { return header_->left; }
  base_ptr &rightmost() const { return header_->right; }

protected:
  template <typename... Args> node_ptr create_node(Args... args);
  void destory_node(node_ptr);
  base_ptr copy_from(base_ptr, base_ptr);
  node_ptr clone_node(base_ptr);
  void erase_since(base_ptr);
  void rb_tree_init();
  void clear() {
    if (node_count_ != 0) {
      erase_since(root());
    }
    leftmost() = header_;
    rightmost() = header_;
    root() = nullptr;
    node_count_ = 0;
  }

public:
  rb_tree() { rb_tree_init(); }
  rb_tree(const rb_tree &);
  rb_tree(rb_tree &&) noexcept;

  rb_tree &operator=(const rb_tree &t);
  rb_tree &operator=(rb_tree &&t) noexcept;

  ~rb_tree();
};
template <typename T, typename Compare>
template <typename... Args>
typename rb_tree<T, Compare>::node_ptr
rb_tree<T, Compare>::create_node(Args... args) {
  auto tmp = node_allocator::allocate(1);
  try {
    tinystl::construct(std::addressof(tmp->value), std::forward<Args>(args)...);
    tmp->left = nullptr;
    tmp->right = nullptr;
    tmp->parent = nullptr;
  } catch (...) {
    tinystl::destory(std::addressof(tmp->value));
    node_allocator::deallocate(tmp);
    throw;
  }
  return tmp;
}
template <typename T, typename Compare>
void rb_tree<T, Compare>::destory_node(node_ptr p) {
  tinystl::destory(std::addressof(p->value));
  node_allocator::deallocate(p);
}
template <typename T, typename Compare>
typename rb_tree<T, Compare>::base_ptr
rb_tree<T, Compare>::copy_from(base_ptr x, base_ptr p) {
  auto top = clone_node(x);
  top->parent = p;
  try {
    if (x->right != nullptr) {
      top->right = copy_from(x->right, top);
    }
    p = top;
    x = x->left;
    while (x != nullptr) {
      auto y = clone_node(x);
      y->parent = p;
      p->left = y;
      if (x->right != nullptr) {
        y->right = copy_from(x->right, y);
      }
      p = y;
      x = x->left;
    }
  } catch (...) {
    erase_since(top);
    throw;
  }
  return top;
}

template <typename T, typename Compare>
typename rb_tree<T, Compare>::node_ptr
rb_tree<T, Compare>::clone_node(base_ptr p) {
  auto tmp = create_node(p->get_node_ptr()->value);
  tmp->color = p->color;
  return tmp;
}

template <typename T, typename Compare>
void rb_tree<T, Compare>::erase_since(base_ptr x) {
  while (x != nullptr) {
    erase_since(x->right);
    auto y = x->left;
    destory_node(x->get_node_ptr());
    x = y;
  }
}

template <typename T, typename Compare>
void rb_tree<T, Compare>::rb_tree_init() {
  header_ = base_allocator::allocate(1);
  root() = nullptr;
  leftmost() = header_;
  rightmost() = header_;
  node_count_ = 0;
}
template <typename T, typename Compare>
rb_tree<T, Compare>::rb_tree(const rb_tree &t) {
  rb_tree_init();
  if (t.node_count_ != 0) {
    header_->parent = copy_from(t.root(), header_);
    leftmost() = rb_tree_min(root());
    rightmost() = rb_tree_max(root());
    node_count_ = t.node_count_;
  }
  key_comp_ = t.key_comp_;
}
template <typename T, typename Compare>
rb_tree<T, Compare>::rb_tree(rb_tree &&t) noexcept
    : header_(std::move(t.header_)) {
  node_count_ = t.node_count_;
  key_comp_ = t.key_comp_;
  t.node_count_ = 0;
}
template <typename T, typename Compare> rb_tree<T, Compare>::~rb_tree() {
  clear();
  base_allocator::deallocate(header_);
}
template <typename T, typename Compare>
rb_tree<T, Compare> &rb_tree<T, Compare>::operator=(const rb_tree &t) {
  clear();
  if (t.node_count_ != 0) {
    header_->parent = copy_from(t.root(), header_);
    leftmost() = rb_tree_min(root());
    rightmost() = rb_tree_max(root());
    node_count_ = t.node_count_;
  }
  key_comp_ = t.key_comp_;
  return *this;
}
template <typename T, typename Compare>
rb_tree<T, Compare> &rb_tree<T, Compare>::operator=(rb_tree &&t) noexcept {
  clear();
  header_ = std::move(t.header_);
  node_count_ = t.node_count_;
  t.node_count_ = 0;
  key_comp_ = t.key_comp_;
  return *this;
}
} // namespace tinystl

#endif