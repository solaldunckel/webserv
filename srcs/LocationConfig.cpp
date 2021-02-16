#include "LocationConfig.hpp"

/*
** Constructors & Deconstructors
*/

LocationConfig::LocationConfig() {
  initDirectiveMap();
}

LocationConfig::~LocationConfig() {
}

std::map<std::string, LocationConfig::type> LocationConfig::directive;
void LocationConfig::initDirectiveMap()
{
    LocationConfig::directive["index"] = &LocationConfig::index;
    LocationConfig::directive["cgi"] = &LocationConfig::cgi;
    LocationConfig::directive["error_page"] = &LocationConfig::error_page;
    LocationConfig::directive["root"] = &LocationConfig::root;
    LocationConfig::directive["client_max_body_size"] = &LocationConfig::client_max_body_size;
}

void LocationConfig::setup(std::vector<std::string>::iterator &it) {
  uri_ = *it++;
  if (*it != "{")
      throw std::runtime_error("missing opening bracket in server block");
  while (*(++it) != "}")
  {
      if (LocationConfig::directive[*it])
        (this->*(LocationConfig::directive[*it]))(++it);
      else
          throw std::runtime_error("invalid directive '" + *it + "' in 'location'");
  }
}

void LocationConfig::index(std::vector<std::string>::iterator &it) {
  while (*it != ";") {
    index_.push_back(*it++);
  }
}

void LocationConfig::cgi(std::vector<std::string>::iterator &it) {
  std::string ext = *it++;

  cgi_[ext] = *it++;
}

void LocationConfig::print() {
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