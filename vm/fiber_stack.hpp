#ifndef RBX_VM_FIBER_STACK_HPP
#define RBX_VM_FIBER_STACK_HPP

namespace rubinius {
  class FiberData;
  class GarbageCollector;

  class FiberStack {
    void* address_;
    size_t size_;
    int refs_;
    FiberData* user_;
#ifdef HAVE_VALGRIND_H
    unsigned valgrind_id_;
#endif

  public:
    FiberStack(size_t size);

    void* address() {
      return address_;
    }

    void* top_address() {
      return (void*)((char*)address_ + size_);
    }

    size_t size() {
      return size_;
    }

    void inc_ref() {
      refs_++;
    }

    void dec_ref() {
      refs_--;
    }

    int refs() {
      return refs_;
    }

    bool unused_p() {
      return refs_ == 0;
    }

    bool shared_p() {
      return user_ && refs_ > 1;
    }

    FiberData* user() {
      return user_;
    }

    void set_user(FiberData* d) {
      user_ = d;
    }

    void allocate();
    void free();
    void flush(STATE);
    void orphan(STATE, FiberData* user);
  };

  class FiberStacks {
  public:
    const static size_t cTrampolineSize = 4096;

  private:
    typedef std::list<FiberStack> Stacks;
    typedef std::list<FiberData*> Datas;

    size_t max_stacks_;
    size_t stack_size_;

    VM* thread_;
    Stacks stacks_;
    Datas datas_;
    void* trampoline_;

  public:
    FiberStacks(VM* thread, SharedState& shared);
    ~FiberStacks();

    FiberStack* allocate();

    void remove_data(FiberData* data) {
      datas_.remove(data);
    }

    FiberData* new_data(bool root=false);

    void* trampoline();

    void gc_scan(GarbageCollector* gc);
  };
}

#endif
