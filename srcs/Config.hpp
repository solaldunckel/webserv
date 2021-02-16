#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <fstream>
# include <vector>
# include <map>

# include "ServerConfig.hpp"

class Config {
 public:
  // Constructors & Deconstructors
  Config();
  Config(std::string &path);
  ~Config();

  void init();
  void tokenize();
  void parse();

  void server(std::vector<std::string>::iterator &it);
  void listen(std::vector<std::string>::iterator &it);
  void server_name(std::vector<std::string>::iterator &it);
  void error_page(std::vector<std::string>::iterator &it);
  void client_max_body_size(std::vector<std::string>::iterator &it);

  std::vector<ServerConfig> &getServers();

  void print();

 private:
  std::ifstream file_;
  std::vector<std::string> tokens_;
  std::vector<ServerConfig> servers_;
};

#endif
