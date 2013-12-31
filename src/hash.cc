#include <node.h>
#include <node_buffer.h>
#include <stdlib.h>

#include "nan.h"
#include "xxhash.h"

using namespace v8;

static Persistent<FunctionTemplate> constructor;

class Hash : public node::ObjectWrap {
  public:
    void* state;

    Hash(unsigned int seed) {
      state = XXH32_init(seed);
    }

    ~Hash() {
      if (state) {
        free(state);
        state = NULL;
      }
    }

    static NAN_METHOD(New) {
      NanScope();

      if (!args.IsConstructCall()) {
        ThrowException(Exception::Error(String::New("Use `new` to create instances of this object.")));
      }

      if (args.Length() == 0 || !args[0]->IsUint32()) {
        ThrowException(Exception::TypeError(String::New("Expected unsigned integer seed argument")));
      }

      Hash* obj = new Hash(args[0]->Uint32Value());
      obj->Wrap(args.This());

      return args.This();
    }

    static NAN_METHOD(Update) {
      NanScope();
      Hash* obj = ObjectWrap::Unwrap<Hash>(args.This());

      if (!node::Buffer::HasInstance(args[0])) {
        ThrowException(Exception::TypeError(String::New("data argument must be a Buffer")));
      }

#if NODE_MAJOR_VERSION == 0 && NODE_MINOR_VERSION < 10
      Local<Object> data = args[0]->ToObject();
#else
      Local<Value> data = args[0];
#endif

      size_t buflen = node::Buffer::Length(data);
      if (buflen > 2147483647 || buflen == 0) {
        ThrowException(Exception::TypeError(String::New("data length must be 0 < n <= 2147483647")));
      }

      XXH32_feed(obj->state, node::Buffer::Data(data), buflen);

      NanReturnValue(Undefined());
    }

    static NAN_METHOD(Digest) {
      NanScope();
      Hash* obj = ObjectWrap::Unwrap<Hash>(args.This());

      unsigned int result = XXH32_result(obj->state);

      // XXH32_result() frees the state already
      obj->state = NULL;

      NanReturnValue(Integer::NewFromUnsigned(result));
    }

    static NAN_METHOD(StaticHash) {
      NanScope();

      if (args.Length() < 2) {
        ThrowException(Exception::Error(String::New("Expected data and seed arguments")));
      }

      if (!node::Buffer::HasInstance(args[0])) {
        ThrowException(Exception::TypeError(String::New("data argument must be a Buffer")));
      } else if (!args[1]->IsUint32()) {
        ThrowException(Exception::TypeError(String::New("seed argument must be an unsigned integer")));
      }

#if NODE_MAJOR_VERSION == 0 && NODE_MINOR_VERSION < 10
      Local<Object> data = args[0]->ToObject();
#else
      Local<Value> data = args[0];
#endif

      size_t buflen = node::Buffer::Length(data);
      if (buflen > 2147483647 || buflen == 0) {
        ThrowException(Exception::TypeError(String::New("data length must be 0 < n <= 2147483647")));
      }

      unsigned int result = XXH32(node::Buffer::Data(data),
                                  buflen,
                                  args[1]->Uint32Value());

      NanReturnValue(Integer::NewFromUnsigned(result));
    }


    static void Initialize(Handle<Object> target) {
      NanScope();

      Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
      Local<String> name = NanSymbol("XXHash");

      constructor = Persistent<FunctionTemplate>::New(tpl);
      constructor->InstanceTemplate()->SetInternalFieldCount(1);
      constructor->SetClassName(name);

      NODE_SET_PROTOTYPE_METHOD(constructor, "update", Update);
      NODE_SET_PROTOTYPE_METHOD(constructor, "digest", Digest);

      constructor->Set(NanSymbol("hash"),
                       FunctionTemplate::New(StaticHash)->GetFunction());

      target->Set(name, constructor->GetFunction());
    }
};

extern "C" {
  void Init(Handle<Object> target) {
    NanScope();
    Hash::Initialize(target);
  }

  NODE_MODULE(hash, Init);
}
