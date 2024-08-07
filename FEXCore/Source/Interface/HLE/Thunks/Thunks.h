// SPDX-License-Identifier: MIT
/*
$info$
tags: glue|thunks
$end_info$
*/

#pragma once

#include "Interface/IR/IR.h"

#include <FEXCore/fextl/memory.h>
#include <FEXCore/fextl/vector.h>

namespace FEXCore::Context {
class ContextImpl;
}

namespace FEXCore::Core {
struct InternalThreadState;
}

namespace FEXCore::IR {
struct SHA256Sum;
}

namespace FEXCore {
typedef void ThunkedFunction(void* ArgsRv);

class ThunkHandler {
public:
  virtual ThunkedFunction* LookupThunk(const IR::SHA256Sum& sha256) = 0;
  virtual void RegisterTLSState(FEXCore::Core::InternalThreadState* Thread) = 0;
  virtual ~ThunkHandler() {}

  static fextl::unique_ptr<ThunkHandler> Create();

  virtual void AppendThunkDefinitions(std::span<const FEXCore::IR::ThunkDefinition> Definitions) = 0;
};
}; // namespace FEXCore
