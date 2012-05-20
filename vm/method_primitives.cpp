#include "prelude.hpp"
#include "vm.hpp"
#include "primitives.hpp"
#include "gen/includes.hpp"
#include "arguments.hpp"
#include "call_frame.hpp"

#include "instruments/tooling.hpp"

#include <iostream>

#ifdef ENABLE_LLVM
#include "llvm/state.hpp"
#endif

namespace rubinius {

  Object* Primitives::unknown_primitive(STATE, CallFrame* call_frame, Executable* exec, Module* mod, Arguments& args) {
    std::string message = std::string("Called unbound or invalid primitive from method name: ");
    message += args.name()->to_str(state)->c_str(state);

    Exception::assertion_error(state, message.c_str());

    return cNil;
  }

#ifdef ENABLE_LLVM
  void Primitives::queue_for_jit(STATE, CallFrame* call_frame, int which) {
    // LLVMState* ls = LLVMState::get(state);
    // ls->compile_callframe(state, 0, call_frame, which);
  }
#endif

  static inline void check_counter(STATE, CallFrame* call_frame, int which) {
#ifdef ENABLE_LLVM
    int& hits = state->shared().primitive_hits(which);
    if(hits >= 0 && ++hits >= state->shared().config.jit_call_til_compile) {
      hits = -1;
      Primitives::queue_for_jit(state, call_frame, which);
    }
#endif
  }

#include "gen/method_primitives.cpp"
#include "gen/method_resolver.cpp"
}
