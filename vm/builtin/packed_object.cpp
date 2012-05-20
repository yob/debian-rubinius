#include "gc/gc.hpp"

#include "builtin/packed_object.hpp"
#include "builtin/object.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/class.hpp"
#include "builtin/symbol.hpp"
#include "builtin/array.hpp"

#include "object_utils.hpp"
#include "objectmemory.hpp"

namespace rubinius {
  Object* PackedObject::get_packed_ivar(STATE, Symbol* sym) {
    LookupTable* tbl = this->reference_class()->packed_ivar_info();
    bool found = false;

    Fixnum* which = try_as<Fixnum>(tbl->fetch(state, sym, &found));
    if(!found) {
      return get_table_ivar(state, sym);
    }

    Object* obj = body_as_array()[which->to_native()];
    if(obj == cUndef) return cNil;
    return obj;
  }

  Object* PackedObject::packed_ivar_defined(STATE, Symbol* sym) {
    LookupTable* tbl = this->reference_class()->packed_ivar_info();
    bool found = false;

    Fixnum* which = try_as<Fixnum>(tbl->fetch(state, sym, &found));
    if(!found) {
      return table_ivar_defined(state, sym);
    }

    Object* obj = body_as_array()[which->to_native()];
    if(obj == cUndef) return cFalse;
    return cTrue;
  }

  Object* PackedObject::set_packed_ivar(STATE, Symbol* sym, Object* val) {
    LookupTable* tbl = this->reference_class()->packed_ivar_info();
    bool found = false;

    Fixnum* which = try_as<Fixnum>(tbl->fetch(state, sym, &found));
    if(!found) {
      return set_table_ivar(state, sym, val);
    }

    body_as_array()[which->to_native()] = val;
    if(val->reference_p()) write_barrier(state, val);
    return val;
  }

  Object* PackedObject::packed_ivar_delete(STATE, Symbol* sym, bool* removed) {
    LookupTable* tbl = this->reference_class()->packed_ivar_info();
    bool found = false;

    Fixnum* which = try_as<Fixnum>(tbl->fetch(state, sym, &found));
    if(!found) {
      return del_table_ivar(state, sym, removed);
    }

    if(removed) *removed = true;

    Object* val = body_as_array()[which->to_native()];
    body_as_array()[which->to_native()] = cUndef;

    return val;
  }

  void PackedObject::add_packed_ivars(STATE, Array* ary) {
    LookupTable* tbl = this->reference_class()->packed_ivar_info();

    LookupTable::iterator i(tbl);

    while(i.advance()) {
      Object* key = i.key();
      if(Fixnum* which = try_as<Fixnum>(tbl->fetch(state, key))) {
        if(body_as_array()[which->to_native()] != cUndef) {
          ary->append(state, key);
        }
      }
    }
  }

  size_t PackedObject::Info::object_size(const ObjectHeader* obj) {
    return const_cast<ObjectHeader*>(obj)->reference_class()->packed_size();
  }

  namespace {
    inline size_t to_fields(size_t total) {
      return (total - sizeof(ObjectHeader)) / sizeof(Object*);
    }
  }

  void PackedObject::Info::mark(Object* obj, ObjectMark& mark) {
    PackedObject* po = reinterpret_cast<PackedObject*>(obj);

    size_t fields = to_fields(object_size(obj));
    Object** body = po->body_as_array();

    for(size_t i = 0; i < fields; i++) {
      if(Object* tmp = mark.call(body[i])) {
        mark.set(obj, &body[i], tmp);
      }
    }
  }

  void PackedObject::Info::show(STATE, Object* self, int level) {
    class_info(state, self, true);
    return;
  }

  void PackedObject::Info::show_simple(STATE, Object* self, int level) {
    class_info(state, self, true);
    return;
  }
}
