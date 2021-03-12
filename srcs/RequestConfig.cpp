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
  ServerConfig *location = getLocationForRequest(server, request_.target_);

  server_ = server;
  location_ = server;

  target_ = request_.target_;

  if (location) {
    location_ = location;
    if (request_.target_.find(location->uri_) != std::string::npos) {
      target_.erase(0, location_->uri_.length());
      target_ = ft::trim_left(target_, '/');
    }
  }
}

// http://nginx.org/en/docs/http/request_processing.html
// https://www.digitalocean.com/community/tutorials/understanding-nginx-server-and-location-block-selection-algorithms

ServerConfig *RequestConfig::getServerForRequest(std::vector<ServerConfig> &servers) {
  std::string host_port = request_.headers_["Host"];

  uint32_t port = 80;

  std::string host = host_port.substr(0, host_port.find(':'));

  if (host_port.find(':') != std::string::npos)
    port = std::stod(host_port.substr(host_port.find(':') + 1));

  std::vector<ServerConfig>::iterator it = servers.begin();

  // Handle server names with * ?
  while (it != servers.end()) {
    for (std::vector<Listen>::iterator list = it->listens_.begin(); list != it->listens_.end(); list++) {
      if (list->ip_ == host_ && list->port_ == port) {
        #ifdef DEBUG
        std::cout << "MATCHING SERVER : " << list->ip_ << ":" << list->port_ << " from " << host_ << std::endl;
        #endif
        port_ = list->port_;
        return &(*it);
      }
    }
    it++;
  }
  it = servers.begin();
  while (it != servers.end()) {
    for (std::vector<Listen>::iterator list = it->listens_.begin(); list != it->listens_.end(); list++) {
      if (list->ip_ == "0.0.0.0" && list->port_ == port) {
        #ifdef DEBUG
        std::cout << "MATCHING SERVER : " << list->ip_ << ":" << list->port_ << std::endl;
        #endif
        port_ = list->port_;
        host_ = host; // ?
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

  for (std::vector<ServerConfig>::iterator it = server->locations_.begin(); it != server->locations_.end(); it++) {
    if (it->uri_ == target) {
      #ifdef DEBUG
      std::cout << "MATCHING LOCATION : " << it->uri_ << std::endl;
      #endif
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

std::string &RequestConfig::getHost() {
  return host_;
}

uint32_t &RequestConfig::getPort() {
  return port_;
}

std::string &RequestConfig::getRoot() {
  return location_->root_;
}

std::string &RequestConfig::getAuth() {
  return location_->credentials_;
}

std::string &RequestConfig::getUri() {
  return location_->uri_;
}

std::map<std::string, std::string> &RequestConfig::getCGI() {
  return location_->cgi_;
}

size_t &RequestConfig::getClientMaxBodySize() {
  return location_->client_max_body_size_;
}

std::string &RequestConfig::getUpload() {
  return location_->upload_;
}

std::vector<std::string> &RequestConfig::getIndexes() {
  return location_->index_;
}

std::map<int, std::string> &RequestConfig::getErrorPages() {
  return location_->error_codes_;
}

std::vector<std::string> &RequestConfig::getMethods() {
  return location_->methods_;
}

std::string &RequestConfig::getBody() {
  return request_.req_body_;
}

std::string &RequestConfig::getMethod() {
  return request_.method_;
}

std::string &RequestConfig::getHeader(std::string key) {
  return request_.headers_[key];
}

std::map<std::string, std::string, ft::comp> &RequestConfig::getHeaders() {
  return request_.headers_;
}

std::string &RequestConfig::getProtocol() {
  return request_.protocol_;
}

bool RequestConfig::methodAccepted(std::string &method) {
  std::vector<std::string> methods = location_->methods_;

  if (methods.empty())
    return true;
  if (std::find(methods.begin(), methods.end(), method) != methods.end())
    return true;
  return false;
}
