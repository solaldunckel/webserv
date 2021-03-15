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

class RequestConfig;

class Response {
 public:
  // Constructors & Deconstructors
  Response(RequestConfig &config, int error_code = 0);
  ~Response();

  typedef int (Response::*type)();
  static std::map<std::string, type> methods_;
  static void initMethodMap();

  int buildErrorPage(int status_code);
  bool isCGI(std::string extension);
  void build();
  void createResponse();
  bool checkAuth();
  std::string methodList();

  enum Status {
    BUILD,
    SENDING,
    COMPLETE,
  };

  int GET();
  int POST();
  int PUT();
  int DELETE();

  int getStatus();
  std::string getResponseBody();
  int send(int fd);

 private:
  RequestConfig &config_;
  int error_code_;
  int redirect_;
  StatusCode status_;
  Status status_send_;
  size_t total_sent_;
  int status_code_;
  std::string response_;
  std::string body_;
  std::map<std::string, std::string> headers_;
};

#endif
