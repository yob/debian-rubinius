#ifdef ENABLE_LLVM

#include "llvm/jit_operations.hpp"
#include "llvm/types.hpp"

namespace rubinius {
namespace type {
  void KnownType::associate(LLVMState* ls, llvm::Instruction* I) {
    if(kind_ == eUnknown && source_ == eUnknownSource) return;

    llvm::MDNode* node;
    if(source_ != eUnknownSource) {
      llvm::Value *impMD[] = {
        ls->cint(kind_),
        ls->cint(value_),
        ls->cint(source_),
        ls->cint(source_id_)
      };

      node = llvm::MDNode::get(ls->ctx(), impMD);
    } else {
      llvm::Value *impMD[] = {
        ls->cint(kind_),
        ls->cint(value_)
      };

      node = llvm::MDNode::get(ls->ctx(), impMD);
    }

    I->setMetadata(ls->metadata_id(), node);
  }

  void KnownType::associate(LLVMState* ls, llvm::Value* V) {
    if(Instruction* I = dyn_cast<Instruction>(V)) {
      associate(ls, I);
    }
  }

  KnownType KnownType::extract(LLVMState* ls, llvm::Value* V) {
    if(Instruction* I = dyn_cast<Instruction>(V)) {
      return extract(ls, I);
    }

    return KnownType::unknown();
  }

  KnownType KnownType::extract(LLVMState* ls, llvm::Instruction* I) {
    if(llvm::MDNode* md = I->getMetadata(ls->metadata_id())) {
      if(md->getNumOperands() == 2) {
        ConstantInt* kind = dyn_cast<ConstantInt>(md->getOperand(0));
        ConstantInt* value = dyn_cast<ConstantInt>(md->getOperand(1));

        if(kind && value) {
          return KnownType((Kind)kind->getValue().getSExtValue(),
                           value->getValue().getSExtValue());
        }
      } else if(md->getNumOperands() == 4) {
        ConstantInt* kind = dyn_cast<ConstantInt>(md->getOperand(0));
        ConstantInt* value = dyn_cast<ConstantInt>(md->getOperand(1));
        ConstantInt* source = dyn_cast<ConstantInt>(md->getOperand(2));
        ConstantInt* source_id = dyn_cast<ConstantInt>(md->getOperand(3));

        if(kind && value && source && source_id) {
          return KnownType((Kind)kind->getValue().getSExtValue(),
                           value->getValue().getSExtValue(),
                           (Source)source->getValue().getSExtValue(),
                           source_id->getValue().getSExtValue());
        }
      }
    }

    return KnownType::unknown();
  }

  bool KnownType::has_hint(LLVMState* ls, llvm::Value* V) {
    if(Instruction* I = dyn_cast<Instruction>(V)) {
      if(I->getMetadata(ls->metadata_id())) {
        return true;
      }
    }

    return false;
  }

  void KnownType::inherit_source(LLVMState* ls, llvm::Value* V) {
    inherit_source(extract(ls, V));
  }

  void KnownType::inherit_source(KnownType kt) {
    if(kt.source() == eUnknownSource) return;

    source_ = kt.source();
    source_id_ = kt.source_id();
  }

  const char* KnownType::describe() {
    switch(kind_) {
    case eUnknown:
      return "<type: unknown>";
    case eTrue:
      return "<type: true>";
    case eFalse:
      return "<type: false>";
    case eNil:
      return "<type: nil>";
    case eFixnum:
      return "<type: fixnum>";
    case eStaticFixnum:
      return "<type: static fixnum>";
    case eInstance:
      return "<type: instance>";
    case eSingletonInstance:
      return "<type: singleton instance>";
    case eSymbol:
      return "<type: symbol>";
    case eType:
      return "<obj: Rubinius::Type>";
    case eClassObject:
      return "<obj: class>";
    }

    return "Confused type info!";
  }
}
}

#endif
