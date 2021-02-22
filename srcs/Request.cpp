#include "Request.hpp"

/*
** Constructors & Deconstructors
*/

Request::Request() {
}

Request::Request(std::string &msg) : msg_(msg), server_(nullptr), location_(nullptr) {
  headers_["Accept-Charsets"];
  headers_["Accept-Language"];
  headers_["Allow"];
  headers_["Authorization"];
  headers_["Content-Language"];
  headers_["Content-Length"];
  headers_["Content-Location"];
  headers_["Content-Type"];
  headers_["Date"];
  headers_["Host"];
  headers_["Last-Modified"];
  headers_["Location"];
  headers_["Referer"];
  headers_["Retry-After"];
  headers_["Server"];
  headers_["Transfer-Encoding"];
  headers_["User-Agent"];
  headers_["WWW-Authenticate"];
}

Request::~Request() {
}

void Request::parse() {
  std::string line, header;
  size_t last;
  std::stringstream input(msg_);

  if (std::getline(input, line)) {
    if ((last = line.find(' ')) != std::string::npos)
      method_ = line.substr(0, last);
    size_t tmp = last;
    if ((last = line.find(' ', last + 1)) != std::string::npos)
      target_ = line.substr(tmp + 1, last - tmp - 1);
    protocol_ = line.substr(last + 1, line.find('\r', last) - last - 1);
  }

  while (std::getline(input, line)) {
    if ((last = line.find(':', 0)) != std::string::npos) {
      header = line.substr(0, last);
      if (headers_.count(header))
        headers_[header] = line.substr(last + 2, line.find('\r') - last - 2);
    }
    else
      break;
  }

  while (std::getline(input, line)) {
    req_body_ += line;
  }
}

ServerConfig *Request::getServerForRequest(std::vector<ServerConfig> &servers) {
  std::string host_port = headers_["Host"];

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

LocationConfig *Request::getLocationForRequest(std::string target) {
  if (!server_)
    return nullptr;
  if (!target.length())
    target = "/";

  for (std::vector<LocationConfig>::iterator it = server_->getLocations().begin(); it != server_->getLocations().end(); it++) {
    if (it->getUri() == target) {
      std::cout << "MATCHING LOCATION : " << it->getUri() << std::endl;
      return &(*it);
    }
  }

  if (target == "/")
    return nullptr;
  return getLocationForRequest(target.substr(0, target.find_last_of('/')));
}

void Request::removeUriFromTarget() {
  if (location_) {
    std::cout << "TARGET : [" << target_  << "]" << std::endl;
    if (target_.find(location_->getUri()) != std::string::npos) {
      target_ = target_.substr(location_->getUri().length());
      std::cout << "NEW TARGET: [" << target_ << "]" << std::endl;
    }
  }
}

void Request::setServer(ServerConfig *server) {
  server_ = server;
}

void Request::setLocation(LocationConfig *location) {
  location_ = location;
}

std::string &Request::getTarget() {
  return target_;
}

std::string &Request::getRoot() {
  if (location_) {
    return location_->getRoot();
  }
  return server_->getRoot();
}

void Request::print() {
  std::cout << "\n###\n\n";
  std::cout << msg_ << std::endl;
}