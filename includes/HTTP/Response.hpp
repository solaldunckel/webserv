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

  void clear();
  int buildErrorPage(int status_code);
  bool isCGI(std::string extension);
  void build();
  int handleMethods();
  void createResponse();
  bool checkAuth();
  bool localization(std::vector<std::string> &matches);
  std::string accept_charset(std::vector<std::string> &matches);
  std::string buildMethodList();
  bool shouldDisconnect();
  bool redirect();
  std::string redirect_target();
  void print();

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
  StatusCode status_;
  size_t total_sent_;
  int status_code_;
  std::string response_;
  std::string body_;
  bool redirect_;
  std::string redirect_target_;
  int redirect_code_;
  size_t header_size_;
  size_t body_size_;
  std::string charset_;
  std::map<std::string, std::string> headers_;
};

#endif
