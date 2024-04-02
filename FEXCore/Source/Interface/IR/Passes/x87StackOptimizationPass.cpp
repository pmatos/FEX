#include "Interface/IR/IREmitter.h"
#include "Interface/IR/PassManager.h"
#include <FEXCore/IR/IR.h>
#include <FEXCore/Utils/Profiler.h>
#include <FEXCore/fextl/vector.h>

#include <memory>
#include <stdint.h>

namespace FEXCore::IR {

template<typename T>
class CircularBuffer {
private:
  using StorageType = typename std::aligned_storage<sizeof(T), alignof(T)>::type;
  fextl::vector<StorageType> buffer;
  fextl::vector<bool> constructed; // TODO(pmatos): We probably can use something better here
  int index;                       // Current insertion index

public:
  CircularBuffer(std::size_t size)
    : buffer(size)
    , constructed(size, false)
    , index(0) {}

  ~CircularBuffer() {
    for (std::size_t i = 0; i < size(); ++i) {
      if (constructed[i]) {
        reinterpret_cast<T*>(&buffer[i])->~T();
      }
    }
  }

  template<typename... Args>
  void push(Args&&... args) {
    index = (index - 1 + size()) % size();
    std::size_t pos = index;
    if (constructed[pos]) {
      reinterpret_cast<T*>(&buffer[pos])->~T();
    }
    LogMan::Msg::DFmt("Push to {}\n", index);
    new (&buffer[pos]) T(std::forward<Args>(args)...);
    constructed[pos] = true;
  }

  template<typename... Args>
  void setTop(Args&&... args) {
    std::size_t pos = index;
    if (constructed[pos]) {
      reinterpret_cast<T*>(&buffer[pos])->~T();
    }
    LogMan::Msg::DFmt("SetTop to {}\n", index);
    new (&buffer[pos]) T(std::forward<Args>(args)...);
    constructed[pos] = true;
  }

  void pop() {
    if (!constructed.empty() && constructed[index]) {
      LogMan::Msg::DFmt("Pop\n");
      std::size_t popIndex = (index + 1) % size();
      reinterpret_cast<T*>(&buffer[popIndex])->~T();
      constructed[popIndex] = false;
      index = popIndex;
    }
  }

  T& top() {
    LogMan::Msg::DFmt("Top\n");
    std::size_t pos = index;
    return *reinterpret_cast<T*>(&buffer[pos]);
  }

  const T& top(size_t offset = 0) const {
    size_t pos = index;
    return *reinterpret_cast<const T*>(&buffer[(pos + offset) % size()]);
  }

  inline size_t count() const {
    size_t sz = 0;
    for (size_t i = 0; i < constructed.size(); ++i) {
      if (constructed[i]) {
        sz++;
      }
    }
    LogMan::Msg::DFmt("Count: {}\n", sz);
    return sz;
  }

  inline size_t size() const {
    return constructed.size();
  }

  inline T& operator[](size_t i) {
    return *reinterpret_cast<T*>(&buffer[i]);
  }

  inline const T& operator[](size_t i) const {
    return *reinterpret_cast<const T*>(&buffer[i]);
  }

  inline void clear() {
    for (std::size_t i = 0; i < size(); ++i) {
      if (constructed[i]) {
        reinterpret_cast<T*>(&buffer[i])->~T();
        constructed[i] = false;
      }
    }
    index = 0;
  }

  inline bool valid(size_t i) const {
    return constructed[i];
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
  OrderedNode* GetX87Top(IREmitter* IREmit);
  void SetX87Top(IREmitter* IREmit, OrderedNode* Value);
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
  CircularBuffer<StackMemberInfo> StackData {8};
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

  // Before any optimizations we need to update our StackData to match the
  // status at the beginning of the block. We need to load the values from the
  // context to the stack. We'll do this by checking which values to load
  // through the x87 tag register.
  // TODO(pmatos)

  // Get beginning of block
  // FIXME(pmatos): there must be a better way to do this.
  auto [BlockBegin, BlockBeginHeader] = *CurrentIR.GetBlocks().begin();
  auto [CodeBegin, IROpBegin] = *CurrentIR.GetCode(BlockBegin).begin();

  // Get Top at beginning of block
  IREmit->SetWriteCursor(CodeBegin);
  auto* orig_top = GetX87Top(IREmit);

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

        LogMan::Msg::DFmt("Stack depth at: {}", StackData.count());
        IREmit->SetWriteCursor(CodeNode);
        IREmit->Remove(CodeNode); // Remove PushStack - it's a nop, we just need to track the stack
        Changed = true;
        break;
      }
      case IR::OP_POPSTACKMEMORY: {
        LogMan::Msg::DFmt("OP_POPSTACKMEMORY\n");
        const auto* Op = IROp->C<IR::IROp_PopStackMemory>();
        const auto& StackMember = StackData.top();
        if (Op->Float == StackMember.InterpretAsFloat && Op->StoreSize == StackMember.StackDataSize && Op->StoreSize == StackMember.SourceDataSize) {
          LogMan::Msg::DFmt("Could optimize memcpy!");
        }

        IREmit->SetWriteCursor(CodeNode);

        auto* AddrNode = CurrentIR.GetNode(Op->Addr);
        if (StackMember.SourceDataSize == OpSize::i128Bit) {
          IREmit->_StoreMem(FPRClass, OpSize::i64Bit, AddrNode, StackMember.SourceDataNode, 1);
          auto NewLocation = IREmit->_Add(OpSize::i64Bit, AddrNode, IREmit->_Constant(8));
          IREmit->_VStoreVectorElement(OpSize::i128Bit, OpSize::i16Bit, StackMember.SourceDataNode, 4, NewLocation);
        } else {
          IREmit->_StoreMem(FPRClass, StackMember.SourceDataSize, AddrNode, StackMember.SourceDataNode, 1);
        }

        IREmit->Remove(StackMember.DataLoadNode);
        IREmit->Remove(CodeNode);
        Changed = true;

        StackData.pop();
        LogMan::Msg::DFmt("Stack depth at: {}", StackData.count());
        break;
      }
      case IR::OP_F80ADDSTACK: {
        LogMan::Msg::DFmt("OP_F80ADDSTACK\n");
        const auto* Op = IROp->C<IR::IROp_F80AddStack>();
        (void)Op; // avoid warning for now
        LogMan::Msg::DFmt("Stack depth at: {}", StackData.count());
        break;
      }
      case IR::OP_F80ADDVALUE: {
        LogMan::Msg::DFmt("F80ADDVALUE\n");
        const auto* Op = IROp->C<IR::IROp_F80AddValue>();
        auto SourceNodeID = Op->X80Src.ID();
        auto* ValueNode = CurrentIR.GetNode(Op->X80Src);

        auto StackOffset = Op->SrcStack1;
        const auto& StackMember = StackData.top(StackOffset);
        auto* StackNode = StackMember.SourceDataNode;

        IREmit->SetWriteCursor(CodeNode);

        auto AddNode = IREmit->_F80Add(ValueNode, StackNode);
        // Store it in the stack
        StackData.setTop(StackMemberInfo {.SourceDataSize = StackMember.SourceDataSize,
                                          .StackDataSize = StackMember.StackDataSize,
                                          .SourceDataNodeID = SourceNodeID,
                                          .SourceDataNode = AddNode,
                                          .DataLoadNode = CodeNode,
                                          .InterpretAsFloat = StackMember.InterpretAsFloat});

        IREmit->Remove(CodeNode);
        Changed = true;
        LogMan::Msg::DFmt("Stack depth at: {}", StackData.count());
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

        // Before leaving we need to write the current values in the stack to
        // context so that the values are correct. Copy SourceDataNode in the
        // stack to the respective mmX register.
        for (size_t i = 0; i < StackData.size(); ++i) {
          if (!StackData.valid(i)) {
            continue;
          }
          LogMan::Msg::DFmt("Writing stack member {} to context", i);
          Changed = true;
          auto &StackMember = StackData[i];
          auto *Node = StackMember.SourceDataNode;
          IREmit->_StoreContextIndexed(Node, IREmit->_Add(OpSize::i32Bit, orig_top, IREmit->_Constant(i)), 16, MMBaseOffset(), 16, FPRClass);
        }

        // Store new top which is now the original top - the number of elements in stack.
        // Careful with underflow wraparound.
        auto mask = IREmit->_Constant(0x7);
        auto new_top = IREmit->_And(OpSize::i32Bit, IREmit->_Sub(OpSize::i32Bit, orig_top, IREmit->_Constant(1)), mask);
        SetX87ValidTag(IREmit, new_top, true);
        SetX87Top(IREmit, new_top);

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
