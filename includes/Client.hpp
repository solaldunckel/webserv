#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>

# include "Request.hpp"

class Client {
 public:
  Client(std::string addr, std::string &host);
  ~Client();

  void createRequest();
  void setupResponse(std::vector<ServerConfig> &servers);
  void clear();

  Request *getRequest();
  RequestConfig *getConfig();
  Response *getResponse();

 private:
  std::string addr_;
  std::string host_;
  Request *request_;
  RequestConfig *config_;
  Response *response_;

};

#endif
