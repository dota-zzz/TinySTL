#ifndef MYTINYSTL_DEQUE_H_
#define MYTINYSTL_DEQUE_H_

#include "allocator.h"
#include "iterator.h"
#include <memory>
#include <iostream>

//copy_insert的异常
//operator-

namespace tinystl
{

#ifndef DEQUE_INIT_MAP_SIZE_
#define DEQUE_INIT_MAP_SIZE_ 8
#endif

  template <typename T>
  struct deque_buffer_size
  {
    static constexpr size_t value = sizeof(T) < 256 ? 4096 / sizeof(T) : 16;
  };

  template <typename T, typename Ref, typename Ptr>
  struct deque_iterator : public tinystl::iterator<random_access_iterator_tag, T>
  {
    typedef deque_iterator<T, T &, T *> iterator;
    typedef deque_iterator<T, const T &, const T *> const_iterator;
    typedef deque_iterator self;

    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef Ref reference;
    typedef Ptr pointer;
    typedef T *value_pointer;
    typedef T **map_pointer;

    static const size_type buffer_size = deque_buffer_size<T>::value;
    value_pointer cur;
    value_pointer first;
    value_pointer last;
    map_pointer node;

  public:
    deque_iterator() noexcept
        : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}
    deque_iterator(value_pointer v, map_pointer m)
        : cur(v), first(*m), last(first + buffer_size), node(m) {}
    deque_iterator(const iterator &t)
        : cur(t.cur), first(t.first), last(t.last), node(t.node) {}
    deque_iterator(const const_iterator &t)
        : cur(t.cur), first(t.first), last(t.last), node(t.node) {}
    deque_iterator(iterator &&t) noexcept
        : cur(t.cur), first(t.first), last(t.last), node(t.node)
    {
      t.cur = nullptr;
      t.first = nullptr;
      t.last = nullptr;
      t.node = nullptr;
    }
    self &operator=(const iterator &t)
    {
      cur = t.cur;
      first = t.first;
      last = t.last;
      node = t.node;
      return *this;
    }
    self &operator=(iterator &&t)
    {
      cur = t.cur;
      first = t.first;
      last = t.last;
      node = t.node;
      t.cur = nullptr;
      t.first = nullptr;
      t.last = nullptr;
      t.node = nullptr;
      return *this;
    }
    void set_node(map_pointer new_node)
    {
      node = new_node;
      first = *node;
      last = first + buffer_size;
    }
    reference operator*() { return *cur; }
    pointer operator->() { return cur; }

    self &operator++()
    {
      ++cur;
      if (cur == last)
      {
        set_node(node + 1);
        cur = first;
      }
      return *this;
    }
    self operator++(int)
    {
      auto x = *this;
      ++cur;
      if (cur == last)
      {
        set_node(node + 1);
        cur = first;
      }
      return x;
    }
    self &operator--()
    {
      if (cur == first)
      {
        set_node(node - 1);
        cur = last;
      }
      --cur;
      return *this;
    }
    self operator--(int)
    {
      auto x = *this;
      if (cur == first)
      {
        set_node(node - 1);
        cur = last;
      }
      --cur;
      return x;
    }
    self &operator+=(difference_type n)
    {
      difference_type offset = n + (cur - first);
      if (offset >= 0 && offset < static_cast<difference_type>(buffer_size))
      {
        cur += n;
      }
      else
      {
        difference_type node_offset =
            offset > 0
                ? offset / static_cast<difference_type>(buffer_size)
                : -static_cast<difference_type>((-offset - 1) / buffer_size) - 1;
        set_node(node + node_offset);
        cur = first +
              (offset - node_offset * static_cast<difference_type>(buffer_size));
      }
      return *this;
    }
    self operator+(difference_type n) const
    {
      deque_iterator x = *this;
      x += n;
      return x;
    }
    self &operator-=(difference_type n) { return *this += -n; }
    self operator-(difference_type n) const
    {
      deque_iterator x = *this;
      x -= n;
      return x;
    }
    difference_type operator-(const deque_iterator &t) const
    {
      return static_cast<difference_type>(buffer_size * (node - t.node) + (cur - first) - (t.cur - t.first));
    }
    bool operator>(const deque_iterator &t)
    {
      return node == t.node ? cur > t.cur : node > t.node;
    }
    bool operator>=(const deque_iterator &t)
    {
      return node == t.node ? cur >= t.cur : node >= t.node;
    }
    bool operator<(const deque_iterator &t) { return !(*this >= t); }
    bool operator<=(const deque_iterator &t) { return !(*this > t); }
    bool operator==(const deque_iterator &t) const
    {
      return node == t.node && cur == t.cur;
    }
    bool operator!=(const deque_iterator &t) { return !(*this == t); }
  };

  template <typename T>
  class deque
  {
  public:
    typedef tinystl::allocator<T> data_allocator;
    typedef tinystl::allocator<T *> map_allocator;

    typedef T value_type;
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef T &reference;
    typedef const T &const_reference;
    typedef std::ptrdiff_t difference_type;
    typedef std::size_t size_type;
    typedef T **map_pointer;
    typedef deque<T> self;

    typedef deque_iterator<T, T &, T *> iterator;
    typedef deque_iterator<T, const T &, const T *> const_iterator;

    static const size_type buffer_size = deque_buffer_size<T>::value;

  private:
    iterator begin_;
    iterator end_;
    map_pointer map_;
    size_type map_size;

  protected:
    void fill_init(size_type n, const value_type &t);
    template <typename InputIterator>
    void copy_init(InputIterator first, InputIterator last);
    void map_init(size_type);
    map_pointer create_map(size_type);
    void create_buffer(map_pointer, map_pointer);
    void destory_buffer(map_pointer, map_pointer);
    void require_buffer(size_type, bool);
    void reallocate_map_at_front(size_type);
    void reallocate_map_at_end(size_type);
    void clear();
    iterator fill_insert(iterator &, size_type, const value_type &);
    template <typename InputIterator>
    iterator copy_insert(iterator &, InputIterator, InputIterator);
    void fill_assign(size_type, const value_type &);
    template <typename InputIterator>
    void range_assign(InputIterator, InputIterator);

  public:
    //构造函数
    deque() { fill_init(0, value_type()); }
    deque(size_type n) { fill_init(n, value_type()); }
    deque(size_type n, const value_type &t) { fill_init(n, t); }
    deque(std::initializer_list<value_type> l) { copy_init(l.begin(), l.end()); }
    template <
        typename InputIterator,
        typename std::enable_if<
            tinystl::has_input_iterator_cat<InputIterator>::value, int>::type = 0>
    deque(InputIterator first, InputIterator last)
    {
      copy_init(first, last);
    }
    deque(const self &d) { copy_init(d.begin(), d.end()); }
    deque(self &&d) noexcept
        : begin_(std::move(d.begin_)), end_(std::move(d.end_)), map_(d.map_),
          map_size(d.map_size)
    {
      d.map_ = nullptr;
      d.map_size = 0;
    }
    //赋值运算符
    self &operator=(const self &t)
    {
      const auto len = size();
      if (len > t.size())
      {
        erase(std::copy(t.begin_, t.end_, begin_), end_);
      }
      else
      {
        iterator mid = t.begin() + len;
        std::copy(t.begin_, mid, begin_);
        insert(end_, mid, t.end_);
      }
      return *this;
    }
    self &operator=(self &&t)
    {
      clear();
      begin_ = std::move(t.begin_);
      end_ = std::move(t.end_);
      map_ = t.map_;
      map_size = t.map_size;
      t.map_ = nullptr;
      t.map_size = 0;
      return *this;
    }

    //析构
    ~deque()
    {
      if (map_ != nullptr)
      {
        clear();
        map_allocator::deallocate(map_);
      }
    }
    //迭代器相关
    iterator begin() { return begin_; }
    iterator begin() const { return begin_; }
    iterator end() { return end_; }
    iterator end() const { return end_; }
    //重构
    void assign(size_type n, const value_type &t) { fill_assign(n, t); }
    void assign(std::initializer_list<value_type> l) { range_assign(l.begin(), l.end()); }
    template <typename InputIterator, typename std::enable_if<tinystl::has_input_iterator_cat<InputIterator>::value, int>::type = 0>
    void assign(InputIterator first, InputIterator last) { range_assign(first, last); }
    //元素相关
    iterator insert(iterator pos, const value_type &t) { return fill_insert(pos, 1, t); }
    iterator insert(iterator pos, size_type n, const value_type &t) { return fill_insert(pos, n, t); }
    iterator insert(iterator pos, value_type &&t)
    {
      return emplace(pos, std::move(t));
    }
    template <typename InputIterator, typename std::enable_if<tinystl::has_input_iterator_cat<InputIterator>::value, int>::type = 0>
    iterator insert(iterator pos, InputIterator first, InputIterator last)
    {
      return copy_insert(pos, first, last);
    }
    iterator insert(iterator pos, std::initializer_list<value_type> l)
    {
      return copy_insert(pos, l.begin(), l.end());
    }

    template <typename... Args>
    void emplace_front(Args... args);
    template <typename... Args>
    void emplace_back(Args... args);
    template <typename... Args>
    iterator emplace(iterator pos, Args... args);

    void push_front(const value_type &t) { emplace_front(t); }
    void push_back(const value_type &t) { emplace_back(t); }
    void push_front(value_type &&t) { emplace_front(std::move(t)); }
    void push_back(value_type &&t) { emplace_back(std::move(t)); }

    void pop_back();
    void pop_front();

    iterator erase(iterator);
    iterator erase(iterator, iterator);

    //查询
    bool empty() const noexcept { return begin_ == end_; }
    size_type size() const noexcept { return static_cast<size_type>(end_ - begin_); }

    //for_debug
    void show()
    {
      std::cout << "map_size:" << map_size << std::endl;
      std::cout << "elem_size:" << size() << std::endl;
      std::cout << "begin_(begin_节点位置 begin_元素位置):" << begin_.node - map_ << " " << begin_.cur - begin_.first;
      std::cout << "end_(end_节点位置 end_元素位置):" << end_.node - map_ << " " << end_.cur - end_.first;
      for (auto x = begin_; x < end_; ++x)
      {
        std::cout << *x << " ";
      }
      std::cout << std::endl;
    }
  };
  template <typename T>
  void deque<T>::fill_init(size_type n, const value_type &t)
  {
    map_init(n);
    for (map_pointer cur = begin_.node; cur < end_.node; ++cur)
    {
      std::uninitialized_fill(*cur, *cur + buffer_size, t);
    }
    std::uninitialized_fill(*(end_.node), end_.cur, t);
  }

  template <typename T>
  void deque<T>::map_init(size_type n)
  {
    const size_type node_num = n / buffer_size + 1;
    map_size =
        std::max(node_num + 2, static_cast<size_type>(DEQUE_INIT_MAP_SIZE_));
    try
    {
      map_ = create_map(map_size);
    }
    catch (...)
    {
      map_ = nullptr;
      map_size = 0;
      throw;
    }
    map_pointer nstart = map_ + (map_size - node_num) / 2;
    map_pointer nfinish = nstart + node_num - 1;
    try
    {
      create_buffer(nstart, nfinish);
    }
    catch (...)
    {
      map_allocator::deallocate(map_);
      map_ = nullptr;
      map_size = 0;
      throw;
    }
    begin_.set_node(nstart);
    begin_.cur = *nstart;
    end_.set_node(nfinish);
    end_.cur = *nfinish + n % buffer_size;
  }

  template <typename T>
  typename deque<T>::map_pointer deque<T>::create_map(size_type n)
  {
    map_pointer mp = map_allocator::allocate(n);
    for (size_type i = 0; i < n; ++i)
    {
      *(mp + i) = nullptr;
    }
    return mp;
  }

  template <typename T>
  void deque<T>::create_buffer(map_pointer first, map_pointer last)
  {
    map_pointer cur;
    try
    {
      for (cur = first; cur <= last; ++cur)
      {
        *cur = data_allocator::allocate(buffer_size);
      }
    }
    catch (...)
    {
      for (map_pointer i = first; i <= cur; ++i)
      {
        data_allocator::deallocate(*i);
        *i = nullptr;
      }
      throw;
    }
  }
  template <typename T>
  template <typename InputIterator>
  void deque<T>::copy_init(InputIterator first, InputIterator last)
  {
    const size_type n = tinystl::distance(first, last);
    map_init(n);
    for (auto cur = begin_.node; cur < end_.node; ++cur)
    {
      auto next = first;
      tinystl::advance(next, buffer_size);
      std::uninitialized_copy(first, next, *cur);
      first = next;
    }
    std::uninitialized_copy(first, last, *(end_.node));
  }
  template <typename T>
  void deque<T>::clear()
  {
    for (map_pointer cur = begin_.node + 1; cur < end_.node; ++cur)
    {
      tinystl::destory(*cur, *cur + buffer_size);
    }
    if (begin_.node != end_.node)
    {
      tinystl::destory(begin_.first, begin_.last);
      tinystl::destory(end_.first, end_.cur);
    }
    else
    {
      tinystl::destory(begin_.first, end_.cur);
    }
    for (map_pointer cur = begin_.node; cur <= end_.node; ++cur)
    {
      data_allocator::deallocate(*cur);
      *cur = nullptr;
    }
    end_ = begin_;
  }
  template <typename T>
  void deque<T>::require_buffer(size_type n, bool front)
  {
    if (front && static_cast<size_type>(begin_.cur - begin_.first) < n)
    {
      const size_type need_node = (n - (begin_.cur - begin_.first)) / buffer_size + 1;
      if (need_node > static_cast<size_type>(begin_.node - map_))
      {
        reallocate_map_at_front(need_node);
        return;
      }
      create_buffer(begin_.node - need_node, begin_.node - 1);
    }
    else if (!front && static_cast<size_type>(end_.last - end_.cur - 1) < n)
    {
      const size_type need_node = (n - (end_.last - end_.cur - 1)) / buffer_size + 1;
      if (need_node > static_cast<size_type>(map_ + map_size - end_.node - 1))
      {
        reallocate_map_at_end(need_node);
        return;
      }
      create_buffer(end_.node + 1, end_.node + need_node);
    }
  }
  template <typename T>
  void deque<T>::reallocate_map_at_front(size_type need_node)
  {
    const size_type new_map_size = std::max(map_size * 2, map_size + need_node + DEQUE_INIT_MAP_SIZE_);
    const size_type old_node = end_.node - begin_.node + 1;
    const size_type new_node = need_node + old_node;
    map_pointer new_map = create_map(new_map_size);
    map_pointer new_begin = new_map + (new_map_size - new_node) / 2;
    map_pointer mid = new_begin + need_node;
    map_pointer new_end = mid + old_node;
    create_buffer(new_begin, mid - 1);
    for (auto begin1 = begin_.node, begin2 = mid; begin1 <= end_.node; ++begin1, ++begin2)
    {
      *begin2 = *begin1;
    }
    begin_.cur = *mid + (begin_.cur - begin_.first);
    end_.cur = *(new_end - 1) + (end_.cur - end_.first);
    map_allocator::deallocate(map_);
    begin_.set_node(mid);
    end_.set_node(new_end - 1);
    map_ = new_map;
    map_size = new_map_size;
  }
  template <typename T>
  void deque<T>::reallocate_map_at_end(size_type need_node)
  {
    const size_type new_map_size = std::max(map_size * 2, map_size + need_node + DEQUE_INIT_MAP_SIZE_);
    const size_type old_node = end_.node - begin_.node + 1;
    const size_type new_node = need_node + old_node;
    map_pointer new_map = create_map(new_map_size);
    map_pointer new_begin = new_map + (new_map_size - new_node) / 2;
    map_pointer mid = new_begin + old_node;
    map_pointer new_end = mid + need_node;
    for (auto begin1 = map_, begin2 = new_begin; begin1 <= end_.node; ++begin1, ++begin2)
    {
      *begin2 = *begin1;
    }
    create_buffer(mid, new_end - 1);
    begin_ = iterator(*new_begin + (begin_.cur - begin_.first), new_begin);
    end_ = iterator(*(mid - 1) + (end_.cur - end_.first), mid - 1);
    map_allocator::deallocate(map_);
    map_ = new_map;
    map_size = new_map_size;
  }
  template <typename T>
  typename deque<T>::iterator deque<T>::fill_insert(iterator &pos, size_type n, const value_type &t)
  {
    const size_type num_before = pos - begin_;
    if (num_before < size() / 2)
    {
      require_buffer(n, true);
      iterator new_begin = begin_ - n;
      iterator old_begin = begin_;
      pos = old_begin + num_before;
      try
      {
        if (num_before >= n)
        {
          iterator copy_end = begin_ + n;
          std::uninitialized_copy(old_begin, copy_end, new_begin);
          std::copy(copy_end, pos, old_begin);
          std::fill(pos - n, pos, t);
        }
        else
        {
          std::uninitialized_copy(old_begin, pos, new_begin);
          std::uninitialized_fill(new_begin + num_before, old_begin, t);
          std::fill(old_begin, pos, t);
        }
        begin_ = new_begin;
        return pos;
      }
      catch (...)
      {
        if (new_begin.node != old_begin.node)
        {
          destory_buffer(new_begin.node, old_begin.node - 1);
        }
        throw;
      }
    }
    else
    {
      const size_type num_after = end_ - pos;
      require_buffer(n, false);
      iterator new_end = end_ + n;
      iterator old_end = end_;
      pos = old_end - num_after;
      try
      {
        if (num_after > n)
        {
          iterator copy_begin = old_end - n;
          std::uninitialized_copy(copy_begin, old_end, old_end);
          std::copy_backward(pos, copy_begin, old_end);
          std::fill(pos, pos + n, t);
        }
        else
        {
          std::uninitialized_fill(old_end, pos + n, t);
          std::uninitialized_copy(pos, old_end, pos + n);
          std::fill(pos, old_end, t);
        }
        end_ = new_end;
        return pos;
      }
      catch (...)
      {
        if (new_end.node != end_.node)
        {
          destory_buffer(old_end.node + 1, new_end.node);
        }
      }
    }
  }
  template <typename T>
  void deque<T>::destory_buffer(map_pointer nstart, map_pointer n_finish)
  {
    for (map_pointer cur = nstart; cur <= n_finish; ++cur)
    {
      data_allocator::deallocate(*cur);
      *cur = nullptr;
    }
  }
  template <typename T>
  template <typename InputIterator>
  typename deque<T>::iterator deque<T>::copy_insert(iterator &pos, InputIterator first, InputIterator last)
  {
    const size_type num_before = pos - begin_;
    const size_type n = tinystl::distance(first, last);
    if (num_before < size() / 2)
    {
      require_buffer(n, true);
      iterator new_begin = begin_ - n;
      iterator old_begin = begin_;
      pos = old_begin + num_before;
      try
      {
        if (num_before >= n)
        {
          iterator copy_end = begin_ + n;
          std::uninitialized_copy(old_begin, copy_end, new_begin);
          std::copy(copy_end, pos, old_begin);
          std::copy(first, last, pos - n);
        }
        else
        {
          auto mid = first;
          tinystl::advance(mid, n - num_before);
          std::uninitialized_copy(old_begin, pos, new_begin);
          std::uninitialized_copy(first, mid, new_begin + num_before);
          std::copy(mid, last, old_begin);
        }
        begin_ = new_begin;
        return pos;
      }
      catch (...)
      {
        if (new_begin.node != old_begin.node)
        {
          destory_buffer(new_begin.node, old_begin.node - 1);
        }
        throw;
      }
    }
    else
    {
      const size_type num_after = end_ - pos;
      require_buffer(n, false);
      iterator new_end = end_ + n;
      iterator old_end = end_;
      pos = old_end - num_after;
      try
      {
        if (num_after > n)
        {
          iterator copy_begin = old_end - n;
          std::uninitialized_copy(copy_begin, old_end, old_end);
          std::copy_backward(pos, copy_begin, old_end);
          std::copy(first, last, pos);
        }
        else
        {
          auto mid = first;
          tinystl::advance(mid, num_after);
          std::uninitialized_copy(mid, last, old_end);
          std::uninitialized_copy(pos, old_end, pos + n);
          std::copy(first, mid, pos);
        }
        end_ = new_end;
        return pos;
      }
      catch (...)
      {
        if (new_end.node != end_.node)
        {
          destory_buffer(old_end.node + 1, new_end.node);
        }
      }
    }
  }
  template <typename T>
  template <typename... Args>
  void deque<T>::emplace_front(Args... args)
  {
    if (begin_.cur != begin_.first)
    {
      --begin_;
      tinystl::construct(begin_.cur, std::forward<Args>(args)...);
    }
    else
    {
      require_buffer(1, true);
      try
      {
        --begin_;
        tinystl::construct(begin_.cur, std::forward<Args>(args)...);
      }
      catch (...)
      {
        ++begin_;
        throw;
      }
    }
  }

  template <typename T>
  template <typename... Args>
  void deque<T>::emplace_back(Args... args)
  {
    if (end_.cur != end_.last - 1)
    {
      tinystl::construct(end_.cur, std::forward<Args>(args)...);
      ++end_;
    }
    else
    {
      require_buffer(1, false);
      try
      {
        tinystl::construct(end_.cur, std::forward<Args>(args)...);
        ++end_;
      }
      catch (...)
      {
        --end_;
        throw;
      }
    }
  }

  template <typename T>
  template <typename... Args>
  typename deque<T>::iterator deque<T>::emplace(iterator pos, Args... args)
  {
    if (pos == begin_)
    {
      emplace_front(std::forward<Args>(args)...);
    }
    else if (pos == end_)
    {
      emplace_back(std::forward<Args>(args)...);
    }
    else
    {
      const size_type num_before = pos - begin_;
      if (num_before <= size() / 2)
      {
        emplace_front(*begin_);
        pos = begin_ + num_before + 1;
        std::copy(begin_ + 2, pos, begin_ + 1);
        *(pos - 1) = std::move(value_type(std::forward<Args>(args)...));
        return pos;
      }
      else
      {
        const size_type num_after = end_ - pos;
        emplace_back(*(end_ - 1));
        pos = end_ - num_after - 1;
        std::copy_backward(end_ - 2, pos, end_ - 1);
        *pos = std::move(value_type(std::forward<Args>(args)...));
        return pos + 1;
      }
    }
  }
  template <typename T>
  void deque<T>::pop_back()
  {
    MY_DEBUG(!empty());
    if (end_.cur != end_.first)
    {
      --end_;
      tinystl::destory(end_.cur);
    }
    else
    {
      --end_;
      tinystl::destory(end_.cur);
      destory_buffer(end_.node + 1, end_.node + 1);
    }
  }
  template <typename T>
  void deque<T>::pop_front()
  {
    MY_DEBUG(!empty());
    if (begin_.cur != begin_.last - 1)
    {
      tinystl::destory(begin_.cur);
      ++begin_;
    }
    else
    {
      tinystl::destory(begin_.cur);
      ++begin_;
      destory_buffer(begin_.node - 1, begin_.node - 1);
    }
  }
  template <typename T>
  typename deque<T>::iterator deque<T>::erase(iterator pos)
  {
    const size_type num_before = pos - begin_;
    if (num_before <= size() / 2)
    {
      std::copy_backward(begin_, pos, pos + 1);
      pop_front();
      return pos + 1;
    }
    else
    {
      std::copy(pos + 1, end_, pos);
      pop_back();
      return pos;
    }
  }
  template <typename T>
  typename deque<T>::iterator deque<T>::erase(iterator first, iterator last)
  {
    if (first == begin_ && last == end_)
    {
      clear();
      return end_;
    }
    else
    {
      const size_type num_before = first - begin_;
      const size_type num_after = end_ - last;
      if (num_before < num_after)
      {
        std::copy_backward(begin_, first, last);
        for (auto x = begin_; x < begin_ + (last - first); ++x)
        {
          tinystl::destory(x.cur);
        }
        begin_ = begin_ + (last - first);
        return last;
      }
      else
      {
        std::copy(last, end_, first);
        for (auto x = end_ - (last - first); x < end_; ++x)
        {
          tinystl::destory(x.cur);
        }
        end_ = end_ - (last - first);
        return first;
      }
    }
  }
  template <typename T>
  void deque<T>::fill_assign(size_type n, const value_type &t)
  {
    if (n > size())
    {
      std::fill(begin_, end_, t);
      insert(end_, n - size(), t);
    }
    else
    {
      std::fill(begin_, begin_ + n, t);
      erase(begin_ + n, end_);
    }
  }
  template <typename T>
  template <typename InputIterator>
  void deque<T>::range_assign(InputIterator first, InputIterator last)
  {
    const size_type n = tinystl::distance(first, last);
    if (n > size())
    {
      std::copy(first, first + size(), begin_);
      insert(end_, first + size(), last);
    }
    else
    {
      erase(std::copy(first, last, begin_), end_);
    }
  }

} // namespace tinystl

#endif
