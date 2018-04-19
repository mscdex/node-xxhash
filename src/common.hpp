#ifndef COMMON_H_
#define COMMON_H_

#include <node.h>
#include <node_buffer.h>
#include <nan.h>

#define XXH_STATIC_LINKING_ONLY
#define XXH_PRIVATE_API
#define XXH_ACCEPT_NULL_INPUT_POINTER 1
#include "xxhash.h"

#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || \
    defined(__DragonFly__)
# include <sys/endian.h>
#elif defined(__APPLE__) || defined(_WIN32)
# define __BIG_ENDIAN 0x1000
# define __LITTLE_ENDIAN 0x0001
# define __BYTE_ORDER __LITTLE_ENDIAN
#elif defined(sun) || defined(__sun) || defined(_AIX)
# ifdef defined(sun) || defined(__sun)
#  include <sys/isa_defs.h>
# endif
# define __BIG_ENDIAN 0x1000
# define __LITTLE_ENDIAN 0x0001
# ifdef _BIG_ENDIAN
#  define __BYTE_ORDER __BIG_ENDIAN
# else
#  define __BYTE_ORDER __LITTLE_ENDIAN
# endif
#else
# include <endian.h>
#endif

char ToLower(char c) {
  return c >= 'A' && c <= 'Z' ? c + ('a' - 'A') : c;
}

bool StringEqualNoCase(const char* a, const char* b) {
  do {
    if (*a == '\0')
      return *b == '\0';
    if (*b == '\0')
      return *a == '\0';
  } while (ToLower(*a++) == ToLower(*b++));
  return false;
}

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
    case 'l':
      // latin1
      if (encoding[1] == 'a') {
        if (strncmp(encoding + 2, "tin1", 4) == 0)
          return node::BINARY;
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

  if (StringEqualNoCase(encoding, "utf8")) {
    return node::UTF8;
  } else if (StringEqualNoCase(encoding, "utf-8")) {
    return node::UTF8;
  } else if (StringEqualNoCase(encoding, "ascii")) {
    return node::ASCII;
  } else if (StringEqualNoCase(encoding, "base64")) {
    return node::BASE64;
  } else if (StringEqualNoCase(encoding, "ucs2")) {
    return node::UCS2;
  } else if (StringEqualNoCase(encoding, "ucs-2")) {
    return node::UCS2;
  } else if (StringEqualNoCase(encoding, "utf16le")) {
    return node::UCS2;
  } else if (StringEqualNoCase(encoding, "utf-16le")) {
    return node::UCS2;
  } else if (StringEqualNoCase(encoding, "latin1")) {
    return node::BINARY;
  } else if (StringEqualNoCase(encoding, "binary")) {
    return node::BINARY;
  } else if (StringEqualNoCase(encoding, "buffer")) {
    return node::BUFFER;
  } else if (StringEqualNoCase(encoding, "hex")) {
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
