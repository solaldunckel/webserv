#include "ServerConfig.hpp"

/*
** Constructors & Deconstructors
*/

ServerConfig::ServerConfig() : client_max_body_size_(0) {
  initDirectiveMap();
}

ServerConfig::~ServerConfig() {
}

ServerConfig	&ServerConfig::operator=(const ServerConfig &copy) {
  client_max_body_size_ = copy.client_max_body_size_;
  root_ = copy.root_;
  error_codes_ = copy.error_codes_;
  index_ = copy.index_;
  return (*this);
}

std::map<std::string, ServerConfig::type> ServerConfig::directive_;
void ServerConfig::initDirectiveMap() {
  ServerConfig::directive_["listen"] = &ServerConfig::listen;
  ServerConfig::directive_["location"] = &ServerConfig::location;
  ServerConfig::directive_["server_name"] = &ServerConfig::server_name;
  ServerConfig::directive_["error_page"] = &ServerConfig::error_page;
  ServerConfig::directive_["client_max_body_size"] = &ServerConfig::client_max_body_size;
  ServerConfig::directive_["root"] = &ServerConfig::root;
  ServerConfig::directive_["limit_except"] = &ServerConfig::limit_except;
  ServerConfig::directive_["index"] = &ServerConfig::index;
  ServerConfig::directive_["cgi"] = &ServerConfig::cgi;
}

void ServerConfig::server(std::vector<std::string>::iterator &it) {
  if (*it != "{")
    throw std::runtime_error("missing opening bracket in server block");
  while (*(++it) != "}") {
    if (*it == "location") {
      ServerConfig loc;

      loc = *this;
      loc.location(++it);
      locations_.push_back(loc);
    }
    else if (ServerConfig::directive_[*it])
      (this->*(ServerConfig::directive_[*it]))(++it);
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

void ServerConfig::client_max_body_size(std::vector<std::string>::iterator &it) {
  if (it->find_first_not_of("0123456789") != std::string::npos)
      throw std::runtime_error("unexpected symbols in client_max_body_size");
  client_max_body_size_ = std::stoi(*it);
  if (*++it != ";")
    throw std::runtime_error("double value in 'client_max_body_size_'");
};

void ServerConfig::error_page(std::vector<std::string>::iterator &it) {
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

void ServerConfig::root(std::vector<std::string>::iterator &it) {
  root_ = *it;
  if (*++it != ";")
    throw std::runtime_error("double value in 'root'");
};

void ServerConfig::index(std::vector<std::string>::iterator &it) {
  while (*it != ";")
    index_.push_back(*it++);
};

void ServerConfig::limit_except(std::vector<std::string>::iterator &it) {
  while (*it != ";")
    methods_.push_back(*it++);
};

void ServerConfig::location(std::vector<std::string>::iterator &it) {
  uri_ = *it++;
  if (*it != "{")
    throw std::runtime_error("missing opening bracket in server block");
  while (*(++it) != "}") {
    if (ServerConfig::directive_[*it])
      (this->*(ServerConfig::directive_[*it]))(++it);
    else
      throw std::runtime_error("invalid directive '" + *it + "' in 'location'");
  }
};

void ServerConfig::cgi(std::vector<std::string>::iterator &it) {
  std::string ext = *it++;

  cgi_[ext] = *it++;
};

std::string &ServerConfig::getUri() {
  return uri_;
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

std::vector<ServerConfig> &ServerConfig::getLocations() {
  return locations_;
};

size_t &ServerConfig::getClientMaxBodySize() {
  return client_max_body_size_;
}

std::string &ServerConfig::getRoot() {
  return root_;
}

std::map<int, std::string>  &ServerConfig::getErrorCodes() {
  return error_codes_;
}

std::vector<std::string> &ServerConfig::getIndexes() {
  return index_;
}

std::vector<std::string> &ServerConfig::getMethods() {
  return methods_;
}

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
    for (std::vector<ServerConfig>::iterator it = locations_.begin(); it != locations_.end(); it++) {
      it->printLocation();
    }
  }
}

void ServerConfig::printLocation() {
  std::cout << "    - uri : " << uri_ << std::endl;

  if (!index_.empty()) {
    std::cout << "      index :" << std::endl;
    for (std::vector<std::string>::iterator it = index_.begin(); it != index_.end(); it++)
      std::cout << "        " << *it << std::endl;
  }

  if (!cgi_.empty()) {
    std::cout << "      cgi : " << std::endl;
    for (std::map<std::string, std::string>::iterator it = cgi_.begin(); it != cgi_.end(); it++)
      std::cout << "        " << it->first << " -> " << it->second << std::endl;
  }

  if (!root_.empty())
    std::cout << "      root : " << root_ << std::endl;
}
