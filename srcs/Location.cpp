#include "Location.hpp"

/*
** Constructors & Deconstructors
*/

Location::Location() {
}

Location::~Location() {
}

std::map<std::string, Location::type> Location::dir;
void Location::initMap()
{
    Location::dir["index"] = &Location::index;
    Location::dir["cgi"] = &Location::cgi;
    // Location::dir["error_page"] = &Location::error_page;
    // Location::dir["client_max_body_size"] = &Location::client_max_body_size;
}

void Location::setup(std::vector<std::string>::iterator &it) {
  initMap();
  uri_ = *it++;
  if (*it != "{")
      throw std::runtime_error("missing opening bracket in server block");
  while (*(++it) != "}")
  {
      if (Location::dir[*it])
        (this->*(Location::dir[*it]))(++it);
      else
          throw std::runtime_error("invalid directive '" + *it + "' in 'location'");
  }
}

void Location::index(std::vector<std::string>::iterator &it) {
  while (*it != ";") {
    index_.push_back(*it++);
  }
}

void Location::cgi(std::vector<std::string>::iterator &it) {
  std::string ext = *it++;

  cgi_[ext] = *it++;
}

void Location::print() {
  std::cout << "Location :" << std::endl;
  std::cout << "  uri : " << uri_ << std::endl;
  std::cout << "  index :" << std::endl;
  for (std::vector<std::string>::iterator it = index_.begin(); it != index_.end(); it++)
    std::cout << "    " << *it << std::endl;
  std::cout << "  cgi : " << std::endl;
  for (std::map<std::string, std::string>::iterator it = cgi_.begin(); it != cgi_.end(); it++)
    std::cout << "    " << it->first << " -> " << it->second << std::endl;
  std::cout << "  root : " << root_ << std::endl;
}

