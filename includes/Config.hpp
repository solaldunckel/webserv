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
  Config(std::string &path);
  ~Config();

  void tokenize();
  void parse();
  bool is_directive(std::string str);

  std::vector<ServerConfig> &getServers();

  void print();

 private:
  std::ifstream file_;
  std::vector<std::string> tokens_;
  std::vector<ServerConfig> servers_;
};

#endif
