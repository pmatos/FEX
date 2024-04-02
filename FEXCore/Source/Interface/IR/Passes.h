// SPDX-License-Identifier: MIT
#pragma once

#include <FEXCore/fextl/memory.h>

namespace FEXCore {
  class CPUIDEmu;
}

namespace FEXCore::Utils {
class IntrusivePooledAllocator;
}

namespace FEXCore::IR {
class Pass;
class RegisterAllocationPass;
class RegisterAllocationData;

fextl::unique_ptr<FEXCore::IR::Pass>
CreateConstProp(bool InlineConstants, bool SupportsTSOImm9, bool Is64BitMode);
fextl::unique_ptr<FEXCore::IR::Pass> CreateContextLoadStoreElimination(bool SupportsAVX);
fextl::unique_ptr<FEXCore::IR::Pass> CreateInlineCallOptimization(const FEXCore::CPUIDEmu* CPUID);
fextl::unique_ptr<FEXCore::IR::Pass> CreateDeadFlagCalculationEliminination();
fextl::unique_ptr<FEXCore::IR::Pass> CreateDeadStoreElimination(bool SupportsAVX);
fextl::unique_ptr<FEXCore::IR::Pass> CreatePassDeadCodeElimination();
fextl::unique_ptr<FEXCore::IR::Pass> CreateIRCompaction(FEXCore::Utils::IntrusivePooledAllocator &Allocator);
fextl::unique_ptr<FEXCore::IR::RegisterAllocationPass>
CreateRegisterAllocationPass(FEXCore::IR::Pass *CompactionPass,
                             bool SupportsAVX);
fextl::unique_ptr<FEXCore::IR::Pass> CreateLongDivideEliminationPass();
fextl::unique_ptr<FEXCore::IR::Pass> CreateX87StackOptimizationPass();

namespace Validation {
fextl::unique_ptr<FEXCore::IR::Pass> CreateIRValidation();
fextl::unique_ptr<FEXCore::IR::Pass> CreateRAValidation();
fextl::unique_ptr<FEXCore::IR::Pass> CreateValueDominanceValidation();
}

namespace Debug {
fextl::unique_ptr<FEXCore::IR::Pass> CreateIRDumper();
}
}

