#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>

# include <fcntl.h>

# include "Request.hpp"
# include "Config.hpp"
# include "StatusCode.hpp"
# include "MimeTypes.hpp"
# include "RequestConfig.hpp"
# include "File.hpp"
# include "CGI.hpp"

class Response {
 public:
  // Constructors & Deconstructors
  Response(RequestConfig &config);
  ~Response();

  typedef int (Response::*type)();
  static std::map<std::string, type> methods_;
  static void initMethodMap();

  void buildErrorPage(int status_code);
  bool isCGI(std::string extension);
  void build();
  bool checkAuth();

  int GET();
  int HEAD();
  int PUT();
  int DELETE();

  std::string getResponseBody();
  int send(int fd);

 private:
  RequestConfig &config_;
  StatusCode status_;
  std::string response_;
  std::string body_;
  std::map<std::string, std::string> headers_;
};

#endif
