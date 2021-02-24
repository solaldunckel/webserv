#ifndef UTILS_HPP
# define UTILS_HPP

# include <netinet/in.h>

# include <iostream>
# include <string>
# include <sstream>

namespace ft {
  std::string to_lower(std::string s);
  void *memset(void *b, int c, size_t len);
  std::string inet_ntop(const void *s);
  void *get_in_addr(struct sockaddr *sa);
};

#endif
