#ifndef STRINGUTILS_HPP
# define STRINGUTILS_HPP

# include <string>
# include <sstream>
# include <algorithm>
# include <iostream>
# include <stdlib.h>
# include <cstring>
# include <limits>

namespace ft {
  std::string to_lower(std::string s);
  std::string to_upper(std::string s);
  std::string trim_left(std::string str, char c);
  std::string trim_right(std::string str, char c);
  size_t to_hex(std::string &str);
  char *strdup(const char *str);
  std::string unique_char(std::string str);
  int stoi(const std::string &str, std::size_t *pos = 0, int base = 10);

  template<typename T>
  std::string to_string(T number) {
    std::stringstream ss;

    ss << number;
    return ss.str();
  }

  struct comp {
    bool operator() (const std::string& lhs, const std::string& rhs) const {
      return to_lower(lhs) < to_lower(rhs);
    };
  };
}

#endif