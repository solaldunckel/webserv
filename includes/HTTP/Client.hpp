#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>

# include "InputArgs.hpp"
# include "Request.hpp"
# include "RequestConfig.hpp"
# include "ServerConfig.hpp"
# include "Response.hpp"

# define TIMEOUT 60

class RequestConfig;
class InputArgs;
class Response;

class Client {
 public:
  Client(int fd, std::string &addr, Listen &host_port, bool disconnect = false);
  ~Client();

  void setupConfig(std::vector<ServerConfig> &servers);
  void setupResponse(std::vector<ServerConfig> &servers, InputArgs &options, int error_code = 0);
  void clear();

  bool timeout();
  bool disconnect();

  int getFd();
  std::string &getAddr();

  Request *getRequest(bool force = false);
  RequestConfig *getConfig();
  Response *getResponse();

 private:
  int fd_;
  std::string addr_;
  Listen &host_port_;
  int disconnect_;
  Request *request_;
  RequestConfig *config_;
  Response *response_;
};

#endif
