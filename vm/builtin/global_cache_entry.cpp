#include "builtin/global_cache_entry.hpp"
#include "builtin/class.hpp"
#include "builtin/staticscope.hpp"

#include "ontology.hpp"

namespace rubinius {
  void GlobalCacheEntry::init(STATE) {
    GO(global_cache_entry).set(
        ontology::new_class(state, "GlobalCacheEntry",
          G(object), G(rubinius)));
  }

  GlobalCacheEntry* GlobalCacheEntry::create(STATE, Object *value,
                                             StaticScope* scope)
  {
    GlobalCacheEntry *entry =
      state->vm()->new_object_mature<GlobalCacheEntry>(G(global_cache_entry));

    entry->update(state, value, scope);
    return entry;
  }

  GlobalCacheEntry* GlobalCacheEntry::empty(STATE) {
    GlobalCacheEntry *entry =
      state->vm()->new_object_mature<GlobalCacheEntry>(G(global_cache_entry));

    entry->serial_ = -1;
    return entry;
  }

  bool GlobalCacheEntry::valid_p(STATE, StaticScope* scope) {
    return serial_ == state->shared().global_serial() &&
           scope_ == scope;
  }

  void GlobalCacheEntry::update(STATE, Object* val, StaticScope* sc) {
    value(state, val);
    scope(state, sc);
    serial_ = state->shared().global_serial();
  }
}

