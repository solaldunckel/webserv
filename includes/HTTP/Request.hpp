#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <sstream>
# include <string>
# include <map>
# include <vector>
# include <algorithm>

# include "Client.hpp"
# include "Config.hpp"
// # include "RequestConfig.hpp"
# include "Response.hpp"
# include "StringUtils.hpp"

class Request {
 public:
  friend class RequestConfig;

  Request();
  ~Request();

  int parse(std::string &buffer);
  int method_line();
  int headers();
  int prebody();
  int body();
  int chunk();

  bool timeout();
  time_t get_timer_in_sec();

  int getStatus();

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

  void print();

 private:
  std::string buffer_;

  std::string method_;
  std::string target_;
  std::string protocol_;
  std::string req_body_;
  std::map<std::string, std::string, ft::comp> headers_;

  int body_offset_;
  int chunk_size_;
  struct timeval start_timer_;
  size_t length_;
  Status status_;
  ChunkStatus chunk_status_;
};

#endif
