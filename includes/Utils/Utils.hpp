#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <exception>
# include <string>
# include <cstring>
# include <sstream>
# include <ctime>

# include <netinet/in.h>
# include <stdlib.h>
# include <sys/time.h>

# include "StringUtils.hpp"

bool isValidMethod(std::string const &str);
bool isValidDirective(std::string const &str);

class webserv_exception : virtual public std::exception {
 public:
  webserv_exception(std::string msg, int err_num = 0, std::string arg = "") : errno_(err_num) {
    error_msg_ = msg;
    if (error_msg_.find('%') != std::string::npos)
      error_msg_.replace(error_msg_.find('%'), 1, arg);
    if (errno_) {
      error_msg_ = error_msg_ + " (" + ft::to_string(errno_) + ": " + strerror(errno_) + ")";
    }
  }
  virtual ~webserv_exception() throw () {}

  const char* what() const throw () {
    return error_msg_.c_str();
  }

 private:
  int errno_;
  std::string error_msg_;
};

namespace ft {
  std::string inet_ntop(const void *s);
  void *get_in_addr(struct sockaddr *sa);
  void free_tab(char **tab);
  std::string get_http_date();
  time_t get_current_time_in_sec();

  template<typename T>
  void delete_(T *&ptr) {
    if (ptr) {
      delete ptr;
      ptr = NULL;
    }
  }
};

#endif
