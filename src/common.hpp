#ifndef COMMON_H_
#define COMMON_H_

#include <node.h>
#include <node_buffer.h>
#include <nan.h>

#ifdef _MSC_VER
# define strcasecmp _stricmp
#else
# include <strings.h>
#endif

#include "xxhash.h"

// As of this writing, node's ParseEncoding() is not exported, so it has been
// copied here instead
enum node::encoding ParseEncoding(const char* encoding,
                                  enum node::encoding default_encoding) {
  switch (encoding[0]) {
    case 'u':
      // utf8, utf16le
      if (encoding[1] == 't' && encoding[2] == 'f') {
        // Skip `-`
        encoding += encoding[3] == '-' ? 4 : 3;
        if (encoding[0] == '8' && encoding[1] == '\0')
          return node::UTF8;
        if (strncmp(encoding, "16le", 4) == 0)
          return node::UCS2;

      // ucs2
      } else if (encoding[1] == 'c' && encoding[2] == 's') {
        encoding += encoding[3] == '-' ? 4 : 3;
        if (encoding[0] == '2' && encoding[1] == '\0')
          return node::UCS2;
      }
      break;
    case 'b':
      // binary
      if (encoding[1] == 'i') {
        if (strncmp(encoding + 2, "nary", 4) == 0)
          return node::BINARY;

      // buffer
      } else if (encoding[1] == 'u') {
        if (strncmp(encoding + 2, "ffer", 4) == 0)
          return node::BUFFER;
      }
      break;
    case '\0':
      return default_encoding;
    default:
      break;
  }

  if (strcasecmp(encoding, "utf8") == 0) {
    return node::UTF8;
  } else if (strcasecmp(encoding, "utf-8") == 0) {
    return node::UTF8;
  } else if (strcasecmp(encoding, "ascii") == 0) {
    return node::ASCII;
  } else if (strcasecmp(encoding, "base64") == 0) {
    return node::BASE64;
  } else if (strcasecmp(encoding, "ucs2") == 0) {
    return node::UCS2;
  } else if (strcasecmp(encoding, "ucs-2") == 0) {
    return node::UCS2;
  } else if (strcasecmp(encoding, "utf16le") == 0) {
    return node::UCS2;
  } else if (strcasecmp(encoding, "utf-16le") == 0) {
    return node::UCS2;
  } else if (strcasecmp(encoding, "binary") == 0) {
    return node::BINARY;
  } else if (strcasecmp(encoding, "buffer") == 0) {
    return node::BUFFER;
  } else if (strcasecmp(encoding, "hex") == 0) {
    return node::HEX;
  } else {
    return default_encoding;
  }
}

node::encoding parse_encoding(v8::Local<v8::Value> enc,
                              node::encoding def_enc = node::BUFFER) {
  if (!enc->IsString())
    return def_enc;

  Nan::Utf8String encoding(enc);
  return ParseEncoding(*encoding, def_enc);
}

#endif
