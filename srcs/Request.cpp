#include "Request.hpp"

/*
** Constructors & Deconstructors
*/

// Request::Request() {
// }

Request::Request(std::string &msg, std::vector<ServerConfig> &servers) : msg_(msg), servers_(servers) {
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

std::string &Request::getHeader(std::string key) {
  return headers_[key];
}

std::string &Request::getTarget() {
  return target_;
}

std::string &Request::getMethod() {
  return method_;
}

void Request::setServer(std::string &server) {
  server_ = server;
}

std::vector<ServerConfig> &Request::getServers() {
  return servers_;
}

void Request::print() {
  std::cout << "\n### REQUEST\n\n";
  std::cout << msg_ << std::endl;
  std::cout << "###" << std::endl;
}