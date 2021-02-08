#ifndef PARSER_HPP
# define PARSER_HPP

# include <iostream>
# include <fstream>
# include <vector>

# include "Config.hpp"

class Parser {
 public:
  // Constructors & Deconstructors
  Parser(std::string &path, std::vector<Config> &servers);
  ~Parser();

  void Tokenize();
  void Parse();

 private:
  std::ifstream file_;
  std::vector<Config> &servers_;
  std::vector<std::string> tokens_;
};

#endif
