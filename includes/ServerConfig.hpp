#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include "IConfig.hpp"
# include "LocationConfig.hpp"

struct Listen {
  std::string ip_;
  uint32_t port_;

  Listen(std::string ip, uint32_t port) : ip_(ip), port_(port) {};
};

class ServerConfig : public IConfig {
 public
  ServerConfig();
  ~ServerConfig();

  typedef void (ServerConfig::*type)(std::vector<std::string>::iterator &);
  static std::map<std::string, type> directive;
  static void initDirectiveMap();

  void server(std::vector<std::string>::iterator &it);
  void server_name(std::vector<std::string>::iterator &it);
  void listen(std::vector<std::string>::iterator &it);

  std::vector<Listen> &getListens();
  std::vector<std::string> &getServerNames();
  std::vector<LocationConfig> &getLocations();

  void print();

 private:
  std::vector<Listen> listens_;
  std::vector<std::string> server_name_;
  std::vector<LocationConfig> locations_;
};

#endif
