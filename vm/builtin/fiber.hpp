#ifndef RBX_BUILTIN_FIBER
#define RBX_BUILTIN_FIBER

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "prelude.hpp"
#include "builtin/object.hpp"
#include "builtin/lookuptable.hpp"

#include "fiber_data.hpp"

namespace rubinius {
  class Fiber : public Object {
  public:
    const static object_type type = FiberType;

    enum Status {
      eSleeping, eRunning, eDead
    };

  private:
    Object* starter_; // slot
    Array* value_; // slot
    Fiber* prev_; // slot
    Exception* exception_; // slot
    LookupTable* locals_; // slot
    Status status_;

    bool root_;

    FiberData* data_;

  public:
    attr_accessor(starter, Object);
    attr_accessor(value, Array);
    attr_accessor(prev, Fiber);
    attr_accessor(exception, Exception);
    attr_accessor(locals, LookupTable);

    bool root_p() {
      return root_;
    }

    CallFrame* call_frame() {
      if(!data_) return 0;
      return data_->call_frame();
    }

    void sleep(CallFrame* cf) {
      if(cf && !data_) rubinius::bug("bad fiber");
      data_->set_call_frame(cf);
      status_ = eSleeping;
    }

    void run() {
      if(data_) data_->set_call_frame(0);
      status_ = eRunning;
    }

    fiber_context_t* ucontext() {
      return data_->machine();
    }

    FiberData* data() {
      return data_;
    }

    void* stack_region() {
      return data_->stack_address();
    }

    void* stack_end() {
      return data_->stack_address();
    }

    void* stack_start() {
      return (void*)((uintptr_t)stack_region() + stack_size());
    }

    int stack_size() {
      return data_->stack_size();
    }

    VariableRootBuffers& variable_root_buffers() {
      return data_->variable_root_buffers();
    }

  public:
    static void init(STATE);

    // Rubinius.primitive :fiber_new
    static Fiber* create(STATE, Object* self, Object* callable);
    static void start_on_stack();

    // Rubinius.primitive :fiber_s_current
    static Fiber* current(STATE);

    // Rubinius.primitive :fiber_resume
    Object* resume(STATE, Arguments& args, CallFrame* calling_environment);

    // Rubinius.primitive :fiber_transfer
    Object* transfer(STATE, Arguments& args, CallFrame* calling_environment);

    // Rubinius.primitive :fiber_s_yield
    static Object* s_yield(STATE, Arguments& args, CallFrame* calling_environment);

    static void finalize(STATE, Fiber* fib);

  public:   /* TypeInfo */

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void mark(Object* t, ObjectMark& mark);
    };
  };
}

#endif
