#include "Interface/IR/IREmitter.h"
#include "Interface/IR/PassManager.h"
#include <FEXCore/IR/IR.h>
#include <FEXCore/IR/IntrusiveIRList.h>
#include <FEXCore/Utils/Profiler.h>
#include <FEXCore/fextl/vector.h>

#include <memory>
#include <stdint.h>

namespace FEXCore::IR {
class X87StackOptimization final : public FEXCore::IR::Pass {
public:
  bool Run(IREmitter *IREmit) override;

private:
  struct StackMemberInfo {
    IR::OpSize SourceDataSize;       // Size of SourceDataNode
    IR::OpSize StackDataSize;        // Size of the loaded data (??? FIXME)
    IR::NodeID SourceDataNodeID;     // ID of the node
    IR::OrderedNode* SourceDataNode; // Reference to the value pushed to stack
    IR::OrderedNode* DataLoadNode;   // Reference to the IR node that loaded the data
    bool InterpretAsFloat {};        // True if this is a floating point value, false if integer
  };
  fextl::vector<StackMemberInfo> StackData{8};
};

bool X87StackOptimization::Run(IREmitter *IREmit) {
  FEXCORE_PROFILE_SCOPED("PassManager::x87StackOpt");

  bool Changed = false;
  auto CurrentIR = IREmit->ViewIR();
  auto *OriginalWriteCursor = IREmit->GetWriteCursor();

  auto *HeaderOp = CurrentIR.GetHeader();
  LOGMAN_THROW_AA_FMT(HeaderOp->Header.Op == OP_IRHEADER, "First op wasn't IRHeader");

  if (!HeaderOp->HasX87) {
    // If there is no x87 in this, just early exit.
    return false;
  }

  StackData.clear();

  for (auto [BlockNode, BlockHeader] : CurrentIR.GetBlocks()) {
    for (auto [CodeNode, IROp] : CurrentIR.GetCode(BlockNode)) {
      switch (IROp->Op) {
        case IR::OP_PUSHSTACK: {
          LogMan::Msg::DFmt("OP_PUSHSTACK\n");
          const auto *Op = IROp->C<IR::IROp_PushStack>();
          auto SourceNodeID = Op->X80Src.ID();
          auto *SourceNode = CurrentIR.GetNode(Op->X80Src);
          auto *SourceNodeOp = CurrentIR.GetOp<IROp_Header>(SourceNode);
          auto SourceNodeSize = SourceNodeOp->Size;
          StackData.emplace_back(StackMemberInfo {
            .SourceDataSize = IR::SizeToOpSize(SourceNodeSize),
            .StackDataSize = IR::SizeToOpSize(Op->LoadSize),
            .SourceDataNodeID = SourceNodeID,
            .SourceDataNode = SourceNode,
            .DataLoadNode = CodeNode,
            .InterpretAsFloat = Op->Float,
          });

          LogMan::Msg::DFmt("Stack depth at: {}", StackData.size());
          IREmit->Remove(CodeNode); // Remove PushStack - it's a nop, we just need to track the stack
          break;
        }
        case IR::OP_POPSTACKMEMORY: {
          LogMan::Msg::DFmt("OP_POPSTACKMEMORY\n");
          const auto *Op = IROp->C<IR::IROp_PopStackMemory>();
          const auto& StackMember = StackData.back();
          if (Op->Float == StackMember.InterpretAsFloat &&
              Op->StoreSize == StackMember.StackDataSize &&
              Op->StoreSize == StackMember.SourceDataSize) {
            LogMan::Msg::DFmt("Could optimize memcpy!");
          }

          IREmit->SetWriteCursor(CodeNode);

          auto *AddrNode = CurrentIR.GetNode(Op->Addr);
          if (StackMember.SourceDataSize == OpSize::i128Bit) {
            IREmit->_StoreMem(FPRClass, OpSize::i64Bit, AddrNode, StackMember.SourceDataNode, 1);
            auto NewLocation = IREmit->_Add(OpSize::i64Bit, AddrNode, IREmit->_Constant(8));
            IREmit->_VStoreVectorElement(OpSize::i128Bit, OpSize::i16Bit, StackMember.SourceDataNode, 4, NewLocation);
          }
          else {
            IREmit->_StoreMem(FPRClass, StackMember.SourceDataSize, AddrNode, StackMember.SourceDataNode, 1);
          }

          IREmit->Remove(StackMember.DataLoadNode);
          IREmit->Remove(CodeNode);

          StackData.pop_back();
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
          auto* ValueNode = CurrentIR.GetNode(Op->X80Src);

          auto StackOffset = Op->SrcStack1;
          const auto& StackMember = StackData[StackOffset];
          auto* StackNode = StackMember.SourceDataNode;

          IREmit->_F80Add(ValueNode, StackNode);
          IREmit->Remove(CodeNode);
          LogMan::Msg::DFmt("Stack depth at: {}", StackData.size());
          break;
        }
        default: break;
      }
    }
  }
  IREmit->SetWriteCursor(OriginalWriteCursor);

  return Changed;
}

fextl::unique_ptr<FEXCore::IR::Pass> CreateX87StackOptimizationPass() {
  return fextl::make_unique<X87StackOptimization>();
}


}
