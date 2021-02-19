#include "Utils.hpp"

namespace ft {
  std::string to_lower(std::string s) {
      std::transform(s.begin(), s.end(), s.begin(), ::tolower);
      return s;
  }

  void	*memset(void *b, int c, size_t len) {
    char *s = (char*)b;

    for (size_t i = 0; i < len; i++)
      s[i] = c;
    return (b);
  }
};
