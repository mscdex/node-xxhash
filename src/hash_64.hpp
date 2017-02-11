#ifndef HASH64_H_
#define HASH64_H_

#include "common.hpp"

using namespace v8;

Nan::Persistent<FunctionTemplate> constructor_64;

class Hash64 : public node::ObjectWrap {
  public:
    XXH64_state_t state;

    Hash64(uint64_t seed) {
      XXH64_reset(&state, seed);
    }

    static Local<Value> convert_result(uint64_t result, node::encoding enc) {
      // Use node::Encode() directly instead of Nan::Encode() because of missing
      // optimizations in Nan::Encode() for node v0.11+
      char result_char[8];
      convert_result_val(result, result_char);
      return node::Encode(
#if NODE_MAJOR_VERSION > 0 || NODE_MINOR_VERSION > 10
                          Isolate::GetCurrent(),
#endif
                          result_char,
                          sizeof(uint64_t),
                          enc);
    }

    static Local<Value> convert_result(uint64_t result, Local<Value> enc_val) {
      Local<Value> result_val;

      if (node::Buffer::HasInstance(enc_val)) {
        result_val = enc_val;
        if (node::Buffer::Length(result_val) >= sizeof(uint64_t)) {
          char* out_buf = node::Buffer::Data(result_val);
          convert_result_val(result, out_buf);
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

    static void convert_result_val(uint64_t val, char buf[4]) {
      for (int ii = 0; ii < 8; ++ii) {
        buf[7 - ii] = val & 0xff;
        val >>= 8;
      }
    }

    static uint64_t convert_seed(Local<Value> seed_val, bool &did_throw) {
      if (seed_val->IsNumber())
        return seed_val->IntegerValue();
      else if (node::Buffer::HasInstance(seed_val)) {
        unsigned char* seed_buf = (unsigned char*)node::Buffer::Data(seed_val);
        size_t seed_buf_len = node::Buffer::Length(seed_val);
        uint8_t nb = (seed_buf_len > 8 ? 8 : seed_buf_len);
        if (nb == 0) {
          did_throw = true;
          Nan::ThrowTypeError("seed Buffer must not be empty");
          return 0;
        }
        uint64_t seed = 0;
        for (uint8_t i = 0; i < nb; ++i) {
          seed <<= 8;
          seed |= seed_buf[i];
        }
        return seed;
      }
      did_throw = true;
      Nan::ThrowTypeError("invalid seed argument");
      return 0;
    }

    ~Hash64() {
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

      uint64_t result = XXH64_digest(&obj->state);

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

      uint64_t result = XXH64(node::Buffer::Data(data),
                              node::Buffer::Length(data),
                              seed);

      if (info.Length() > 2)
        info.GetReturnValue().Set(convert_result(result, info[2]));
      else
        info.GetReturnValue().Set(convert_result(result, node::BUFFER));
    }


    static void Initialize(Handle<Object> target) {
      Local<String> name = Nan::New<String>("XXHash64").ToLocalChecked();
      Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);

      tpl->InstanceTemplate()->SetInternalFieldCount(1);
      tpl->SetClassName(name);

      Nan::SetPrototypeMethod(tpl, "update", Update);
      Nan::SetPrototypeMethod(tpl, "digest", Digest);

      Nan::SetMethod(tpl, "hash", StaticHash);
      target->Set(name, tpl->GetFunction());

      constructor_64.Reset(tpl);
    }
};

#endif
