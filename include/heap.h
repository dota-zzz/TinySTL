#ifndef MYTINYSTL_HEAP_H_
#define MYTINYSTL_HEAP_H_

#include "iterator.h"
#include <iostream>

namespace tinystl {
template <typename RandomAccessIterator, typename Distance, typename T>
void push_heap_aux(RandomAccessIterator first, Distance hole_index,
                   Distance top_index, T value) {
  auto parent = (hole_index - 1) / 2;
  while (hole_index > top_index && *(first + parent) < value) {
    *(first + hole_index) = *(first + parent);
    hole_index = parent;
    parent = (hole_index - 1) / 2;
  }
  *(first + hole_index) = value;
}

template <typename RandomAccessIterator, typename Distance>
void push_heap_d(RandomAccessIterator first, RandomAccessIterator last,
                 Distance *) {
  push_heap_aux(first, static_cast<Distance>(last - first - 1),
                static_cast<Distance>(0), *(last - 1));
}

template <typename RandomAccessIterator>
void push_heap(RandomAccessIterator first, RandomAccessIterator last) {
  push_heap_d(first, last, tinystl::distance_type(first));
}

template <typename RandomAccessIterator, typename Distance, typename T,
          typename Compare>
void push_heap_aux(RandomAccessIterator first, Distance hole_index,
                   Distance top_index, T value, Compare compare) {
  auto parent = (hole_index - 1) / 2;
  while (hole_index > top_index && compare(*(first + parent), value)) {
    *(first + hole_index) = *(first + parent);
    hole_index = parent;
    parent = (hole_index - 1) / 2;
  }
  *(first + hole_index) = value;
}

template <typename RandomAccessIterator, typename Distance, typename Compare>
void push_heap_d(RandomAccessIterator first, RandomAccessIterator last,
                 Distance *, Compare compare) {
  push_heap_aux(first, static_cast<Distance>(last - first - 1),
                static_cast<Distance>(0), *(last - 1), compare);
}

template <typename RandomAccessIterator, typename Compare>
void push_heap(RandomAccessIterator first, RandomAccessIterator last,
               Compare compare) {
  push_heap_d(first, last, tinystl::distance_type(first), compare);
}

template <typename RandomAccessIterator, typename T, typename Distance>
void adjust_heap(RandomAccessIterator first, Distance hole_index, Distance len,
                 T value) {
  auto top_index = hole_index;
  auto rchild = 2 * (hole_index + 1);
  while (rchild < len) {
    if (*(first + rchild - 1) > *(first + rchild)) {
      --rchild;
    }
    *(first + hole_index) = *(first + rchild);
    hole_index = rchild;
    rchild = 2 * (hole_index + 1);
  }
  if (rchild == len) {
    *(first + hole_index) = *(first + rchild - 1);
    hole_index = rchild - 1;
  }
  push_heap_aux(first, hole_index, top_index, value);
}

template <typename RandomAccessIterator, typename T, typename Distance>
void pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last,
                  RandomAccessIterator result, T value, Distance *) {
  *result = *first;
  adjust_heap(first, static_cast<Distance>(0), last - first, value);
}

template <typename RandomAccessIterator>
void pop_heap(RandomAccessIterator first, RandomAccessIterator last) {
  pop_heap_aux(first, last - 1, last - 1, *(last - 1),
               tinystl::distance_type(first));
}

template <typename RandomAccessIterator, typename T, typename Distance,
          typename Compare>
void adjust_heap(RandomAccessIterator first, Distance hole_index, Distance len,
                 T value, Compare compare) {
  auto rchild = 2 * (hole_index + 1);
  while (rchild < len) {
    if (compare(*(first + rchild), *(first + rchild - 1))) {
      --rchild;
    }
    *(first + hole_index) = *(first + rchild);
    hole_index = rchild;
    rchild = 2 * (hole_index + 1);
  }
  if (rchild == len) {
    *(first + hole_index) = *(first + rchild - 1);
    hole_index = rchild - 1;
  }
  push_heap_aux(first, hole_index, hole_index, value);
}

template <typename RandomAccessIterator, typename T, typename Distance,
          typename Compare>
void pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last,
                  RandomAccessIterator result, T value, Distance *,
                  Compare compare) {
  *result = *first;
  adjust_heap(first, static_cast<Distance>(0), last - first, value, compare);
}

template <typename RandomAccessIterator, typename Compare>
void pop_heap(RandomAccessIterator first, RandomAccessIterator last,
              Compare compare) {
  pop_heap_aux(first, last - 1, last - 1, *(last - 1),
               tinystl::distance_type(first), compare);
}

template <typename RandomAccessIterator>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
  while (last - first > 1) {
    pop_heap(first, last--);
  }
}
template <typename RandomAccessIterator, typename Compare>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last,
               Compare compare) {
  while (last - first > 1) {
    pop_heap(first, last--, compare);
  }
}

template <typename RandomAccessIterator, typename Distance>
void make_heap_aux(RandomAccessIterator first, RandomAccessIterator last,
                   Distance *) {
  Distance len = last - first;
  if (len < 2) {
    return;
  }
  auto hole_index = (len - 2) / 2;
  while (hole_index >= 0) {
    adjust_heap(first, hole_index, len, *(first + hole_index));
    --hole_index;
  }
}

template <typename RandomAccessIterator>
void make_heap(RandomAccessIterator first, RandomAccessIterator last) {
  make_heap_aux(first, last, distance_type(first));
}

template <typename RandomAccessIterator, typename Distance, typename Compare>
void make_heap_aux(RandomAccessIterator first, RandomAccessIterator last,
                   Distance *, Compare compare) {
  Distance len = last - first;
  if (len < 2) {
    return;
  }
  auto hole_index = (len - 2) / 2;
  while (hole_index >= 0) {
    adjust_heap(first, hole_index, len, *(first + hole_index), compare);
    --hole_index;
  }
}

template <typename RandomAccessIterator, typename Compare>
void make_heap(RandomAccessIterator first, RandomAccessIterator last,
               Compare compare) {
  make_heap_aux(first, last, distance_type(first), compare);
}
} // namespace tinystl

#endif