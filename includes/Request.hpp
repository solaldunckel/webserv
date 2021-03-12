#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <sstream>
# include <string>
# include <map>
# include <vector>
# include <algorithm>

# include "Config.hpp"
# include "RequestConfig.hpp"
# include "Response.hpp"
# include "Utils.hpp"

class Response;

class Request {
 public:
  friend class RequestConfig;

  // Constructors & Deconstructors
  Request();
  ~Request();

  int parse(std::string &buffer);
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

  void config(std::string &host, std::vector<ServerConfig> &servers);
  int send(int fd);

  void print();

 private:
  std::string buffer_;

  std::string method_;
  std::string target_;
  std::string protocol_;
  std::string req_body_;
  std::map<std::string, std::string, ft::comp> headers_;

  RequestConfig *config_;
  Response *response_;

  int body_offset_;
  int chunk_size_;
  size_t length_;
  Status status_;
  ChunkStatus chunk_status_;
};

#endif
