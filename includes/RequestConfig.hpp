#ifndef REQUESTCONFIG_HPP
# define REQUESTCONFIG_HPP

# include "Request.hpp"

class RequestConfig {
 public:
  // Constructors & Deconstructors
  RequestConfig(Request &request, std::string &host);
  ~RequestConfig();

  void setup();

  ServerConfig *getServerForRequest(std::vector<ServerConfig> &servers);
  ServerConfig *getLocationForRequest(ServerConfig *server, std::string target);

  std::string &getMethod();
  std::string &getRoot();
  std::string &getTarget();
  int &getClientMaxBodySize();
  std::vector<std::string> &getIndexes();

 private:
  Request &request_;
  std::string &host_;
  ServerConfig *server_;
  ServerConfig *location_;
  std::string target_;
};

#endif