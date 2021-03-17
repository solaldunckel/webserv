#include "RequestConfig.hpp"

/*
** Constructors & Deconstructors
*/

RequestConfig::RequestConfig(Request &request, Listen &host_port, std::vector<ServerConfig> &servers, Client &client) :
  request_(request),
  host_port_(host_port),
  servers_(servers),
  client_(client)
{
  setup();
}

RequestConfig::~RequestConfig() {
}

void RequestConfig::setup() {
  ServerConfig *server = getServerForRequest(servers_);
  ServerConfig *location = nullptr;

  if (request_.getStatus() > 2)
    location = getLocationForRequest(server, request_.target_);

  server_ = server;
  location_ = server;

  target_ = request_.target_;
  if (location) {
    location_ = location;
    if (request_.target_.find(location->uri_) != std::string::npos) {
      target_.erase(0, location_->uri_.length());
    }
  }
}

bool RequestConfig::redirectLocation(std::string target) {
  ServerConfig *location = nullptr;

  if (request_.getStatus() > 2)
    location = getLocationForRequest(server_, target);

  if (location != location_) {
    location_ = location;
    target_ = target;
    if (target_.find(location->uri_) != std::string::npos) {
      target_.erase(0, location_->uri_.length());
    }
    return true;
  }
  return false;
}

ServerConfig *RequestConfig::getServerForRequest(std::vector<ServerConfig> &servers) {
  std::vector<ServerConfig*> matching_servers;

  // Match server based on request ip + port
  for (std::vector<ServerConfig>::iterator it = servers.begin(); it != servers.end(); it++) {
    for (std::vector<Listen>::iterator list = it->listens_.begin(); list != it->listens_.end(); list++) {
      if ((list->ip_ == host_port_.ip_ || list->ip_ == "0.0.0.0") && list->port_ == host_port_.port_) {
        matching_servers.push_back(&(*it));
        break;
      }
    }
  }

  // If only one match, it's our server
  if (matching_servers.size() == 1)
    return matching_servers.front();

  std::string host = request_.headers_["Host"].substr(0, request_.headers_["Host"].find(':'));

  // We match based on server names
  for (std::vector<ServerConfig*>::iterator it = matching_servers.begin(); it != matching_servers.end(); it++) {
    std::vector<std::string> server_names = (*it)->getServerNames();

    for (std::vector<std::string>::iterator server_name = server_names.begin(); server_name != server_names.end(); server_name++) {
      if (host == *server_name) {
        std::cout << "MATCHED SERVER BY SERVER NAME : " << *server_name << std::endl;
        return *it;
      }
    }
  }

  // We use default server (first one)
  return matching_servers.front();
}

bool sort_by_uri_len(ServerConfig *lhs, ServerConfig *rhs) {
  return (lhs->getUri().length() > rhs->getUri().length());
}

ServerConfig *RequestConfig::getLocationForRequest(ServerConfig *server, std::string &target) {
  ServerConfig *location = nullptr;

  std::vector<ServerConfig*> prefix_matches;
  std::vector<ServerConfig*> reg_locations;

  for (std::vector<ServerConfig>::iterator it = server->locations_.begin(); it != server->locations_.end(); it++) {
    if (it->modifier_ != 2 && it->modifier_ != 3) {
      if (it->modifier_ == 1 && it->uri_ == target) {
        std::cout << "MATCHING EXACT LOCATION : " << it->uri_ << std::endl;
        location = &(*it);
        break ;
      }
      if (target.find(it->uri_) == 0) {
        prefix_matches.push_back(&(*it));
      }
    else
      reg_locations.push_back(&(*it));
    }
  }

  if (location)
    return location;

  std::sort(prefix_matches.begin(), prefix_matches.end(), sort_by_uri_len);
  location = prefix_matches.front();
  if (location->modifier_ == 4)
    return location;

  regex_t test;

  for (std::vector<ServerConfig*>::iterator it = reg_locations.begin(); it != reg_locations.end(); it++) {
    int err = 1;
    if ((*it)->modifier_ == 2)
      err = regcomp(&test, (*it)->uri_.c_str(), REG_NOSUB | REG_EXTENDED);
    else
      err = regcomp(&test, (*it)->uri_.c_str(), REG_NOSUB | REG_EXTENDED | REG_ICASE);
    if (err == 0) {
      int match = regexec(&test, target.c_str(), 0, NULL, 0);
      regfree(&test);
      if (match == 0)
        return *it;
    }
  }
  // location = findLongestLocationMatch(prefix_matches, target);
  return location;
}

ServerConfig *RequestConfig::findLongestLocationMatch(std::vector<ServerConfig> &matches, std::string target) {
  if (!target.length())
    target = "/";

  for (std::vector<ServerConfig>::iterator it = matches.begin(); it != matches.end(); it++) {
    if (it->uri_ == target) {
      // #ifdef DEBUG
      std::cout << "MATCHING LOCATION : " << it->uri_ << std::endl;
      // #endif
      return &(*it);
    }
  }

  if (target == "/")
    return nullptr;
  if (target.find_last_of('/') == target.length() - 1)
    return findLongestLocationMatch(matches, target.substr(0, target.find_last_of('/')));
  return findLongestLocationMatch(matches, target.substr(0, target.find_last_of('/') + 1));
}

bool RequestConfig::methodAccepted(std::string &method) {
  std::vector<std::string> methods = location_->methods_;

  if (methods.empty())
    return true;
  if (std::find(methods.begin(), methods.end(), method) != methods.end())
    return true;
  return false;
}

/* GETTERS ! */

std::string &RequestConfig::getTarget() {
  return target_;
}

std::string &RequestConfig::getQuery() {
  return request_.query_string_;
}

void RequestConfig::setTarget(std::string target) {
  target_ = target;
}

std::string &RequestConfig::getHost() {
  return host_port_.ip_;
}

uint32_t &RequestConfig::getPort() {
  return host_port_.port_;
}

Client &RequestConfig::getClient() {
  return client_;
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

std::string &RequestConfig::getCGIBin() {
  return location_->cgi_bin_;
}

size_t &RequestConfig::getClientMaxBodySize() {
  return location_->client_max_body_size_;
}

bool RequestConfig::getAutoindex() {
  return location_->autoindex_;
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
