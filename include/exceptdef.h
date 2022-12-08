#ifndef MYTINYSTL_EXCEPTDEF_H_
#define MYTINYSTL_EXCEPTDEF_H_

#include <cassert>
#include <stdexcept>
namespace tinystl {
#define MY_DEBUG(EXPR) assert(EXPR)

#define THROW_OUT_OF_RANGE_IF(expr,what) if(expr) throw std::out_of_range(what)
} // namespace tinystl

#endif