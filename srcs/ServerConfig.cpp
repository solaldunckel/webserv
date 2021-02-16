#include "ServerConfig.hpp"

/*
** Constructors & Deconstructors
*/

ServerConfig::ServerConfig() {
  initDirectiveMap();
}

ServerConfig::~ServerConfig() {
}

std::map<std::string, ServerConfig::type> ServerConfig::directive;
void ServerConfig::initDirectiveMap()
{
    ServerConfig::directive["listen"] = &ServerConfig::listen;
    ServerConfig::directive["server_name"] = &ServerConfig::server_name;
    ServerConfig::directive["error_page"] = &ServerConfig::error_page;
    ServerConfig::directive["client_max_body_size"] = &ServerConfig::client_max_body_size;
    ServerConfig::directive["root"] = &ServerConfig::root;
}

void ServerConfig::server(std::vector<std::string>::iterator &it) {
  if (*it != "{")
      throw std::runtime_error("missing opening bracket in server block");
  while (*(++it) != "}")
  {
      if (*it == "location") {
        LocationConfig loc;

        loc.setup(++it);
        locations_.push_back(loc);
      }
      else if (ServerConfig::directive[*it])
        (this->*(ServerConfig::directive[*it]))(++it);
      else
          throw std::runtime_error("invalid directive '" + *it + "' in 'server'");
  }
}

void ServerConfig::server_name(std::vector<std::string>::iterator &it) {
    while (*it != ";")
      server_name_.push_back(*it++);
}

void ServerConfig::listen(std::vector<std::string>::iterator &it) {
  std::string str = *it;
  std::string ip = "0.0.0.0";
  uint32_t port = 8000;

  if (str.find(':') != std::string::npos) {
    ip = str.substr(0, str.find(':'));

    std::string port = str.substr(str.find(':') + 1);

    if (port.find_first_not_of("0123456789") != std::string::npos) {
      std::cout << "Invalid port" << std::endl;
    }

    port = std::stoi(port);
  }
  else if (str.find_first_not_of("0123456789") != std::string::npos) {
    ip = str;
  }
  else
    port = std::stoi(str);

  host_ips_[ip] = port;

  if (*++it != ";")
      throw std::runtime_error("double value in 'listen'");
}

void ServerConfig::print() {
  std::cout << "Server :" << std::endl;

  std::cout << "  listen :" << std::endl;
  for (std::map<std::string, uint32_t>::iterator it = host_ips_.begin();
    it != host_ips_.end(); it++) {
      std::cout << "    " << it->first << ":" << it->second << std::endl;
  }
  std::cout << "  server_name :" << std::endl;
  for (std::vector<std::string>::iterator it = server_name_.begin();
    it != server_name_.end(); it++) {
      std::cout << "    " << *it << std::endl;
  }
  std::cout << "  client_max_body_size : " << client_max_body_size_ << std::endl;
  std::cout << "  Locations :" << std::endl;
  for (std::vector<LocationConfig>::iterator it = locations_.begin(); it != locations_.end(); it++) {
    it->print();
  }
}
