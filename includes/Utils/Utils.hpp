#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <exception>
# include <string>
# include <sstream>

# include <netinet/in.h>
# include <stdlib.h>
# include <sys/time.h>
# include <time.h>

bool isValidMethod(std::string const &str);
bool isValidDirective(std::string const &str);

namespace ft {
  void *memset(void *b, int c, size_t len);
  std::string inet_ntop(const void *s);
  void *get_in_addr(struct sockaddr *sa);
  void free_tab(char **tab);
  std::string get_http_date();
  time_t get_current_time_in_sec();

  template<typename T>
  void delete_(T *&ptr) {
    delete ptr;
    ptr = nullptr;
  }
};

#endif
