#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <string>
# include <fstream>
# include <vector>
# include <stack>

# include "ServerConfig.hpp"
# include "StringUtils.hpp"
# include "Utils.hpp"

class Config {
 public:
  Config(std::string &path);
  ~Config();

  void tokenize();
  void parse();

  std::vector<ServerConfig> &getServers();

  void print();

 private:
  std::ifstream file_;
  std::vector<std::string> tokens_;
  std::vector<ServerConfig> servers_;
};

#endif
