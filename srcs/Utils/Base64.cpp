#include "Base64.hpp"

namespace ft {
  const std::string b64decode(const void *data, const size_t &len) {
    if (len == 0) return "";

    unsigned char *p = (unsigned char*)data;
    size_t j = 0,
      pad1 = len % 4 || p[len - 1] == '=',
      pad2 = pad1 && (len % 4 > 2 || p[len - 2] != '=');
    const size_t last = (len - pad1) / 4 << 2;
    std::string result(last / 4 * 3 + pad1 + pad2, '\0');
    unsigned char *str = (unsigned char*) &result[0];

    for (size_t i = 0; i < last; i += 4) {
      int n = ft::B64index[p[i]] << 18 | ft::B64index[p[i + 1]] << 12 | ft::B64index[p[i + 2]] << 6 | ft::B64index[p[i + 3]];
      str[j++] = n >> 16;
      str[j++] = n >> 8 & 0xFF;
      str[j++] = n & 0xFF;
    }
    if (pad1) {
      int n = ft::B64index[p[last]] << 18 | ft::B64index[p[last + 1]] << 12;
      str[j++] = n >> 16;
      if (pad2) {
        n |= ft::B64index[p[last + 2]] << 6;
        str[j++] = n >> 8 & 0xFF;
      }
    }
    return result;
  }

  std::string b64decode(const std::string &str64) {
    return b64decode(str64.c_str(), str64.size());
  }
}