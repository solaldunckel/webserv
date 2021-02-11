#include "Request.hpp"

/*
** Constructors & Deconstructors
*/

Request::Request() {
}

Request::Request(std::string &msg) : msg_(msg) {
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
  print();
  std::string line, header;
  size_t last;
  std::stringstream input(msg_);

  // Headers
  while (std::getline(input, line)) {
    if ((last = line.find(':', 0)) != std::string::npos) {
      header = line.substr(0, last);
      if (headers_.count(header))
        headers_[header] = line.substr(last + 2);
    // } else {
    //   last = line.find(' ', 0);
    //   method_ = line.substr(0, last);
    //   last = line.find(' ', last);
    //   target_ = line.substr(last, line.find(' ', last));
    //   std::cout << "method :" << method_ << std::endl;
    //   std::cout << "target :" << target_ << std::endl;
    }
  }
}

void Request::print() {
  std::cout << msg_ << std::endl;
}