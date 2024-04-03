#include "FEXCore/Utils/LogManager.h"
#include "Interface/IR/IREmitter.h"
#include "Interface/IR/PassManager.h"
#include <FEXCore/IR/IR.h>
#include <FEXCore/Utils/Profiler.h>
#include <FEXCore/fextl/vector.h>

#include <memory>
#include <optional>
#include <stdint.h>

namespace FEXCore::IR {


// FixedSizeStack is a model of the x87 Stack where each element in this
// fixed size stack lives at an offset from top. The top of the stack is at
// index 0.
template<typename T, size_t MaxSize>
class FixedSizeStack {
private:
  fextl::vector<T> buffer;

public:
  FixedSizeStack() {
    buffer.reserve(MaxSize);
  }

  void push(T&& value) {
    if (buffer.size() == MaxSize) {
      buffer.pop_back();
    }
    buffer.insert(buffer.begin(), std::move(value));
  }

  void pop() {
    if (!buffer.empty()) {
      buffer.erase(buffer.begin());
    }
  }

  std::optional<T> top(size_t offset = 0) const {
    if (!buffer.empty()) {
      return buffer[offset];
    }
    return std::nullopt;
  }

  void setTop(T&& value, size_t offset = 0) {
    if (!buffer.empty()) {
      buffer[offset] = std::move(value);
    }
  }

  inline size_t size() const {
    return buffer.size();
  }

  inline T& operator[](size_t i) {
    return buffer[i];
  }

  inline const T& operator[](size_t i) const {
    return buffer[i];
  }

  inline void clear() {
    buffer.clear();
  }
};

class X87StackOptimization final : public FEXCore::IR::Pass {
public:
  bool Run(IREmitter* IREmit) override;

private:
  // FIXME(pmatos): copy from OpcodeDispatcher.h
  [[nodiscard]]
  uint32_t MMBaseOffset() {
    return static_cast<uint32_t>(offsetof(Core::CPUState, mm[0][0]));
  }

  // Top Management Helpers
  // FIXME(pmatos): copy from X87.h
  /// Returns current Top value.
  OrderedNode* GetX87Top(IREmitter* IREmit);
  /// Sets Top value to Value.
  void SetX87Top(IREmitter* IREmit, OrderedNode* Value);
  /// Set the valid tag for Value as valid (if Valid is true), or invalid (if Valid is false).
  void SetX87ValidTag(IREmitter* IREmit, OrderedNode* Value, bool Valid);

  struct StackMemberInfo {
    IR::OpSize SourceDataSize;       // Size of SourceDataNode
    IR::OpSize StackDataSize;        // Size of the loaded data (??? FIXME)
    IR::NodeID SourceDataNodeID;     // ID of the node
    IR::OrderedNode* SourceDataNode; // Reference to the value pushed to stack
    IR::OrderedNode* DataLoadNode;   // Reference to the IR node that loaded the data
    bool InterpretAsFloat {};        // True if this is a floating point value, false if integer
  };
  // Index on vector is offset to top value at start of block
  FixedSizeStack<StackMemberInfo, 8> StackData;
};

OrderedNode* X87StackOptimization::GetX87Top(IREmitter* IREmit) {
  return IREmit->_LoadContext(1, GPRClass, offsetof(FEXCore::Core::CPUState, flags) + FEXCore::X86State::X87FLAG_TOP_LOC);
}

void X87StackOptimization::SetX87Top(IREmitter* IREmit, OrderedNode* Value) {
  IREmit->_StoreContext(1, GPRClass, Value, offsetof(FEXCore::Core::CPUState, flags) + FEXCore::X86State::X87FLAG_TOP_LOC);
}

void X87StackOptimization::SetX87ValidTag(IREmitter* IREmit, OrderedNode* Value, bool Valid) {
  // if we are popping then we must first mark this location as empty
  OrderedNode* AbridgedFTW = IREmit->_LoadContext(1, GPRClass, offsetof(FEXCore::Core::CPUState, AbridgedFTW));
  OrderedNode* RegMask = IREmit->_Lshl(OpSize::i32Bit, IREmit->_Constant(1), Value);
  OrderedNode* NewAbridgedFTW = Valid ? IREmit->_Or(OpSize::i32Bit, AbridgedFTW, RegMask) : IREmit->_Andn(OpSize::i32Bit, AbridgedFTW, RegMask);
  IREmit->_StoreContext(1, GPRClass, NewAbridgedFTW, offsetof(FEXCore::Core::CPUState, AbridgedFTW));
}

bool X87StackOptimization::Run(IREmitter* IREmit) {
  FEXCORE_PROFILE_SCOPED("PassManager::x87StackOpt");

  bool Changed = false;
  auto CurrentIR = IREmit->ViewIR();
  auto* OriginalWriteCursor = IREmit->GetWriteCursor();

  auto* HeaderOp = CurrentIR.GetHeader();
  LOGMAN_THROW_AA_FMT(HeaderOp->Header.Op == OP_IRHEADER, "First op wasn't IRHeader");

  if (!HeaderOp->HasX87) {
    // If there is no x87 in this, just early exit.
    return false;
  }

  StackData.clear();

  // Run optimization proper
  for (auto [BlockNode, BlockHeader] : CurrentIR.GetBlocks()) {
    for (auto [CodeNode, IROp] : CurrentIR.GetCode(BlockNode)) {
      switch (IROp->Op) {
      case IR::OP_PUSHSTACK: {
        LogMan::Msg::DFmt("OP_PUSHSTACK\n");
        const auto* Op = IROp->C<IR::IROp_PushStack>();
        auto SourceNodeID = Op->X80Src.ID();
        auto* SourceNode = CurrentIR.GetNode(Op->X80Src);
        auto* SourceNodeOp = CurrentIR.GetOp<IROp_Header>(SourceNode);
        auto SourceNodeSize = SourceNodeOp->Size;
        StackData.push(StackMemberInfo {
          .SourceDataSize = IR::SizeToOpSize(SourceNodeSize),
          .StackDataSize = IR::SizeToOpSize(Op->LoadSize),
          .SourceDataNodeID = SourceNodeID,
          .SourceDataNode = SourceNode,
          .DataLoadNode = CodeNode,
          .InterpretAsFloat = Op->Float,
        });

        LogMan::Msg::DFmt("Stack depth at: {}", StackData.size());
        IREmit->SetWriteCursor(CodeNode);
        IREmit->Remove(CodeNode); // Remove PushStack - it's a nop, we just need to track the stack
        Changed = true;
        break;
      }
      case IR::OP_POPSTACKMEMORY: {
        LogMan::Msg::DFmt("OP_POPSTACKMEMORY\n");
        const auto* Op = IROp->C<IR::IROp_PopStackMemory>();
        const auto& StackMember = StackData.top();
        LOGMAN_THROW_A_FMT(StackMember != std::nullopt, "Stack is empty");
        if (Op->Float == StackMember->InterpretAsFloat && Op->StoreSize == StackMember->StackDataSize &&
            Op->StoreSize == StackMember->SourceDataSize) {
          LogMan::Msg::DFmt("Could optimize memcpy!");
        }

        IREmit->SetWriteCursor(CodeNode);

        auto* AddrNode = CurrentIR.GetNode(Op->Addr);
        if (StackMember->SourceDataSize == OpSize::i128Bit) {
          IREmit->_StoreMem(FPRClass, OpSize::i64Bit, AddrNode, StackMember->SourceDataNode, 1);
          auto NewLocation = IREmit->_Add(OpSize::i64Bit, AddrNode, IREmit->_Constant(8));
          IREmit->_VStoreVectorElement(OpSize::i128Bit, OpSize::i16Bit, StackMember->SourceDataNode, 4, NewLocation);
        } else {
          IREmit->_StoreMem(FPRClass, StackMember->SourceDataSize, AddrNode, StackMember->SourceDataNode, 1);
        }

        IREmit->Remove(StackMember->DataLoadNode);
        IREmit->Remove(CodeNode);
        Changed = true;

        StackData.pop();
        LogMan::Msg::DFmt("Stack depth at: {}", StackData.size());
        break;
      }
      case IR::OP_F80ADDSTACK: {
        LogMan::Msg::DFmt("OP_F80ADDSTACK\n");
        const auto* Op = IROp->C<IR::IROp_F80AddStack>();
        (void)Op; // avoid warning for now
        LogMan::Msg::DFmt("Stack depth at: {}", StackData.size());
        break;
      }
      case IR::OP_F80ADDVALUE: {
        LogMan::Msg::DFmt("F80ADDVALUE\n");
        const auto* Op = IROp->C<IR::IROp_F80AddValue>();
        auto SourceNodeID = Op->X80Src.ID();
        auto* ValueNode = CurrentIR.GetNode(Op->X80Src);

        auto StackOffset = Op->SrcStack1;
        const auto& StackMember = StackData.top(StackOffset);
        LOGMAN_THROW_A_FMT(StackMember != std::nullopt, "Stack is empty");
        auto* StackNode = StackMember->SourceDataNode;

        IREmit->SetWriteCursor(CodeNode);

        auto AddNode = IREmit->_F80Add(ValueNode, StackNode);
        // Store it in the stack
        StackData.setTop(StackMemberInfo {.SourceDataSize = StackMember->SourceDataSize,
                                          .StackDataSize = StackMember->StackDataSize,
                                          .SourceDataNodeID = SourceNodeID,
                                          .SourceDataNode = AddNode,
                                          .DataLoadNode = CodeNode,
                                          .InterpretAsFloat = StackMember->InterpretAsFloat});

        IREmit->Remove(CodeNode);
        Changed = true;
        LogMan::Msg::DFmt("Stack depth at: {}", StackData.size());
        break;
      }
      default: break;
      }
    }
  }

  // FIXME(pmatos) there's probably a better way to do this
  // TODO(pmatos): we don't need to do this if we don't have any followup
  // blocks. How can we check that? OTOH, not writing to the proper registers
  // might screw up testing that expects the values to be in the stack registers
  // at the end, so maybe we need a testing flag that forces the writing of this
  // data to the context.
  for (auto [BlockNode, BlockHeader] : CurrentIR.GetBlocks()) {
    for (auto [CodeNode, IROp] : CurrentIR.GetCode(BlockNode)) {
      if (IROp->Op == OP_ENTRYPOINTOFFSET) {
        LogMan::Msg::DFmt("OP_ENTRYPOINTOFFSET\n");
        // Set write cursor to previous instruction
        IREmit->SetWriteCursor(IREmit->UnwrapNode(CodeNode->Header.Previous));

        // Store new top which is now the original top - the number of elements in stack.
        // Careful with underflow wraparound.
        auto* orig_top = GetX87Top(IREmit);
        auto mask = IREmit->_Constant(0x7);
        auto new_top = IREmit->_And(OpSize::i32Bit, IREmit->_Sub(OpSize::i32Bit, orig_top, IREmit->_Constant(1)), mask);
        SetX87ValidTag(IREmit, new_top, true);
        SetX87Top(IREmit, new_top);

        // Before leaving we need to write the current values in the stack to
        // context so that the values are correct. Copy SourceDataNode in the
        // stack to the respective mmX register.
        for (size_t i = 0; i < StackData.size(); ++i) {
          LogMan::Msg::DFmt("Writing stack member {} to context TOP+{}", i, i);
          Changed = true;
          const auto StackMember = StackData.top(i);
          LOGMAN_THROW_A_FMT(StackMember != std::nullopt, "Stack does not have enough elements");
          auto* Node = StackMember->SourceDataNode;
          IREmit->_StoreContextIndexed(Node, IREmit->_Add(OpSize::i32Bit, new_top, IREmit->_Constant(i)), 16, MMBaseOffset(), 16, FPRClass);
        }

        break;
      }
    }
  }

  IREmit->SetWriteCursor(OriginalWriteCursor);
  return Changed;
}

fextl::unique_ptr<FEXCore::IR::Pass> CreateX87StackOptimizationPass() {
  return fextl::make_unique<X87StackOptimization>();
}
} // namespace FEXCore::IR
