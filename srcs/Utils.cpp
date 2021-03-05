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

  std::string inet_ntop(const void *s) {
    const u_char *src = (const u_char*)s;
    std::ostringstream oss;

    oss << std::to_string(src[0]) << '.' << std::to_string(src[1]) << '.' <<
      std::to_string(src[2]) << '.' << std::to_string(src[3]);
    return oss.str();
  }

  void *get_in_addr(struct sockaddr *sa) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  std::string trim_left(std::string str, char c) {
    size_t startpos = str.find_first_not_of(c);
    if (startpos != std::string::npos)
      str = str.substr(startpos);
    return str;
  }

  size_t to_hex(std::string &str) {
    std::stringstream ss;
    size_t hex;

    ss << std::hex << str;
    ss >> hex;

    return hex;
  }

  size_t strlen(char *str) {
    int i = 0;
    while (str[i])
      i++;
    return i;
  }

  char *strdup(char *str) {
    if (!str)
      return nullptr;
    char *newstr = (char*)malloc(sizeof(char) * strlen(str) + 1);

    if (!newstr)
      return nullptr;

    size_t i = 0;

    while (str[i])
      newstr[i] = str[i];

    newstr[i] = '\0';
    return newstr;
  }
};
