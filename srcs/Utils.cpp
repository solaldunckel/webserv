#include "Utils.hpp"

bool isValidMethod(std::string const &str) {
  return (str == "GET" ||
          str == "POST" ||
          str == "HEAD" ||
          str == "PUT" ||
          str == "DELETE");
}

bool isValidDirective(std::string const &str) {
  return (str == "listen" ||
          str == "server_name" ||
          str == "root" ||
          str == "auth" ||
          str == "error_page" ||
          str == "upload" ||
          str == "autoindex" ||
          str == "index" ||
          str == "cgi");
}

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

  size_t strlen(const char *str) {
    int i = 0;
    while (str[i])
      i++;
    return i;
  }

  char *strdup(const char *str) {
    if (!str)
      return nullptr;
    char *newstr = (char*)malloc(sizeof(char) * ft::strlen(str) + 1);

    if (!newstr)
      return nullptr;

    size_t i = 0;

    while (str[i]) {
      newstr[i] = str[i];
      i++;
    }

    newstr[i] = '\0';
    return newstr;
  }

  void free_tab(char **tab) {
    int i = 0;

    while (tab[i]) {
      free(tab[i]);
      tab[i] = nullptr;
      i++;
    }
    free(tab);
  }

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
      int n = B64index[p[i]] << 18 | B64index[p[i + 1]] << 12 | B64index[p[i + 2]] << 6 | B64index[p[i + 3]];
      str[j++] = n >> 16;
      str[j++] = n >> 8 & 0xFF;
      str[j++] = n & 0xFF;
    }
    if (pad1) {
      int n = B64index[p[last]] << 18 | B64index[p[last + 1]] << 12;
      str[j++] = n >> 16;
      if (pad2) {
        n |= B64index[p[last + 2]] << 6;
        str[j++] = n >> 8 & 0xFF;
      }
    }
    return result;
  }

  std::string b64decode(const std::string &str64) {
    return b64decode(str64.c_str(), str64.size());
  }

  std::string get_http_date() {
    struct timeval tv;
    char buf[32];
    gettimeofday(&tv, NULL);

    struct tm	*tm;
    tm = gmtime(&tv.tv_sec);
    int ret = strftime(buf, 32, "%a, %d %b %Y %T GMT", tm);
    return std::string(buf, ret);
  }

  time_t get_current_time_in_sec() {
    struct timeval now;

    gettimeofday(&now, NULL);
    return (now.tv_sec);
  }
};
