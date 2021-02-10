#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <fstream>
# include <vector>
# include <map>

# include "Listen.hpp"
# include "Location.hpp"

class Config {
 public:
  // Constructors & Deconstructors
  Config();
  ~Config();

  typedef void (Config::*type)(std::vector<std::string>::iterator &);
  static std::map<std::string, type> dir;
  static void initMap();

  void server(std::vector<std::string>::iterator &it);
  void listen(std::vector<std::string>::iterator &it);
  void server_name(std::vector<std::string>::iterator &it);
  void error_page(std::vector<std::string>::iterator &it);
  void client_max_body_size(std::vector<std::string>::iterator &it);

  void print();

 private:
  Listen listen_;
  std::vector<std::string> server_name_;
  std::map<int, std::string> error_codes_;
  std::vector<Location> locations_;
  int client_max_body_size_;
};

#endif
