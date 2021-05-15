#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>

# include "RequestConfig.hpp"
# include "StatusCode.hpp"
# include "MimeTypes.hpp"
# include "File.hpp"
# include "CGI.hpp"
# include "Base64.hpp"

class RequestConfig;

class Response {
 public:
  Response(RequestConfig &config, int worker_id, int error_code = 0);
  ~Response();

  typedef int (Response::*type)();
  static std::map<std::string, type> methods_;
  static void initMethodMap();

  int buildErrorPage(int status_code);
  bool isCGI(std::string extension);
  void build();
  int handleMethods();
  void createResponse();
  bool checkAuth();
  bool localization(std::vector<std::string> &matches);
  std::string accept_charset(std::vector<std::string> &matches);
  std::string methodList();
  void print();

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
  File file_;
  int error_code_;
  int worker_id_;
  int redirect_;
  StatusCode status_;
  Status status_send_;
  size_t total_sent_;
  int status_code_;
  std::string response_;
  std::string body_;
  std::string charset_;
  std::map<std::string, std::string> headers_;
};

#endif
