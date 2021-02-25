#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <sstream>
# include <string>
# include <map>
# include <vector>
# include <algorithm>

# include "Config.hpp"
# include "Utils.hpp"

struct comp {
  bool operator() (const std::string& lhs, const std::string& rhs) const {
    return ft::to_lower(lhs) < ft::to_lower(rhs);
  };
};

class Request {
 public:
  // Constructors & Deconstructors
  // Request();
  Request(std::string &msg, std::vector<ServerConfig> &servers);
  ~Request();

  void parse();

  void removeUriFromTarget();

  bool isValid();
  std::string &getMethod();
  std::string &getTarget();
  std::string &getProtocol();
  std::string &getBody();
  std::vector<ServerConfig> &getServers();
  std::string &getHeader(std::string key);

  void print();

 private:
  std::string msg_;
  std::string method_;
  std::string target_;
  std::string protocol_;
  std::string req_body_;
  std::string server_;
  bool valid_;
  std::vector<ServerConfig> &servers_;
  std::map<std::string, std::string, comp> headers_;
};

#endif
