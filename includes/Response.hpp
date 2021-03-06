#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>

# include <fcntl.h>

# include "Request.hpp"
# include "Config.hpp"
# include "MimeTypes.hpp"
# include "RequestConfig.hpp"
# include "File.hpp"

class Response {
 public:
  // Constructors & Deconstructors
  Response(RequestConfig &config);
  ~Response();

  typedef int (Response::*type)();
  static std::map<std::string, type> methods_;
  static void initMethodMap();
  static void initErrorCodes();

  void buildErrorPage(int status_code);
  bool isCGI(std::string &path, std::string extension);
  void build();
  bool checkAuth();

  int handleGet();
  int handlePut();
  int handleDelete();

  std::string getResponseBody();
  int send(int fd);

 private:
  RequestConfig &config_;
  int status_code_;
  std::string response_;
  std::string body_;
  std::map<std::string, std::string> headers_;
  static std::map<int, std::string> error_codes_;
};

#endif
