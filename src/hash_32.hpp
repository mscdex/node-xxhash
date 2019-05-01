#ifndef HASH32_H_
#define HASH32_H_

#include "common.hpp"

using namespace v8;

class Hash32 : public Nan::ObjectWrap {
  public:
    XXH32_state_t state;

    Hash32(uint32_t seed) {
      XXH32_reset(&state, seed);
    }

    ~Hash32() {
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

#if __BYTE_ORDER == __BIG_ENDIAN
      result = XXH_swap32(result);
#endif
      if (node::Buffer::HasInstance(enc_val)) {
        result_val = enc_val;
        if (node::Buffer::Length(result_val) >= sizeof(uint32_t)) {
          char* out_buf = node::Buffer::Data(result_val);
          *(reinterpret_cast<uint32_t*>(&out_buf[0])) = result;
        } else {
          Nan::ThrowError("Buffer argument too small");
        }
      } else if (enc_val->IsString()) {
        node::encoding enc = parse_encoding(enc_val);
        if (enc == node::BASE64 ||
            enc == node::HEX ||
            enc == node::BINARY ||
            enc == node::BUFFER) {
          result_val = convert_result(result, enc);
        } else {
          Nan::ThrowError("invalid encoding");
        }
      } else {
        Nan::ThrowTypeError("argument must be a Buffer or string");
      }

      return result_val;
    }

    static uint32_t convert_seed(Local<Value> seed_val, bool &did_throw) {
      if (seed_val->IsUint32()) {
        return Nan::To<uint32_t>(seed_val).FromJust();
      } else if (node::Buffer::HasInstance(seed_val)) {
        char* seed_buf = node::Buffer::Data(seed_val);
        if (node::Buffer::Length(seed_val) == 4) {
#if __BYTE_ORDER == __LITTLE_ENDIAN
          return XXH_readLE32(seed_buf, XXH_littleEndian);
#else
          return XXH_readLE32(seed_buf, XXH_bigEndian);
#endif
        }
        Nan::ThrowTypeError("seed Buffer must be 4 bytes");
      } else {
        Nan::ThrowTypeError("invalid seed argument");
      }
      did_throw = true;
      return 0;
    }

    static NAN_METHOD(New) {
      if (!info.IsConstructCall())
        return Nan::ThrowError("Use `new` to create instances of this object.");
      else if (info.Length() == 0)
        return Nan::ThrowTypeError("Missing seed argument");

      bool did_throw = false;
      uint32_t seed = convert_seed(info[0], did_throw);
      if (did_throw)
        return;

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

      bool did_throw = false;
      uint32_t seed = convert_seed(info[1], did_throw);
      if (did_throw)
        return;

      Local<Value> data = info[0];

      uint32_t result = XXH32(node::Buffer::Data(data),
                              node::Buffer::Length(data),
                              seed);

      if (info.Length() > 2)
        info.GetReturnValue().Set(convert_result(result, info[2]));
      else
        info.GetReturnValue().Set(Nan::New<Integer>(result));
    }
    
    static NAN_MODULE_INIT(Init) {
      Local<String> name = Nan::New<String>("XXHash32").ToLocalChecked();
      Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);

      tpl->InstanceTemplate()->SetInternalFieldCount(1);
      tpl->SetClassName(name);

      Nan::SetPrototypeMethod(tpl, "update", Update);
      Nan::SetPrototypeMethod(tpl, "digest", Digest);
      Nan::SetMethod(tpl, "hash", StaticHash);
      Nan::Set(target, name, Nan::GetFunction(tpl).ToLocalChecked());
    }
};

#endif
