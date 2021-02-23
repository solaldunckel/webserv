// #ifndef LOCATIONCONFIG_HPP
// # define LOCATIONCONFIG_HPP

// # include "IConfig.hpp"
// # include "ServerConfig.hpp"

// class LocationConfig : public IConfig {
//  public:
//   // Constructors & Deconstructors
//   LocationConfig();
//   ~LocationConfig();

//   typedef void (LocationConfig::*type)(std::vector<std::string>::iterator &);
//   static std::map<std::string, type> directive_;
//   static void initDirectiveMap();

//   void setup(std::vector<std::string>::iterator &it);
//   void cgi(std::vector<std::string>::iterator &it);

//   std::string &getUri();

//   void print();

//  private:
//   std::string uri_;
//   std::vector<std::string> limit_except_;
//   std::map<std::string, std::string> cgi_;
// };

// #endif
