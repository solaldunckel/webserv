#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <iostream>
# include <string>
# include <vector>
# include <map>

struct Listen {
  std::string ip_;
  uint32_t port_;

  Listen() : ip_(""), port_(0) {};
  Listen(std::string ip, uint32_t port) : ip_(ip), port_(port) {};
};

inline bool operator==(const Listen &lhs, const Listen &rhs) {
  return lhs.ip_ == rhs.ip_ && lhs.port_ == rhs.port_;
}

class ServerConfig {
 public:
  friend class RequestConfig;

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
  void limit_except(std::vector<std::string>::iterator &it);
  void auth(std::vector<std::string>::iterator &it);

  void client_max_body_size(std::vector<std::string>::iterator &it);
  void error_page(std::vector<std::string>::iterator &it);
  void root(std::vector<std::string>::iterator &it);
  void index(std::vector<std::string>::iterator &it);
  void cgi(std::vector<std::string>::iterator &it);
  void autoindex(std::vector<std::string>::iterator &it);
  void upload(std::vector<std::string>::iterator &it);

  std::vector<Listen> &getListens();
  std::vector<std::string> &getServerNames();
  std::vector<ServerConfig> &getLocations();

  void print();
  void printLocation();

 private:
  std::vector<Listen> listens_;
  std::vector<std::string> server_name_;
  std::vector<ServerConfig> locations_;
  std::string uri_;
  std::string credentials_;
  bool autoindex_;
  size_t client_max_body_size_;
  std::string root_;
  std::string upload_;
  std::vector<std::string> methods_;
  std::map<int, std::string> error_codes_;
  std::vector<std::string> index_;
  std::map<std::string, std::string> cgi_;
};

#endif
