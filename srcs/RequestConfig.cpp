#include "RequestConfig.hpp"

/*
** Constructors & Deconstructors
*/

RequestConfig::RequestConfig(Request &request) : request_(request) {
}

RequestConfig::~RequestConfig() {
}

void RequestConfig::setup() {
  ServerConfig *server = getServerForRequest(request_.getServers());
  ServerConfig *location = getLocationForRequest(server, request_.getTarget());

  server_ = server;
  if (location)
    location_ = location;
  else
    location_ = server;

  target_ = request_.getTarget();

  // root_ = server->getRoot();
  // client_max_body_size_ = server->getClientMaxBodySize();
  // error_codes_ = server->getErrorCodes();

  // if (location) {
  //   if (!location->getRoot().empty())
  //     root_ = location->getRoot();
  //   if (location->getClientMaxBodySize() > 0)
  //     root_ = location->getClientMaxBodySize();
  // }
}

ServerConfig *RequestConfig::getServerForRequest(std::vector<ServerConfig> &servers) {
  std::string host_port = request_.getHeader("Host");

  std::string host = host_port.substr(0, host_port.find(':'));
  uint32_t port = std::stod(host_port.substr(host_port.find(':') + 1));

  std::vector<ServerConfig>::iterator it = servers.begin();

  while (it != servers.end()) {
    for (std::vector<Listen>::iterator list = it->getListens().begin(); list != it->getListens().end(); list++) {
      if (list->ip_ == host && list->port_ == port) {
        std::cout << "MATCHING SERVER : " << list->ip_ << ":" << list->port_ << std::endl;
        return &(*it);
      }
    }
    it++;
  }
  return nullptr;
}

ServerConfig *RequestConfig::getLocationForRequest(ServerConfig *server, std::string target) {
  if (!target.length())
    target = "/";

  for (std::vector<ServerConfig>::iterator it = server->getLocations().begin(); it != server->getLocations().end(); it++) {
    if (it->getUri() == target) {
      std::cout << "MATCHING LOCATION : " << it->getUri() << std::endl;
      return &(*it);
    }
  }

  if (target == "/")
    return nullptr;
  return getLocationForRequest(server, target.substr(0, target.find_last_of('/')));
}

/* GETTERS ! */

std::string &RequestConfig::getTarget() {
  return target_;
}

Request &RequestConfig::getRequest() {
  return request_;
}

std::string &RequestConfig::getRoot() {
  return location_->getRoot();
}

int &RequestConfig::getClientMaxBodySize() {
  return location_->getClientMaxBodySize();
}

std::vector<std::string> &RequestConfig::getIndexes() {
  return location_->getIndexes();
}