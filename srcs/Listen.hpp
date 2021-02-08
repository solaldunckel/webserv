#ifndef LISTEN_HPP
# define LISTEN_HPP

# include <iostream>
# include <fstream>
# include <vector>

// # include "Config.hpp"

// class Config;

class Listen {
 public:
  // Constructors & Deconstructors
  Listen();
  ~Listen();

  void setup(std::string &str);
  bool valid();

 private:
  std::string ip_;
  int port_;
};

#endif