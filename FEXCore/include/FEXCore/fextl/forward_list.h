// SPDX-License-Identifier: MIT
#pragma once
#include <FEXCore/fextl/allocator.h>

#include <forward_list>

namespace fextl {
  template<class T, class Allocator = fextl::FEXAlloc<T>> using forward_list = std::forward_list<T, Allocator>;
}
