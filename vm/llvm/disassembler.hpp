#ifndef VM_LLVM_DISASSEMBLER_HPP
#define VM_LLVM_DISASSEMBLER_HPP

#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <llvm/Support/TargetRegistry.h>
#include <llvm/MC/MCAsmInfo.h>
#include <llvm/MC/MCDisassembler.h>
#include <llvm/Support/MemoryObject.h>

namespace rubinius {

  class JITMemoryObject: public llvm::MemoryObject {

    public:

      JITMemoryObject(const uint8_t* buffer, uint64_t size)
        : buffer_(buffer)
        , size_(size)
      {}

      uint64_t getOffset() const {
        return (uint64_t) buffer_;
      }

      uint64_t getBase() const {
        return 0;
      }

      uint64_t getExtent() const {
        return size_;
      }

      int readByte(uint64_t addr, uint8_t *byte) const {
        if(addr >= size_) {
          return -1;
        }
        *byte = buffer_[addr];
        return 0;
      }

      int readBytes(uint64_t offset, uint64_t size, uint8_t* buffer, uint64_t* copied) const {
        // Cannot read passed the maximum buffer size
        if(offset >= size_) {
          return -1;
        }
        // If the offset + requested size is bigger than the
        // requested size, we clamp it at the maximum size from the offset.
        if((offset + size) > size_) {
          size = size_ - offset;
        }
        memcpy(buffer, buffer_ + offset, size);
        if(copied) {
          *copied = size;
        }
        return 0;
      }

    private:
      const uint8_t* buffer_;
      uint64_t size_;

  };

  class JITDisassembler {
    public:
      JITDisassembler(void* buffer, size_t size);
      ~JITDisassembler();
      std::string print_machine_code();
    private:

      const llvm::Target* target;
      const llvm::TargetMachine* target_machine;
      const llvm::MCSubtargetInfo* sub_target;
      const llvm::MCAsmInfo* asm_info;
#if RBX_LLVM_API_VER > 300
      const llvm::MCInstrInfo* instr_info;
      const llvm::MCRegisterInfo* reg_info;
#endif

      llvm::MCDisassembler* disassembler;
      const JITMemoryObject* memory_object;
  };

}

#endif
