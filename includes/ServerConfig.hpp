#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

// # include "IConfig.hpp"
// # include "LocationConfig.hpp"
# include <iostream>
# include <string>
# include <vector>
# include <map>

struct Listen {
  std::string ip_;
  uint32_t port_;

  Listen(std::string ip, uint32_t port) : ip_(ip), port_(port) {};
};

class ServerConfig {
 public:
  ServerConfig();
  ~ServerConfig();

  ServerConfig	&operator=(const ServerConfig &copy);

  typedef void (ServerConfig::*type)(std::vector<std::string>::iterator &);
  static std::map<std::string, type> directive_;
  static void initDirectiveMap();

  void server(std::vector<std::string>::iterator &it);
  void location(std::vector<std::string>::iterator &it);
  void listen(std::vector<std::string>::iterator &it);
  void server_name(std::vector<std::string>::iterator &it);

  void client_max_body_size(std::vector<std::string>::iterator &it);
  void error_page(std::vector<std::string>::iterator &it);
  void root(std::vector<std::string>::iterator &it);
  void index(std::vector<std::string>::iterator &it);
  void cgi(std::vector<std::string>::iterator &it);

  std::vector<Listen> &getListens();
  std::vector<std::string> &getServerNames();
  std::vector<ServerConfig> &getLocations();
  int &getClientMaxBodySize();
  std::string &getRoot();
  std::map<int, std::string> &getErrorCodes();
  std::vector<std::string> &getIndexes();
  std::string &getUri();

  void print();
  void printLocation();

 private:
  std::vector<Listen> listens_;
  std::vector<std::string> server_name_;
  std::vector<ServerConfig> locations_;
  std::string uri_;
  int client_max_body_size_;
  std::string root_;
  std::map<int, std::string> error_codes_;
  std::vector<std::string> index_;
  std::map<std::string, std::string> cgi_;
};

#endif
