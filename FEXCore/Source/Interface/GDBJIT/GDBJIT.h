// SPDX-License-Identifier: MIT


#include <Interface/IR/AOTIR.h>

namespace FEXCore {
  void GDBJITRegister(FEXCore::IR::AOTIRCacheEntry *Entry, uintptr_t VAFileStart, uint64_t GuestRIP, uintptr_t HostEntry, FEXCore::Core::DebugData *DebugData);
}