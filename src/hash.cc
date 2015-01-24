#include <node.h>
#include <node_buffer.h>
#include <nan.h>
#include <stdlib.h>

#include "xxhash.h"

using namespace v8;

static Persistent<FunctionTemplate> constructor;

class Hash : public node::ObjectWrap {
  public:
    XXH32_state_t state;

    Hash(unsigned int seed) {
      XXH32_reset(&state, seed);
    }

    ~Hash() {
    }

    static NAN_METHOD(New) {
      NanScope();

      if (!args.IsConstructCall())
        return NanThrowError("Use `new` to create instances of this object.");

      if (args.Length() == 0 || !args[0]->IsUint32())
        return NanThrowTypeError("Expected unsigned integer seed argument");

      Hash* obj = new Hash(args[0]->Uint32Value());
      obj->Wrap(args.This());

      NanReturnValue(args.This());
    }

    static NAN_METHOD(Update) {
      NanScope();
      Hash* obj = ObjectWrap::Unwrap<Hash>(args.This());

      if (!node::Buffer::HasInstance(args[0]))
        return NanThrowTypeError("data argument must be a Buffer");

#if NODE_MAJOR_VERSION == 0 && NODE_MINOR_VERSION < 10
      Local<Object> data = args[0]->ToObject();
#else
      Local<Value> data = args[0];
#endif

      size_t buflen = node::Buffer::Length(data);
      /*if (buflen > 2147483647 || buflen == 0)
        return NanThrowTypeError("data length must be 0 < n <= 2147483647");*/

      XXH32_update(&obj->state, node::Buffer::Data(data), buflen);

      NanReturnUndefined();
    }

    static NAN_METHOD(Digest) {
      NanScope();
      Hash* obj = ObjectWrap::Unwrap<Hash>(args.This());

      uint32_t result = XXH32_digest(&obj->state);

      NanReturnValue(NanNew<Integer>(result));
    }

    static NAN_METHOD(StaticHash) {
      NanScope();

      if (args.Length() < 2)
        return NanThrowTypeError("Expected data and seed arguments");

      if (!node::Buffer::HasInstance(args[0]))
        return NanThrowTypeError("data argument must be a Buffer");
      else if (!args[1]->IsUint32())
        return NanThrowTypeError("seed argument must be an unsigned integer");

#if NODE_MAJOR_VERSION == 0 && NODE_MINOR_VERSION < 10
      Local<Object> data = args[0]->ToObject();
#else
      Local<Value> data = args[0];
#endif

      size_t buflen = node::Buffer::Length(data);
      /*if (buflen > 2147483647 || buflen == 0)
        return NanThrowTypeError("data length must be 0 < n <= 2147483647");*/

      uint32_t result = XXH32(node::Buffer::Data(data),
                              buflen,
                              args[1]->Uint32Value());

      NanReturnValue(NanNew<Integer>(result));
    }


    static void Initialize(Handle<Object> target) {
      NanScope();

      Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

      NanAssignPersistent(constructor, tpl);
      tpl->InstanceTemplate()->SetInternalFieldCount(1);
      tpl->SetClassName(NanNew<String>("XXHash"));

      NODE_SET_PROTOTYPE_METHOD(tpl, "update", Update);
      NODE_SET_PROTOTYPE_METHOD(tpl, "digest", Digest);
      tpl->Set(NanNew<String>("hash"),
               NanNew<FunctionTemplate>(StaticHash)->GetFunction());

      target->Set(NanNew<String>("XXHash"), tpl->GetFunction());
    }
};

extern "C" {
  void Init(Handle<Object> target) {
    NanScope();
    Hash::Initialize(target);
  }

  NODE_MODULE(hash, Init);
}