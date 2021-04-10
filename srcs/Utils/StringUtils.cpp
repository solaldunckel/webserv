#include "StringUtils.hpp"

namespace ft {
  std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
  }

  std::string to_upper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
  }

  std::string trim_left(std::string str, char c) {
    size_t startpos = str.find_first_not_of(c);
    if (startpos != std::string::npos)
      str.erase(0, startpos);
    return str;
  }

  size_t to_hex(std::string &str) {
    std::stringstream ss;
    size_t hex;

    ss << std::hex << str;
    ss >> hex;

    return hex;
  }

  char *strdup(const char *str) {
    if (!str)
      return NULL;
    char *newstr = (char*)malloc(sizeof(char) * (strlen(str) + 1));

    if (!newstr)
      return NULL;

    size_t i = 0;

    while (str[i]) {
      newstr[i] = str[i];
      i++;
    }

    newstr[i] = '\0';
    return newstr;
  }

  std::string unique_char(std::string str) {
    std::string::iterator it = str.begin();

    while (it != str.end()) {
      if (*it == '/') {
        it++;
        while (it != str.end() && *it == '/') {
          it = str.erase(it);
        }
      }
      else
        it++;
    }
    return str;
  }

  int stoi(const std::string &str, std::size_t *pos, int base) {
    static std::string b("0123456789abcdef");
    (void)pos;
    std::string our_base(b, 0, base);

    int i = 0;

    if (str.empty())
      throw std::invalid_argument("null or empty string argument");

    bool negate = (str[0] != '-');

    if (str[i] == '+' || str[i] == '-')
      ++i;

    long result = 0;

    while (str[i]) {
      if (our_base.find(str[i]) == std::string::npos)
        throw std::invalid_argument("invalid input string");
      result = result * base + our_base.find(str[i]);
      if (result > std::numeric_limits<int>::max())
        throw std::invalid_argument("overflow");
      i++;
    }
    return negate ? result : -result;
  }
}