#include <nan.h>

#include "hash_32.hpp"
#include "hash_64.hpp"

NAN_MODULE_INIT(Init) {
    Hash32::Init(target);
    Hash64::Init(target);
}

NODE_MODULE(addon, Init)