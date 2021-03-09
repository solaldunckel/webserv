#ifndef REQUESTCONFIG_HPP
# define REQUESTCONFIG_HPP

# include "Request.hpp"

class RequestConfig {
 public:
  // Constructors & Deconstructors
  RequestConfig(Request &request, std::string &host, std::vector<ServerConfig> &servers);
  ~RequestConfig();

  void setup();

  ServerConfig *getServerForRequest(std::vector<ServerConfig> &servers);
  ServerConfig *getLocationForRequest(ServerConfig *server, std::string target);

  std::string &getMethod();
  std::string &getRoot();
  std::string &getUri();
  std::string &getTarget();
  std::string &getBody();
  std::string &getAuth();
  std::map<std::string, std::string> &getCGI();
  std::map<std::string, std::string, comp> &getHeaders();
  std::string &getHeader(std::string key);
  size_t &getClientMaxBodySize();
  std::string &getProtocol();
  std::vector<std::string> &getIndexes();
  std::vector<std::string> &getMethods();
  std::string &getHost();
  uint32_t &getPort();

  bool methodAccepted(std::string &method);

 private:
  Request &request_;
  std::string &host_;
  std::vector<ServerConfig> &servers_;
  ServerConfig *server_;
  ServerConfig *location_;
  std::string target_;
  uint32_t port_;
};

#endif
