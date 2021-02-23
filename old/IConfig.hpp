// #ifndef ICONFIG_HPP
// # define ICONFIG_HPP

// # include <iostream>
// # include <map>
// # include <vector>

// class IConfig {
//  public:
//   // Constructors & Deconstructors
//   IConfig() {};
//   ~IConfig() {};

//   void client_max_body_size(std::vector<std::string>::iterator &it) {
//     if (it->find_first_not_of("0123456789") != std::string::npos)
//         throw std::runtime_error("unexpected symbols in client_max_body_size");
//     client_max_body_size_ = std::stoi(*it);
//     if (*++it != ";")
//       throw std::runtime_error("double value in 'client_max_body_size_'");
//   };

//   void error_page(std::vector<std::string>::iterator &it) {
//     std::vector<int> codes;

//     while (it->find_first_not_of("0123456789") == std::string::npos) {
//       codes.push_back(std::stoi(*it++));
//     }
//     for (std::vector<int>::iterator it2 = codes.begin(); it2 != codes.end(); it2++) {
//       error_codes_[*it2] = *it;
//     }
//     if (*++it != ";")
//       throw std::runtime_error("double value in 'listen'");
//   };

//   void root(std::vector<std::string>::iterator &it) {
//     root_ = *it;
//     if (*++it != ";")
//       throw std::runtime_error("double value in 'root'");
//   };

//   void index(std::vector<std::string>::iterator &it) {
//     while (*it != ";") {
//       index_.push_back(*it++);
//   }
// }

//   IConfig	&operator=(const IConfig &copy) {
//     std::cout << "= OPERATOR" << std::endl;
//     client_max_body_size_ = copy.client_max_body_size_;
//     root_ = copy.root_;
//     error_codes_ = copy.error_codes_;
//     return (*this);
//   }

//   int &getClientMaxBodySize() {
//     return client_max_body_size_;
//   }

//   std::string &getRoot() {
//     return root_;
//   }

//   std::map<int, std::string>  &getErrorCodes() {
//     return error_codes_;
//   }

//   std::vector<std::string> &getIndexes() {
//     return index_;
//   }

//  protected:
//   int client_max_body_size_;
//   std::string root_;
//   std::map<int, std::string> error_codes_;
//   std::vector<std::string> index_;
// };

// #endif