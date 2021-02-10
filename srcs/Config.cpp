#include "Config.hpp"

/*
** Constructors & Deconstructors
*/

Config::Config() {
}

Config::~Config() {
}

std::map<std::string, Config::type> Config::dir;
void Config::initMap()
{
    Config::dir["listen"] = &Config::listen;
    Config::dir["server_name"] = &Config::server_name;
    Config::dir["error_page"] = &Config::error_page;
    Config::dir["client_max_body_size"] = &Config::client_max_body_size;
}

void Config::server(std::vector<std::string>::iterator &it) {
  initMap();
  if (*it != "{")
      throw std::runtime_error("missing opening bracket in server block");
  while (*(++it) != "}")
  {
      if (*it == "location") {
        Location loc;

        loc.setup(++it);
        locations_.push_back(loc);
      }
      else if (Config::dir[*it])
        (this->*(Config::dir[*it]))(++it);
      else
          throw std::runtime_error("invalid directive '" + *it + "' in 'server'");
  }
}

void Config::listen(std::vector<std::string>::iterator &it) {
  listen_.setup(*it);
  // if (!listen_.valid())
  //   throw std::runtime_error("'listen' invalid");
  if (*++it != ";")
      throw std::runtime_error("double value in 'listen'");
}

void Config::server_name(std::vector<std::string>::iterator &it)
{
    while (*it != ";")
      server_name_.push_back(*it++);
}

void Config::error_page(std::vector<std::string>::iterator &it)
{
    std::vector<int> codes;

    while (it->find_first_not_of("0123456789") == std::string::npos) {
      codes.push_back(std::stoi(*it++));
    }
    for (std::vector<int>::iterator it2 = codes.begin(); it2 != codes.end(); it2++) {
      error_codes_[*it2] = *it;
    }
    if (*++it != ";")
      throw std::runtime_error("double value in 'listen'");
}

void Config::client_max_body_size(std::vector<std::string>::iterator &it)
{
    if (it->find_first_not_of("0123456789") != std::string::npos)
        throw std::runtime_error("unexpected symbols in client_max_body_size");
    client_max_body_size_ = std::stoi(*it++);
}

void Config::print() {
  std::cout << "Server :" << std::endl;
  std::cout << "  listen : " << listen_.getIp() << ":" << listen_.getPort() << std::endl;
  std::cout << "  server_name :" << std::endl;
  for (std::vector<std::string>::iterator it = server_name_.begin();
    it != server_name_.end(); it++) {
      std::cout << "    " << *it << std::endl;
    }
  std::cout << "  client_max_body_size : " << client_max_body_size_ << std::endl;
  std::cout << "  Locations :" << std::endl;
  for (std::vector<Location>::iterator it = locations_.begin(); it != locations_.end(); it++) {
    it->print();
  }
}


