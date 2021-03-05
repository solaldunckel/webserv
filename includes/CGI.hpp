#ifndef CGI_HPP
# define CGI_HPP

# include <iostream>
# include <map>

# include "Utils.hpp"

class CGI {
 public:
  CGI();
  ~CGI();

  int execute();
  void setCGIEnv();

 private:
  std::map<std::string, std::string> cgi_env_;
};

#endif