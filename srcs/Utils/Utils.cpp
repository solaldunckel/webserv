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

  void free_tab(char **tab) {
    int i = 0;

    while (tab[i]) {
      free(tab[i]);
      tab[i] = nullptr;
      i++;
    }
    free(tab);
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
