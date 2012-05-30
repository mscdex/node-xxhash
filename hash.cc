#include <node.h>
#include <node_buffer.h>

#include "xxhash.h"

using namespace node;
using namespace v8;

Handle<Value> Fast(const Arguments& args) {
  HandleScope scope;

  if (args.Length() != 2) {
    ThrowException(Exception::Error(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

  if (!Buffer::HasInstance(args[0]) || !args[1]->IsUint32()) {
    ThrowException(Exception::TypeError(String::New("Wrong arguments")));
    return scope.Close(Undefined());
  }

  Local<Object> buf = args[0]->ToObject();
  uint32_t seed = args[1]->Uint32Value();

  Local<Integer> result = Integer::NewFromUnsigned(XXH_fast32(Buffer::Data(buf),
                                                            Buffer::Length(buf),
                                                            seed)
                                                );
  return scope.Close(result->ToUint32());
}

Handle<Value> Strong(const Arguments& args) {
  HandleScope scope;

  if (args.Length() != 2) {
    ThrowException(Exception::Error(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

  if (!Buffer::HasInstance(args[0]) || !args[1]->IsUint32()) {
    ThrowException(Exception::TypeError(String::New("Wrong arguments")));
    return scope.Close(Undefined());
  }

  Local<Object> buf = args[0]->ToObject();
  uint32_t seed = args[1]->Uint32Value();

  Local<Integer> result = Integer::NewFromUnsigned(XXH_strong32(Buffer::Data(buf),
                                                            Buffer::Length(buf),
                                                            seed)
                                                );
  return scope.Close(result->ToUint32());
}

void Init(Handle<Object> target) {
  HandleScope scope;
  target->Set(String::NewSymbol("fast"),
      FunctionTemplate::New(Fast)->GetFunction());
  target->Set(String::NewSymbol("strong"),
      FunctionTemplate::New(Strong)->GetFunction());
}

NODE_MODULE(hash, Init)