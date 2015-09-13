#include <node.h>

#include "hash_32.hpp"
#include "hash_64.hpp"

extern "C" {
  void Init(v8::Handle<v8::Object> target) {
    Hash32::Initialize(target);
    Hash64::Initialize(target);
  }

  NODE_MODULE(hash, Init);
}
