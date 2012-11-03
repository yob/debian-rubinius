#include "stack_variables.hpp"
#include "builtin/variable_scope.hpp"
#include "machine_code.hpp"
#include "call_frame.hpp"

namespace rubinius {

  VariableScope* StackVariables::create_heap_alias(STATE, CallFrame* call_frame,
                                                   bool full)
  {
    if(on_heap_) return on_heap_;

    MachineCode* mcode = call_frame->compiled_code->machine_code();
    VariableScope* scope = state->new_object<VariableScope>(G(variable_scope));

    if(parent_) {
      scope->parent(state, parent_);
    } else {
      scope->parent(state, nil<VariableScope>());
    }

    scope->self(state, self_);
    scope->block(state, block_);
    scope->module(state, module_);
    scope->method(state, call_frame->compiled_code);
    scope->heap_locals(state, Tuple::create(state, mcode->number_of_locals));
    scope->last_match(state, last_match_);
    scope->fiber(state, state->vm()->current_fiber.get());

    scope->number_of_locals_ = mcode->number_of_locals;

    if(full) {
      scope->isolated_ = false;
    } else {
      scope->isolated_ = true;
    }

    scope->locals_ = locals_;

    scope->set_block_as_method(call_frame->block_as_method_p());

    on_heap_ = scope;

    return scope;
  }

  void StackVariables::set_last_match(STATE, Object* obj) {
    // For closures, get back to the top of the chain and set the
    // last_match there. This means that the last_match is shared
    // amongst all closures in a method, but thats how it's implemented
    // in ruby.
    if(parent_) {
      VariableScope* scope = parent_;
      while(CBOOL(scope->parent())) {
        scope = scope->parent();
      }

      return scope->last_match(state, obj);
    }

    // Use a heap alias if there is one.
    if(on_heap_) {
      on_heap_->last_match(state, obj);

    // Otherwise, use the local one. This is where a last_match usually
    // first appears.
    } else {
      last_match_ = obj;
    }
  }

  Object* StackVariables::last_match(STATE) {
    // For closures, get back to the top of the chain and get that
    // last_match.
    if(parent_) {
      VariableScope* scope = parent_;
      while(CBOOL(scope->parent())) {
        scope = scope->parent();
      }

      return scope->last_match();
    }

    // Otherwise, if this has a heap alias, get the last_match from there.
    if(on_heap_) {
      return on_heap_->last_match();

    // Lastly, use the local one. This is where a last_match begins life.
    } else {
      return last_match_;
    }
  }

  void StackVariables::flush_to_heap(STATE) {
    if(!on_heap_) return;

    on_heap_->isolated_ = true;

    for(int i = 0; i < on_heap_->number_of_locals_; i++) {
      on_heap_->set_local(state, i, locals_[i]);
    }
  }
}
