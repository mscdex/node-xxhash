#include <node.h>
#include <node_buffer.h>
#include <nan.h>
#include <stdlib.h>

#include "xxhash.h"

using namespace v8;

static Nan::Persistent<FunctionTemplate> constructor;

class Hash : public node::ObjectWrap {
  public:
    XXH32_state_t state;

    Hash(unsigned int seed) {
      XXH32_reset(&state, seed);
    }

    ~Hash() {
    }

    static NAN_METHOD(New) {
      if (!info.IsConstructCall())
        return Nan::ThrowError("Use `new` to create instances of this object.");

      if (info.Length() == 0 || !info[0]->IsUint32())
        return Nan::ThrowTypeError("Expected unsigned integer seed argument");

      Hash* obj = new Hash(info[0]->Uint32Value());
      obj->Wrap(info.This());

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(Update) {
      Hash* obj = ObjectWrap::Unwrap<Hash>(info.This());

      if (!node::Buffer::HasInstance(info[0]))
        return Nan::ThrowTypeError("data argument must be a Buffer");

#if NODE_MAJOR_VERSION == 0 && NODE_MINOR_VERSION < 10
      Local<Object> data = info[0]->ToObject();
#else
      Local<Value> data = info[0];
#endif

      size_t buflen = node::Buffer::Length(data);
      /*if (buflen > 2147483647 || buflen == 0)
        return Nan::ThrowTypeError("data length must be 0 < n <= 2147483647");*/

      XXH32_update(&obj->state, node::Buffer::Data(data), buflen);

      info.GetReturnValue().SetUndefined();
    }

    static NAN_METHOD(Digest) {
      Hash* obj = ObjectWrap::Unwrap<Hash>(info.This());

      uint32_t result = XXH32_digest(&obj->state);

      info.GetReturnValue().Set(Nan::New<Integer>(result));
    }

    static NAN_METHOD(StaticHash) {
      if (info.Length() < 2)
        return Nan::ThrowTypeError("Expected data and seed arguments");

      if (!node::Buffer::HasInstance(info[0]))
        return Nan::ThrowTypeError("data argument must be a Buffer");
      else if (!info[1]->IsUint32())
        return Nan::ThrowTypeError("seed argument must be an unsigned integer");

#if NODE_MAJOR_VERSION == 0 && NODE_MINOR_VERSION < 10
      Local<Object> data = info[0]->ToObject();
#else
      Local<Value> data = info[0];
#endif

      size_t buflen = node::Buffer::Length(data);
      /*if (buflen > 2147483647 || buflen == 0)
        return Nan::ThrowTypeError("data length must be 0 < n <= 2147483647");*/

      uint32_t result = XXH32(node::Buffer::Data(data),
                              buflen,
                              info[1]->Uint32Value());

      info.GetReturnValue().Set(Nan::New<Integer>(result));
    }


    static void Initialize(Handle<Object> target) {
      Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);

      constructor.Reset(tpl);
      tpl->InstanceTemplate()->SetInternalFieldCount(1);
      tpl->SetClassName(Nan::New<String>("XXHash").ToLocalChecked());

      Nan::SetPrototypeMethod(tpl, "update", Update);
      Nan::SetPrototypeMethod(tpl, "digest", Digest);

      tpl->Set(Nan::New<String>("hash").ToLocalChecked(),
               Nan::New<FunctionTemplate>(StaticHash)->GetFunction());
      target->Set(Nan::New<String>("XXHash").ToLocalChecked(),
               tpl->GetFunction());
    }
};

extern "C" {
  void Init(Handle<Object> target) {
    Hash::Initialize(target);
  }

  NODE_MODULE(hash, Init);
}
