#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include "IConfig.hpp"
# include "LocationConfig.hpp"

class ServerConfig : public IConfig {
 public:
  // Constructors & Deconstructors
  ServerConfig();
  ~ServerConfig();

  typedef void (ServerConfig::*type)(std::vector<std::string>::iterator &);
  static std::map<std::string, type> directive;
  static void initDirectiveMap();

  void server(std::vector<std::string>::iterator &it);
  void server_name(std::vector<std::string>::iterator &it);
  void listen(std::vector<std::string>::iterator &it);

  void print();

 private:
  std::map<std::string, uint32_t> host_ips_;
  std::vector<std::string> server_name_;
  std::vector<LocationConfig> locations_;
};

#endif
