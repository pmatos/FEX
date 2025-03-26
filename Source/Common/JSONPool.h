// SPDX-License-Identifier: MIT

#include <FEXCore/fextl/memory.h>
#include <FEXCore/fextl/list.h>

#include <tiny-json.h>

namespace FEX::JSON {
struct JsonAllocator : jsonPool_t {
  fextl::list<json_t> json_objects;

  JsonAllocator();
};

template<typename T>
const json_t* CreateJSON(T& Container, JsonAllocator& Allocator) {
  if (Container.empty()) {
    return nullptr;
  }

  // Make sure data is null-terminated
  if (Container.back() != '\0') {
    Container.push_back('\0');
  }

  // Attempt to parse the JSON
  const json_t* result = json_createWithPool(&Container.at(0), &Allocator);
  
  // Return null if parsing failed
  return result;
}
} // namespace FEX::JSON
