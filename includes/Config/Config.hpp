#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <string>
# include <vector>
# include <stack>

# include <fcntl.h>

# include "get_next_line.hpp"
# include "Server.hpp"
# include "ServerConfig.hpp"
# include "StringUtils.hpp"
# include "Utils.hpp"

class Config {
 public:
  Config(std::string &path);
  ~Config();

  void tokenize();
  void parse();
  void clear();

  std::vector<ServerConfig> &getServers();
  std::string &getPath();

  int getWorkers();

  void print();

 private:
  std::string path_;
  int fd_;

  int workers_;

  std::string file_content_;
  std::vector<std::string> tokens_;
  std::vector<ServerConfig> servers_;
};

#endif
