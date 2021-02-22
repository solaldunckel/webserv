#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <sstream>
# include <string>
# include <map>
# include <vector>
# include <algorithm>

# include "Utils.hpp"

struct comp {
  bool operator() (const std::string& lhs, const std::string& rhs) const {
    return ft::to_lower(lhs) < ft::to_lower(rhs);
  };
};

class Request {
 public:
  // Constructors & Deconstructors
  Request();
  Request(std::string &msg);
  ~Request();

  friend class Response;

  void isValid();
  void parse();

  void print();

 private:
  std::string msg_;
  std::string method_;
  std::string target_;
  std::string protocol_;
  std::string req_body_;
  std::map<std::string, std::string, comp> headers_;
};

#endif
