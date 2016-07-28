#ifndef HASH32_H_
#define HASH32_H_

#include "common.hpp"

using namespace v8;

Nan::Persistent<FunctionTemplate> constructor_32;

class Hash32 : public node::ObjectWrap {
  public:
    XXH32_state_t state;

    Hash32(uint32_t seed) {
      XXH32_reset(&state, seed);
    }

    static Local<Value> convert_result(uint32_t result, node::encoding enc) {
      // Use node::Encode() directly instead of Nan::Encode() because of missing
      // optimizations in Nan::Encode() for node v0.11+
      return node::Encode(
#if NODE_MAJOR_VERSION > 0 || NODE_MINOR_VERSION > 10
                          Isolate::GetCurrent(),
#endif
                          reinterpret_cast<const char*>(&result),
                          sizeof(uint32_t),
                          enc);
    }

    static Local<Value> convert_result(uint32_t result, Local<Value> enc_val) {
      Local<Value> result_val;

      if (node::Buffer::HasInstance(enc_val)) {
        result_val = enc_val;
        if (node::Buffer::Length(result_val) >= sizeof(uint32_t)) {
          char* out_buf = node::Buffer::Data(result_val);
          *(reinterpret_cast<uint32_t*>(&out_buf[0])) = result;
        } else
          Nan::ThrowError("Buffer argument too small");
      } else if (enc_val->IsString()) {
        node::encoding enc = parse_encoding(enc_val);
        if (enc == node::BASE64 ||
            enc == node::HEX ||
            enc == node::BINARY ||
            enc == node::BUFFER) {
          result_val = convert_result(result, enc);
        } else
          Nan::ThrowError("invalid encoding");
      } else
        Nan::ThrowTypeError("argument must be a Buffer or string");

      return result_val;
    }

    static uint32_t convert_seed(Local<Value> seed_val) {
      if (seed_val->IsUint32())
        return seed_val->Uint32Value();
      else if (node::Buffer::HasInstance(seed_val)) {
        char* seed_buf = node::Buffer::Data(seed_val);
        size_t seed_buf_len = node::Buffer::Length(seed_val);
        uint8_t nb = (seed_buf_len > 4 ? 4 : seed_buf_len);
        if (nb == 0) {
          seed_val.Clear();
          Nan::ThrowTypeError("seed Buffer must not be empty");
          return 0;
        }
        uint32_t seed = 0;
        for (uint8_t i = 0; i < nb; ++i) {
          seed <<= 8;
          seed += seed_buf[i];
        }
        return seed;
      }
      seed_val.Clear();
      Nan::ThrowTypeError("invalid seed argument");
      return 0;
    }

    ~Hash32() {
    }

    static NAN_METHOD(New) {
      if (!info.IsConstructCall())
        return Nan::ThrowError("Use `new` to create instances of this object.");
      else if (info.Length() == 0)
        return Nan::ThrowTypeError("Missing seed argument");

      uint32_t seed = convert_seed(info[0]);
      if (seed == 0 && info[0].IsEmpty())
        return info.GetReturnValue().SetUndefined();

      Hash32* obj = new Hash32(seed);
      obj->Wrap(info.This());

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(Update) {
      Hash32* obj = ObjectWrap::Unwrap<Hash32>(info.This());

      if (!node::Buffer::HasInstance(info[0]))
        return Nan::ThrowTypeError("data argument must be a Buffer");

      Local<Value> data = info[0];

      XXH32_update(&obj->state,
                   node::Buffer::Data(data),
                   node::Buffer::Length(data));

      info.GetReturnValue().SetUndefined();
    }

    static NAN_METHOD(Digest) {
      Hash32* obj = ObjectWrap::Unwrap<Hash32>(info.This());

      uint32_t result = XXH32_digest(&obj->state);

      if (info.Length() > 0)
        info.GetReturnValue().Set(convert_result(result, info[0]));
      else
        info.GetReturnValue().Set(Nan::New<Integer>(result));
    }

    static NAN_METHOD(StaticHash) {
      if (info.Length() < 2)
        return Nan::ThrowTypeError("Expected data and seed arguments");
      else if (!node::Buffer::HasInstance(info[0]))
        return Nan::ThrowTypeError("data argument must be a Buffer");

      uint32_t seed = convert_seed(info[1]);
      if (seed == 0 && info[1].IsEmpty())
        return info.GetReturnValue().SetUndefined();

      Local<Value> data = info[0];

      uint32_t result = XXH32(node::Buffer::Data(data),
                              node::Buffer::Length(data),
                              seed);

      if (info.Length() > 2)
        info.GetReturnValue().Set(convert_result(result, info[2]));
      else
        info.GetReturnValue().Set(Nan::New<Integer>(result));
    }


    static void Initialize(Handle<Object> target) {
      Local<String> name = Nan::New<String>("XXHash32").ToLocalChecked();
      Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);

      tpl->InstanceTemplate()->SetInternalFieldCount(1);
      tpl->SetClassName(name);

      Nan::SetPrototypeMethod(tpl, "update", Update);
      Nan::SetPrototypeMethod(tpl, "digest", Digest);
      Nan::SetMethod(tpl, "hash", StaticHash);
      target->Set(name, tpl->GetFunction());

      constructor_32.Reset(tpl);
    }
};

#endif
