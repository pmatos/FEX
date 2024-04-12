// SPDX-License-Identifier: MIT
#include "FEXCore/IR/IR.h"
#include "Interface/Core/OpcodeDispatcher.h"
#include "Interface/Core/X86Tables/X86Tables.h"

#include <FEXCore/Core/CoreState.h>
#include <FEXCore/Core/X86Enums.h>
#include <FEXCore/Utils/EnumUtils.h>
#include <FEXCore/Utils/LogManager.h>
#include <FEXCore/Utils/FPState.h>

#include <stddef.h>
#include <stdint.h>

namespace FEXCore::IR {
class OrderedNode;
#define OpcodeArgs [[maybe_unused]] FEXCore::X86Tables::DecodedOp Op

// Float LoaD operation
template <size_t width> void OpDispatchBuilder::FLD(OpcodeArgs) {
  static_assert(width == 32 || width == 64 || width == 80, "Unsupported FLD width");

  CurrentHeader->HasX87 = true;
  size_t read_width = (width == 80) ? 16 : width / 8;
  OrderedNode *data{};

  if (Op->Src[0].IsNone()) {
    // Implicit arg
    data = _ReadStackValue(Op->OP & 7);
  } else {
    // Read from memory
    data = LoadSource_WithOpSize(FPRClass, Op, Op->Src[0], read_width, Op->Flags);
    data = _F80CVTTo(data, read_width);
  }
  _PushStack(data, OpSize::i128Bit, true, read_width);
}

template void OpDispatchBuilder::FLD<32>(OpcodeArgs);
template void OpDispatchBuilder::FLD<64>(OpcodeArgs);
template void OpDispatchBuilder::FLD<80>(OpcodeArgs);

template <size_t width> void OpDispatchBuilder::FST(OpcodeArgs) {
  static_assert(width == 32 || width == 64 || width == 80, "Unsupported FST width");
  CurrentHeader->HasX87 = true;

  if (Op->Src[0].IsNone()) { // Destination is stack
    auto offset = Op->OP & 7;
    _StoreStackToStack(offset);
  } else {
    // Destination is memory
    OrderedNode* Mem = LoadSource(GPRClass, Op, Op->Dest, Op->Flags, {.LoadData = false});
    if constexpr (width == 80) {
      _StoreStackMemory(Mem, OpSize::i128Bit, true, 10);
    } else if constexpr (width == 32 || width == 64) {
      _StoreStackMemory(Mem, OpSize::i128Bit, true, width / 8);
    }
  }
  if (Op->TableInfo->Flags & X86Tables::InstFlags::FLAGS_POP) {
    _PopStackDestroy();
  }
}

template void OpDispatchBuilder::FST<32>(OpcodeArgs);
template void OpDispatchBuilder::FST<64>(OpcodeArgs);
template void OpDispatchBuilder::FST<80>(OpcodeArgs);

template<size_t width, bool Integer, OpDispatchBuilder::OpResult ResInST0>
void OpDispatchBuilder::FADD(OpcodeArgs) {
  static_assert(width == 16 || width == 32 || width == 64 || width == 80, "Unsupported FADD width");

  CurrentHeader->HasX87 = true;

  if (Op->Src[0].IsNone()) { // Implicit argument case
    auto offset = Op->OP & 7;
    auto st0 = 0;
    if constexpr (ResInST0 == OpResult::RES_STI) {
      _F80AddStack(offset, st0);
    } else {
      _F80AddStack(st0, offset);
    }
    return;
  }

  // We have one memory argument
  OrderedNode* arg {};

  if constexpr (width == 16 || width == 32 || width == 64) {
    if constexpr (Integer) {
      arg = LoadSource(GPRClass, Op, Op->Src[0], Op->Flags);
      arg = _F80CVTToInt(arg, width / 8);
    } else {
      arg = LoadSource(FPRClass, Op, Op->Src[0], Op->Flags);
      arg = _F80CVTTo(arg, width / 8);
    }
  }

  // top of stack is at offset zero
  _F80AddValue(0, arg);
}

template void OpDispatchBuilder::FADD<32, false, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);
template void OpDispatchBuilder::FADD<64, false, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);
template void OpDispatchBuilder::FADD<80, false, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);
template void OpDispatchBuilder::FADD<80, false, OpDispatchBuilder::OpResult::RES_STI>(OpcodeArgs);

template void OpDispatchBuilder::FADD<16, true, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);
template void OpDispatchBuilder::FADD<32, true, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);

template<size_t width, bool Integer, OpDispatchBuilder::OpResult ResInST0>
void OpDispatchBuilder::FMUL(OpcodeArgs) {
  static_assert(width == 16 || width == 32 || width == 64 || width == 80, "Unsupported FMUL width");

  CurrentHeader->HasX87 = true;
  if (Op->Src[0].IsNone()) { // Implicit argument case
    auto offset = Op->OP & 7;
    auto st0 = 0;
    if constexpr (ResInST0 == OpResult::RES_STI) {
      _F80MulStack(offset, st0);
    } else {
      _F80MulStack(st0, offset);
    }
    return;
  }

  // We have one memory argument
  OrderedNode* arg {};

  if constexpr (width == 16 || width == 32 || width == 64) {
    if constexpr (Integer) {
      arg = LoadSource(GPRClass, Op, Op->Src[0], Op->Flags);
      arg = _F80CVTToInt(arg, width / 8);
    } else {
      arg = LoadSource(FPRClass, Op, Op->Src[0], Op->Flags);
      arg = _F80CVTTo(arg, width / 8);
    }
  }

  // top of stack is at offset zero
  _F80MulValue(0, arg);
}

template void OpDispatchBuilder::FMUL<32, false, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);
template void OpDispatchBuilder::FMUL<64, false, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);
template void OpDispatchBuilder::FMUL<80, false, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);
template void OpDispatchBuilder::FMUL<80, false, OpDispatchBuilder::OpResult::RES_STI>(OpcodeArgs);

template void OpDispatchBuilder::FMUL<16, true, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);
template void OpDispatchBuilder::FMUL<32, true, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);

template<size_t width, bool Integer, bool reverse, OpDispatchBuilder::OpResult ResInST0>
void OpDispatchBuilder::FSUB(OpcodeArgs) {
  static_assert(width == 16 || width == 32 || width == 64 || width == 80, "Unsupported FSUB width");
  CurrentHeader->HasX87 = true;

  if (Op->Src[0].IsNone()) {
    const auto offset = Op->OP & 7;
    const auto st0 = 0;

    if constexpr (reverse) {
      if constexpr (ResInST0 == OpResult::RES_STI) {
        _F80SubStack(offset, st0, offset);
      } else {
        _F80SubStack(st0, offset, st0);
      }
    } else {
      if constexpr (ResInST0 == OpResult::RES_STI) {
        _F80SubStack(offset, offset, st0);
      } else {
        _F80SubStack(st0, st0, offset);
      }
    }

    if (Op->TableInfo->Flags & X86Tables::InstFlags::FLAGS_POP) {
      _PopStackDestroy();
    }
    return;
  }

  // We have one memory argument
  OrderedNode* arg {};

  if constexpr (width == 16 || width == 32 || width == 64) {
    if constexpr (Integer) {
      arg = LoadSource(GPRClass, Op, Op->Src[0], Op->Flags);
      arg = _F80CVTToInt(arg, width / 8);
    } else {
      arg = LoadSource(FPRClass, Op, Op->Src[0], Op->Flags);
      arg = _F80CVTTo(arg, width / 8);
    }
  }

  // top of stack is at offset zero
  if constexpr (reverse) {
    _F80SubRValue(arg, 0);
  } else {
    _F80SubValue(0, arg);
  }

  if (Op->TableInfo->Flags & X86Tables::InstFlags::FLAGS_POP) {
    _PopStackDestroy();
  }
}

template void OpDispatchBuilder::FSUB<32, false, false, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);
template void OpDispatchBuilder::FSUB<32, false, true, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);

template void OpDispatchBuilder::FSUB<64, false, false, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);
template void OpDispatchBuilder::FSUB<64, false, true, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);

template void OpDispatchBuilder::FSUB<80, false, false, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);
template void OpDispatchBuilder::FSUB<80, false, true, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);

template void OpDispatchBuilder::FSUB<80, false, false, OpDispatchBuilder::OpResult::RES_STI>(OpcodeArgs);
template void OpDispatchBuilder::FSUB<80, false, true, OpDispatchBuilder::OpResult::RES_STI>(OpcodeArgs);

template void OpDispatchBuilder::FSUB<16, true, false, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);
template void OpDispatchBuilder::FSUB<16, true, true, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);

template void OpDispatchBuilder::FSUB<32, true, false, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);
template void OpDispatchBuilder::FSUB<32, true, true, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);

template<size_t width, bool Integer, bool reverse, OpDispatchBuilder::OpResult ResInST0>
void OpDispatchBuilder::FDIV(OpcodeArgs) {
  static_assert(width == 16 || width == 32 || width == 64 || width == 80, "Unsupported FSUB width");
  CurrentHeader->HasX87 = true;

  if (Op->Src[0].IsNone()) {
    const auto offset = Op->OP & 7;
    const auto st0 = 0;

    if constexpr (reverse) {
      if constexpr (ResInST0 == OpResult::RES_STI) {
        _F80DivStack(offset, st0, offset);
      } else {
        _F80DivStack(st0, offset, st0);
      }
    } else {
      if constexpr (ResInST0 == OpResult::RES_STI) {
        _F80DivStack(offset, offset, st0);
      } else {
        _F80DivStack(st0, st0, offset);
      }
    }

    if (Op->TableInfo->Flags & X86Tables::InstFlags::FLAGS_POP) {
      _PopStackDestroy();
    }
    return;
  }

  // We have one memory argument
  OrderedNode* arg {};

  if constexpr (width == 16 || width == 32 || width == 64) {
    if constexpr (Integer) {
      arg = LoadSource(GPRClass, Op, Op->Src[0], Op->Flags);
      arg = _F80CVTToInt(arg, width / 8);
    } else {
      arg = LoadSource(FPRClass, Op, Op->Src[0], Op->Flags);
      arg = _F80CVTTo(arg, width / 8);
    }
  }

  // top of stack is at offset zero
  if constexpr (reverse) {
    _F80DivRValue(arg, 0);
  } else {
    _F80DivValue(0, arg);
  }

  if (Op->TableInfo->Flags & X86Tables::InstFlags::FLAGS_POP) {
    _PopStackDestroy();
  }
}

template void OpDispatchBuilder::FDIV<32, false, false, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);
template void OpDispatchBuilder::FDIV<32, false, true, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);

template void OpDispatchBuilder::FDIV<64, false, false, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);
template void OpDispatchBuilder::FDIV<64, false, true, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);

template void OpDispatchBuilder::FDIV<80, false, false, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);
template void OpDispatchBuilder::FDIV<80, false, true, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);

template void OpDispatchBuilder::FDIV<80, false, false, OpDispatchBuilder::OpResult::RES_STI>(OpcodeArgs);
template void OpDispatchBuilder::FDIV<80, false, true, OpDispatchBuilder::OpResult::RES_STI>(OpcodeArgs);

template void OpDispatchBuilder::FDIV<16, true, false, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);
template void OpDispatchBuilder::FDIV<16, true, true, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);

template void OpDispatchBuilder::FDIV<32, true, false, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);
template void OpDispatchBuilder::FDIV<32, true, true, OpDispatchBuilder::OpResult::RES_ST0>(OpcodeArgs);

void OpDispatchBuilder::X87FNSTENV(OpcodeArgs) {
  // FIXME(pmatos): Unsure if this should be a single IR instruction, a bunch of them
  // or just lowered into memory stores here.

  // 14 bytes for 16bit
  // 2 Bytes : FCW
  // 2 Bytes : FSW
  // 2 bytes : FTW
  // 2 bytes : Instruction offset
  // 2 bytes : Instruction CS selector
  // 2 bytes : Data offset
  // 2 bytes : Data selector

  // 28 bytes for 32bit
  // 4 bytes : FCW
  // 4 bytes : FSW
  // 4 bytes : FTW
  // 4 bytes : Instruction pointer
  // 2 bytes : instruction pointer selector
  // 2 bytes : Opcode
  // 4 bytes : data pointer offset
  // 4 bytes : data pointer selector

  auto Size = GetDstSize(Op);
  OrderedNode* Mem = LoadSource(GPRClass, Op, Op->Dest, Op->Flags, {.LoadData = false});
  Mem = AppendSegmentOffset(Mem, Op->Flags);

  {
    auto FCW = _LoadContext(2, GPRClass, offsetof(FEXCore::Core::CPUState, FCW));
    _StoreMem(GPRClass, Size, Mem, FCW, Size);
  }

  {
    OrderedNode* MemLocation = _Add(OpSize::i64Bit, Mem, _Constant(Size * 1));
    _StoreMem(GPRClass, Size, MemLocation, ReconstructFSW(), Size);
  }

  auto ZeroConst = _Constant(0);

  {
    // FTW
    OrderedNode* MemLocation = _Add(OpSize::i64Bit, Mem, _Constant(Size * 2));
    _StoreMem(GPRClass, Size, MemLocation, GetX87FTW(), Size);
  }

  {
    // Instruction Offset
    OrderedNode* MemLocation = _Add(OpSize::i64Bit, Mem, _Constant(Size * 3));
    _StoreMem(GPRClass, Size, MemLocation, ZeroConst, Size);
  }

  {
    // Instruction CS selector (+ Opcode)
    OrderedNode* MemLocation = _Add(OpSize::i64Bit, Mem, _Constant(Size * 4));
    _StoreMem(GPRClass, Size, MemLocation, ZeroConst, Size);
  }

  {
    // Data pointer offset
    OrderedNode* MemLocation = _Add(OpSize::i64Bit, Mem, _Constant(Size * 5));
    _StoreMem(GPRClass, Size, MemLocation, ZeroConst, Size);
  }

  {
    // Data pointer selector
    OrderedNode* MemLocation = _Add(OpSize::i64Bit, Mem, _Constant(Size * 6));
    _StoreMem(GPRClass, Size, MemLocation, ZeroConst, Size);
  }
}

void OpDispatchBuilder::X87LDENV(OpcodeArgs) {
  // FIXME(pmatos): Same.

  auto Size = GetSrcSize(Op);
  OrderedNode* Mem = LoadSource(GPRClass, Op, Op->Src[0], Op->Flags, {.LoadData = false});
  Mem = AppendSegmentOffset(Mem, Op->Flags);

  auto NewFCW = _LoadMem(GPRClass, 2, Mem, 2);
  _StoreContext(2, GPRClass, NewFCW, offsetof(FEXCore::Core::CPUState, FCW));

  OrderedNode* MemLocation = _Add(OpSize::i64Bit, Mem, _Constant(Size * 1));
  auto NewFSW = _LoadMem(GPRClass, Size, MemLocation, Size);
  ReconstructX87StateFromFSW(NewFSW);

  {
    // FTW
    OrderedNode* MemLocation = _Add(OpSize::i64Bit, Mem, _Constant(Size * 2));
    SetX87FTW(_LoadMem(GPRClass, Size, MemLocation, Size));
  }
}

} // namespace FEXCore::IR