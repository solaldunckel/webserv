#ifndef ICONFIG_HPP
# define ICONFIG_HPP

# include <iostream>
# include <map>
# include <vector>

class IConfig {
 public:
  // Constructors & Deconstructors
  IConfig() {};
  ~IConfig() {};

  void client_max_body_size(std::vector<std::string>::iterator &it) {
    if (it->find_first_not_of("0123456789") != std::string::npos)
        throw std::runtime_error("unexpected symbols in client_max_body_size");
    client_max_body_size_ = std::stoi(*it);
    if (*++it != ";")
      throw std::runtime_error("double value in 'client_max_body_size_'");
  };

  void error_page(std::vector<std::string>::iterator &it) {
    std::vector<int> codes;

    while (it->find_first_not_of("0123456789") == std::string::npos) {
      codes.push_back(std::stoi(*it++));
    }
    for (std::vector<int>::iterator it2 = codes.begin(); it2 != codes.end(); it2++) {
      error_codes_[*it2] = *it;
    }
    if (*++it != ";")
      throw std::runtime_error("double value in 'listen'");
  };

  void root(std::vector<std::string>::iterator &it) {
    root_ = *it;
    if (*++it != ";")
      throw std::runtime_error("double value in 'root'");
  };

  // virtual void print() = 0;

 protected:
  int client_max_body_size_;
  std::string root_;
  std::map<int, std::string> error_codes_;
};

#endif