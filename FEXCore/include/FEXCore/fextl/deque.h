// SPDX-License-Identifier: MIT
#pragma once
#include <FEXCore/fextl/allocator.h>

#include <deque>

namespace fextl {
  template<class T, class Allocator = fextl::FEXAlloc<T>> using deque = std::deque<T, Allocator>;
}
