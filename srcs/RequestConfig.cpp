#include "RequestConfig.hpp"

/*
** Constructors & Deconstructors
*/

RequestConfig::RequestConfig(Request &request, std::string &host, std::vector<ServerConfig> &servers) : request_(request), host_(host), servers_(servers) {
}

RequestConfig::~RequestConfig() {
}

void RequestConfig::setup() {
  ServerConfig *server = getServerForRequest(servers_);
  ServerConfig *location = getLocationForRequest(server, request_.getTarget());

  server_ = server;
  location_ = server;

  if (location) {
    if (request_.getTarget().find(location->getUri()) != std::string::npos)
      target_ = target_.substr(location_->getUri().length());
  }

  if (location) {
    location_ = location;
    if (request_.getTarget().find(location->getUri()) != std::string::npos)
      target_ = request_.getTarget().substr(location_->getUri().length());
  }
}

ServerConfig *RequestConfig::getServerForRequest(std::vector<ServerConfig> &servers) {
  std::string host_port = request_.getHeader("Host");

  uint32_t port = 80;

  if (host_port.find(':') != std::string::npos)
    port = std::stod(host_port.substr(host_port.find(':') + 1));

  std::vector<ServerConfig>::iterator it = servers.begin();

  while (it != servers.end()) {
    for (std::vector<Listen>::iterator list = it->getListens().begin(); list != it->getListens().end(); list++) {
      if (list->ip_ == host_ && list->port_ == port) {
        std::cout << "MATCHING SERVER : " << list->ip_ << ":" << list->port_ << std::endl;
        return &(*it);
      }
    }
    it++;
  }
  it = servers.begin();
  while (it != servers.end()) {
    for (std::vector<Listen>::iterator list = it->getListens().begin(); list != it->getListens().end(); list++) {
      if (list->ip_ == "0.0.0.0" && list->port_ == port) {
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

std::string &RequestConfig::getMethod() {
  return request_.getMethod();
}

std::string &RequestConfig::getRoot() {
  return location_->getRoot();
}

std::string &RequestConfig::getBody() {
  return request_.getBody();
}

std::string &RequestConfig::getProtocol() {
  return request_.getProtocol();
}

bool RequestConfig::isValidRequest() {
  return request_.isValid();
}

int &RequestConfig::getClientMaxBodySize() {
  return location_->getClientMaxBodySize();
}

std::vector<std::string> &RequestConfig::getIndexes() {
  return location_->getIndexes();
}

bool RequestConfig::methodAccepted(std::string &method) {
  std::vector<std::string> methods = location_->getMethods();

  if (methods.empty())
    return true;
  if (std::find(methods.begin(), methods.end(), method) != methods.end())
    return true;
  return false;
}