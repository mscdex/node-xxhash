#ifndef HASH64_H_
#define HASH64_H_

#include "common.hpp"

using namespace v8;

class Hash64 : public Nan::ObjectWrap {
  public:
    XXH64_state_t state;

    Hash64(uint64_t seed) {
      XXH64_reset(&state, seed);
    }

    ~Hash64() {
    }

    static Local<Value> convert_result(uint64_t result, node::encoding enc) {
      // Use node::Encode() directly instead of Nan::Encode() because of missing
      // optimizations in Nan::Encode() for node v0.11+
      return node::Encode(
#if NODE_MAJOR_VERSION > 0 || NODE_MINOR_VERSION > 10
                          Isolate::GetCurrent(),
#endif
                          reinterpret_cast<const char*>(&result),
                          sizeof(uint64_t),
                          enc);
    }

    static Local<Value> convert_result(uint64_t result, Local<Value> enc_val) {
      Local<Value> result_val;

      if (node::Buffer::HasInstance(enc_val)) {
        result_val = enc_val;
        if (node::Buffer::Length(result_val) >= sizeof(uint64_t)) {
          char* out_buf = node::Buffer::Data(result_val);
          *(reinterpret_cast<uint64_t*>(&out_buf[0])) = result;
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

    static uint64_t convert_seed(Local<Value> seed_val, bool &did_throw) {
      if (seed_val->IsUint32()) {
        return Nan::To<uint32_t>(seed_val).FromJust();
      } else if (node::Buffer::HasInstance(seed_val)) {
        char* seed_buf = node::Buffer::Data(seed_val);
        size_t seed_buf_len = node::Buffer::Length(seed_val);
        if (seed_buf_len == 4) {
#if __BYTE_ORDER == __LITTLE_ENDIAN
          return XXH_readLE32(seed_buf, XXH_littleEndian);
#else
          return XXH_readLE32(seed_buf, XXH_bigEndian);
#endif
        } else if (seed_buf_len == 8) {
#if __BYTE_ORDER == __LITTLE_ENDIAN
          return XXH_readLE64(seed_buf, XXH_littleEndian);
#else
          return XXH_readLE64(seed_buf, XXH_bigEndian);
#endif
        }
        Nan::ThrowTypeError("seed Buffer must be 4 or 8 bytes");
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
      uint64_t seed = convert_seed(info[0], did_throw);
      if (did_throw)
        return;

      Hash64* obj = new Hash64(seed);
      obj->Wrap(info.This());

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(Update) {
      Hash64* obj = ObjectWrap::Unwrap<Hash64>(info.This());

      if (!node::Buffer::HasInstance(info[0]))
        return Nan::ThrowTypeError("data argument must be a Buffer");

      Local<Value> data = info[0];

      XXH64_update(&obj->state,
                   node::Buffer::Data(data),
                   node::Buffer::Length(data));

      info.GetReturnValue().SetUndefined();
    }

    static NAN_METHOD(Digest) {
      Hash64* obj = ObjectWrap::Unwrap<Hash64>(info.This());

#if __BYTE_ORDER == __LITTLE_ENDIAN
      uint64_t result = XXH64_digest(&obj->state);
#else
      uint64_t result = XXH_swap64(XXH64_digest(&obj->state));
#endif

      if (info.Length() > 0)
        info.GetReturnValue().Set(convert_result(result, info[0]));
      else
        info.GetReturnValue().Set(convert_result(result, node::BUFFER));
    }

    static NAN_METHOD(StaticHash) {
      if (info.Length() < 2)
        return Nan::ThrowTypeError("Expected data and seed arguments");
      else if (!node::Buffer::HasInstance(info[0]))
        return Nan::ThrowTypeError("data argument must be a Buffer");

      bool did_throw = false;
      uint64_t seed = convert_seed(info[1], did_throw);
      if (did_throw)
        return;

      Local<Value> data = info[0];

#if __BYTE_ORDER == __LITTLE_ENDIAN
      uint64_t result = XXH64(node::Buffer::Data(data),
                              node::Buffer::Length(data),
                              seed);
#else
      uint64_t result = XXH_swap64(XXH64(node::Buffer::Data(data),
                                         node::Buffer::Length(data),
                                         seed));
#endif

      if (info.Length() > 2)
        info.GetReturnValue().Set(convert_result(result, info[2]));
      else
        info.GetReturnValue().Set(convert_result(result, node::BUFFER));
    }

    static NAN_MODULE_INIT(Init) {
      Local<String> name = Nan::New<String>("XXHash64").ToLocalChecked();
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
