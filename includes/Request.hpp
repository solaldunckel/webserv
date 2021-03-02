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
  Request(std::string &msg, std::vector<ServerConfig> &servers);
  ~Request();

  int parse(std::string buffer);
  int method_line();
  int headers();
  int prebody();
  int body();
  int chunk();

  enum Status {
    FIRST_LINE,
    HEADERS,
    PREBODY,
    BODY,
    CHUNK,
    COMPLETE,
    ERROR
  };

  enum ChunkStatus {
    CHUNK_BODY,
    CHUNK_SIZE,
  };

  void initHeadersMap();

  bool isValid();
  std::string &getMethod();
  std::string &getTarget();
  std::string &getProtocol();
  std::string &getBody();
  std::vector<ServerConfig> &getServers();
  std::string &getHeader(std::string key);

  void clear();
  void print();

 private:
  std::stringstream input_;
  std::string buffer_;

  std::string method_;
  std::string target_;
  std::string protocol_;
  std::string req_body_;

  int body_offset_;
  int chunk_status_;
  int chunk_size_;
  size_t length_;
  Status status_;
  bool valid_;
  // std::vector<ServerConfig> &servers_;
  std::map<std::string, std::string, comp> headers_;
};

#endif
