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

    std::string port_str = str.substr(str.find(':') + 1);

    if (port_str.find_first_not_of("0123456789") != std::string::npos) {
      std::cout << "Invalid port" << std::endl;
    }

    port = std::stoi(port_str);
  }
  else if (str.find_first_not_of("0123456789") != std::string::npos) {
    ip = str;
  }
  else
    port = std::stoi(str);

  listens_.push_back(Listen(ip, port));

  if (*++it != ";")
      throw std::runtime_error("double value in 'listen'");
}

/*
** Getter Functions
*/

std::vector<Listen> &ServerConfig::getListens() {
  return listens_;
};

std::vector<std::string> &ServerConfig::getServerNames() {
  return server_name_;
};

std::vector<LocationConfig> &ServerConfig::getLocations() {
  return locations_;
};

/* Debug Functions */

void ServerConfig::print() {
  std::cout << "Server :" << std::endl;

  std::cout << "  listen :" << std::endl;
  for (std::vector<Listen>::iterator it = listens_.begin();
    it != listens_.end(); it++) {
      std::cout << "    " << it->ip_ << ":" << it->port_ << std::endl;
  }
  std::cout << "  server_name :" << std::endl;
  for (std::vector<std::string>::iterator it = server_name_.begin(); it != server_name_.end(); it++) {
      std::cout << "    " << *it << std::endl;
  }
  if (client_max_body_size_ > 0)
    std::cout << "  client_max_body_size : " << client_max_body_size_ << std::endl;
  if (locations_.size() > 0) {
    std::cout << "  Locations :" << std::endl;
    for (std::vector<LocationConfig>::iterator it = locations_.begin(); it != locations_.end(); it++) {
      it->print();
    }
  }
}
