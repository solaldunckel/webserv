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
  Request();
  Request(std::string &msg);
  ~Request();

  friend class Response;

  void isValid();
  void parse();

  void setServer(ServerConfig *server);
  void setLocation(LocationConfig *location);
  void removeUriFromTarget();

  ServerConfig *getServerForRequest(std::vector<ServerConfig> &servers);
  LocationConfig *getLocationForRequest(std::string target);

  std::string &getTarget();
  std::string &getRoot();

  void print();

 private:
  std::string msg_;
  std::string method_;
  std::string target_;
  std::string protocol_;
  std::string req_body_;
  ServerConfig *server_;
  LocationConfig *location_;
  std::map<std::string, std::string, comp> headers_;
};

#endif
