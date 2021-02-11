#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <sstream>
# include <string>
# include <map>

class Request {
 public:
  // Constructors & Deconstructors
  Request();
  Request(std::string &msg);
  ~Request();

  void print();
  void parse();
 private:
  std::string msg_;
  std::string method_;
  std::string target_;
  std::map<std::string, std::string> headers_;
};

#endif
