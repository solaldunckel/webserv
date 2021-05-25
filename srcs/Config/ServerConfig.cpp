#include "ServerConfig.hpp"

ServerConfig::ServerConfig() : credentials_("off"), autoindex_(false), client_max_body_size_(0), cgi_bin_("cgi-bin") {
  modifier_ = NONE;
  initDirectiveMap();
}

ServerConfig::~ServerConfig() {
}

ServerConfig	&ServerConfig::operator=(const ServerConfig &copy) {
  client_max_body_size_ = copy.client_max_body_size_;
  root_ = copy.root_;
  error_codes_ = copy.error_codes_;
  index_ = copy.index_;
  cgi_ = copy.cgi_;
  cgi_bin_ = copy.cgi_bin_;
  autoindex_ = copy.autoindex_;
  credentials_ = copy.credentials_;
  return (*this);
}

std::map<std::string, ServerConfig::type> ServerConfig::directive_;
void ServerConfig::initDirectiveMap() {
  ServerConfig::directive_["listen"] = &ServerConfig::listen;
  ServerConfig::directive_["location"] = &ServerConfig::location;
  ServerConfig::directive_["server_name"] = &ServerConfig::server_name;
  ServerConfig::directive_["error_page"] = &ServerConfig::error_page;
  ServerConfig::directive_["auth"] = &ServerConfig::auth;
  ServerConfig::directive_["client_max_body_size"] = &ServerConfig::client_max_body_size;
  ServerConfig::directive_["root"] = &ServerConfig::root;
  ServerConfig::directive_["limit_except"] = &ServerConfig::limit_except;
  ServerConfig::directive_["autoindex"] = &ServerConfig::autoindex;
  ServerConfig::directive_["index"] = &ServerConfig::index;
  ServerConfig::directive_["upload"] = &ServerConfig::upload;
  ServerConfig::directive_["cgi"] = &ServerConfig::cgi;
  ServerConfig::directive_["cgi_bin"] = &ServerConfig::cgi_bin;
}

void ServerConfig::server(std::vector<std::string>::iterator &it) {
  if (*it != "{")
    throw std::runtime_error("missing opening bracket in server block");
  while (*(++it) != "}") {
    if (ServerConfig::directive_[*it])
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
      throw std::runtime_error("invalid port :" + port_str);
    }

    port = ft::stoi(port_str);
    if (port > 65535)
      throw std::runtime_error("duplicate value in 'listen'");
  }
  else if (str.find_first_not_of("0123456789") != std::string::npos) {
    ip = str;
  }
  else
    port = ft::stoi(str);

  Listen list(ip, port);

  if (std::find(listens_.begin(), listens_.end(), list) != listens_.end())
    throw std::runtime_error("duplicate value in 'listen'");
    // throw webserv_exception("duplicate listen " + ip + ":" + port);

  listens_.push_back(Listen(ip, port));

  if (*++it != ";")
    throw std::runtime_error("double value in 'listen'");
}

void ServerConfig::client_max_body_size(std::vector<std::string>::iterator &it) {
  if (it->find_first_not_of("0123456789") != std::string::npos)
      throw std::runtime_error("unexpected symbols in client_max_body_size");
  client_max_body_size_ = ft::stoi(*it);
  if (*++it != ";")
    throw std::runtime_error("double value in 'client_max_body_size_'");
};

void ServerConfig::error_page(std::vector<std::string>::iterator &it) {
  std::vector<int> codes;

  while (it->find_first_not_of("0123456789") == std::string::npos) {
    codes.push_back(ft::stoi(*it++));
  }
  for (std::vector<int>::iterator it2 = codes.begin(); it2 != codes.end(); it2++) {
    error_codes_[*it2] = *it;
  }
  if (*++it != ";")
    throw std::runtime_error("double value in 'listen'");
};

void ServerConfig::auth(std::vector<std::string>::iterator &it) {
  credentials_ = *it;
  if (*++it != ";")
    throw std::runtime_error("double value in 'auth'");
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

void ServerConfig::autoindex(std::vector<std::string>::iterator &it) {
  if (*it == "on")
    autoindex_ = true;
  else if (*it == "off")
    autoindex_ = false;
  else
    throw std::runtime_error("unknown value in 'autoindex'");

  if (*++it != ";")
    throw std::runtime_error("double value in 'autoindex'");
};

void ServerConfig::upload(std::vector<std::string>::iterator &it) {
  upload_ = *it;
  if (*++it != ";")
    throw std::runtime_error("double value in 'upload'");
};

void ServerConfig::cgi_bin(std::vector<std::string>::iterator &it) {
  cgi_bin_ = *it;
  if (*++it != ";")
    throw std::runtime_error("double value in 'cgi_bin'");
};

bool is_loc_modifier(std::string &str) {
  return (str == "=" ||
          str == "~" ||
          str == "~*" ||
          str == "^~");
}

void ServerConfig::location_loop(std::vector<std::string>::iterator &it, std::vector<ServerConfig> &locations) {
  if (is_loc_modifier(*it)) {
    if (*it == "=")
      modifier_ = EXACT;
    #ifdef BONUS
    else if (*it == "~")
      modifier_ = CASE_SENSITIVE_REG;
    else if (*it == "~*")
      modifier_ = CASE_INSENSITIVE_REG;
    else if (*it == "^~")
      modifier_ = LONGEST;
    #endif
    else
      throw std::runtime_error("unknown modifier in location");
    it++;
  }
  else
    modifier_ = NONE;
  uri_ = *it++;
  if (*it != "{")
    throw std::runtime_error("missing opening bracket in server block");
  while (*(++it) != "}") {
    if (ServerConfig::directive_[*it])
      (this->*(ServerConfig::directive_[*it]))(++it);
    else
      throw std::runtime_error("invalid directive '" + *it + "' in 'location'");
  }
  locations.push_back(*this);;
}

void ServerConfig::location(std::vector<std::string>::iterator &it) {
  ServerConfig loc;

  loc = *this;
  loc.location_loop(it, locations_);
};

void ServerConfig::cgi(std::vector<std::string>::iterator &it) {
  std::string &ext = *it++;
  std::string &exec = *it++;
  cgi_[ext] = exec;
  if (*it != ";")
    throw std::runtime_error("triple value in 'cgi'");
};

/*
** Getter Functions
*/

std::string &ServerConfig::getUri() {
  return uri_;
}

std::vector<Listen> &ServerConfig::getListens() {
  return listens_;
};

std::vector<std::string> &ServerConfig::getServerNames() {
  return server_name_;
};

std::vector<ServerConfig> &ServerConfig::getLocations() {
  return locations_;
};
