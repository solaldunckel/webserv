#ifndef UTILS_HPP
# define UTILS_HPP

# include <netinet/in.h>
# include <stdlib.h>

# include <iostream>
# include <string>
# include <sstream>
# include <sys/time.h>
# include <time.h>

bool isValidMethod(std::string const &str);
bool isValidDirective(std::string const &str);

namespace ft {
  static const int B64index[256] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  62, 63, 62, 62, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0,  0,  0,  0,  0,  0,
    0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0,  0,  0,  0,  63,
    0,  26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
  };

  std::string to_lower(std::string s);
  void *memset(void *b, int c, size_t len);
  std::string inet_ntop(const void *s);
  void *get_in_addr(struct sockaddr *sa);
  std::string trim_left(std::string str, char c);
  size_t to_hex(std::string &str);
  size_t strlen(const char *str);
  char *strdup(const char *str);
  const std::string b64decode(const void* data, const size_t &len);
  std::string b64decode(const std::string &str64);
  std::string get_http_date();

  struct comp {
    bool operator() (const std::string& lhs, const std::string& rhs) const {
      return to_lower(lhs) < to_lower(rhs);
    };
  };
};

#endif
