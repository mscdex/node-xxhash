#include <node.h>
#include <node_buffer.h>
#include <stdlib.h>

#include "xxhash.h"

using namespace node;
using namespace v8;

static Persistent<FunctionTemplate> constructor;

class Hash : public ObjectWrap {
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

    static Handle<Value> New(const Arguments& args) {
      HandleScope scope;

      if (!args.IsConstructCall()) {
        return ThrowException(Exception::Error(
            String::New("Use `new` to create instances of this object."))
        );
      }

      if (args.Length() == 0 || !args[0]->IsUint32()) {
        return ThrowException(Exception::TypeError(
            String::New("Expected unsigned integer seed argument"))
        );
      }

      Hash* obj = new Hash(args[0]->Uint32Value());
      obj->Wrap(args.This());

      return args.This();
    }

    static Handle<Value> Update(const Arguments& args) {
      HandleScope scope;
      Hash* obj = ObjectWrap::Unwrap<Hash>(args.This());

      if (!Buffer::HasInstance(args[0])) {
        return ThrowException(Exception::TypeError(
            String::New("data argument must be a Buffer"))
        );
      }

      Local<Object> data = args[0]->ToObject();

      if (Buffer::Length(data) > 2147483647) {
        return ThrowException(Exception::TypeError(
            String::New("data length must be 0 <= n <= 2147483647"))
        );
      }

      XXH32_feed(obj->state, Buffer::Data(data), Buffer::Length(data));

      return scope.Close(Undefined());
    }

    static Handle<Value> Digest(const Arguments& args) {
      HandleScope scope;
      Hash* obj = ObjectWrap::Unwrap<Hash>(args.This());

      unsigned int result = XXH32_result(obj->state);

      // XXH32_result() frees the state already
      obj->state = NULL;

      return scope.Close(Integer::NewFromUnsigned(result));
    }

    static Handle<Value> StaticHash(const Arguments& args) {
      HandleScope scope;

      if (args.Length() < 2) {
        return ThrowException(Exception::Error(
            String::New("Expected data and seed arguments"))
        );
      }

      if (!Buffer::HasInstance(args[0])) {
        return ThrowException(Exception::TypeError(
            String::New("data argument must be a Buffer"))
        );
      } else if (!args[1]->IsUint32()) {
        return ThrowException(Exception::TypeError(
            String::New("seed argument must be an unsigned integer"))
        );
      }

      Local<Object> data = args[0]->ToObject();

      if (Buffer::Length(data) > 2147483647) {
        return ThrowException(Exception::TypeError(
            String::New("data length must be 0 <= n <= 2147483647"))
        );
      }

      unsigned int result = XXH32(Buffer::Data(data),
                                  Buffer::Length(data),
                                  args[1]->Uint32Value());

      return scope.Close(Integer::NewFromUnsigned(result));
    }


    static void Initialize(Handle<Object> target) {
      HandleScope scope;

      Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
      Local<String> name = String::NewSymbol("XXHash");

      constructor = Persistent<FunctionTemplate>::New(tpl);
      constructor->InstanceTemplate()->SetInternalFieldCount(1);
      constructor->SetClassName(name);

      NODE_SET_PROTOTYPE_METHOD(constructor, "update", Update);
      NODE_SET_PROTOTYPE_METHOD(constructor, "digest", Digest);

      constructor->Set(String::NewSymbol("hash"),
                       FunctionTemplate::New(StaticHash)->GetFunction());

      target->Set(name, constructor->GetFunction());
    }
};

void Init(Handle<Object> target) {
  HandleScope scope;
  Hash::Initialize(target);
}

NODE_MODULE(hash, Init)