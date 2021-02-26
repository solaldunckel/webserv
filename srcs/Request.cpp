#include "Request.hpp"

/*
** Constructors & Deconstructors
*/

// Request::Request() {
// }

Request::Request() : status_(FIRST_LINE) {
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

bool isValidMethod(std::string str) {
  return (str == "GET" || str == "POST" || str == "HEAD" || str == "PUT"
          || str == "DELETE");
}

void Request::initHeadersMap() {
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

// void Request::parse() {
//   std::string line, header;
//   size_t last;
//   std::stringstream input(msg_);

//   if (std::getline(input, line)) {
//     if ((last = line.find(' ')) != std::string::npos)
//       method_ = line.substr(0, last);
//     if (!isValidMethod(method_))
//       valid_ = false;
//     size_t tmp = last;
//     if ((last = line.find(' ', last + 1)) != std::string::npos)
//       target_ = line.substr(tmp + 1, last - tmp - 1);
//     protocol_ = line.substr(last + 1, line.find('\r', last) - last - 1);
//   }

//   while (std::getline(input, line)) {
//     if ((last = line.find(':', 0)) != std::string::npos) {
//       header = line.substr(0, last);
//       std::cout << header.length() << std::endl;
//       if (headers_.count(header))
//         headers_[header] = line.substr(last + 2, line.find('\r') - last - 2);
//     }
//     else
//       break;
//   }

//   while (std::getline(input, line)) {
//     req_body_ += line;
//   }
// }

int Request::parse(std::string buffer) {
  int ret = 0;
  buffer_ += buffer;

  if (status_ == FIRST_LINE) {
    std::cout << "PARSING FIRST LINE" << std::endl;
    ret = method_line();
  }
  if (status_ == HEADERS) {
    std::cout << "PARSING HEADERS" << std::endl;
    ret = headers();
  }
  if (status_ == BODY) {
    std::cout << "PARSING BODY" << std::endl;
    ret = body();
  }
  if (status_ == COMPLETE) {
    std::cout << "PARSING COMPLETE" << std::endl;
    ret = 1;
  }
  if (status_ == ERROR) {
    std::cout << "PARSING ERROR" << std::endl;
    ret = -1;
  }
  return ret;
}

int Request::method_line() {
  std::string tmp;

  if (buffer_.find_first_of("\r\n") != std::string::npos) {
    tmp = buffer_.substr(0, buffer_.find(' '));

    if (isValidMethod(tmp)) {
      method_ = tmp;
      buffer_ = buffer_.substr(method_.length() + 1);
    } else {
      status_ = ERROR;
      return -1;
    }

    tmp = buffer_.substr(0, buffer_.find(' '));

    if (tmp.length() < 10000) {
      target_ = tmp;
      buffer_ = buffer_.substr(target_.length() + 1);
    } else {
      status_ = ERROR;
      return -1;
    }

    tmp = buffer_.substr(0, buffer_.find_first_of("\r\n"));

    if (tmp == "HTTP/1.1") {
      protocol_ = tmp;
      buffer_ = buffer_.substr(buffer_.find_first_of("\r\n") + 2);
    } else {
      status_ = ERROR;
      return -1;
    }

    status_ = HEADERS;
  }

  return 1;
}

// GET / HTTP/1.1\r\n
// Content-Type: Test
// \r\n
// body

int Request::headers() {
  size_t end, last;
  std::string tmp;

  if (buffer_.find_first_of("\r\n") != std::string::npos) {
    while ((end = buffer_.find_first_of("\r\n")) != std::string::npos) {
      if (buffer_.find("\r\n") == 0) {
        buffer_ = buffer_.substr(end + 2);
        status_ = BODY;
        break;
      }
      if ((last = buffer_.find(':', 0)) != std::string::npos) {
        tmp = buffer_.substr(0, last);
        if (headers_.count(tmp))
          headers_[tmp] = buffer_.substr(last + 1, end);
      }
      buffer_ = buffer_.substr(buffer_.find_first_of("\r\n") + 2);
    }
  }
  return 1;
}

int Request::body() {
  size_t count = 0;

  if (buffer_.find_first_of("\r\n") != std::string::npos) {
    if (!headers_["Content-Length"].empty())
      count = std::stoi(headers_["Content-Length"]);

    std::cout <<buffer_.length() << std::endl;
    if (buffer_.length() > count) {
      req_body_ = std::string(buffer_, count);
      status_ = COMPLETE;
    }
  }
  return 1;
}

bool Request::isValid() {
  return valid_;
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

std::string &Request::getBody() {
  return req_body_;
}

std::string &Request::getProtocol() {
  return protocol_;
}

// std::vector<ServerConfig> &Request::getServers() {
//   return servers_;
// }

void Request::print() {
  std::cout << "\n### REQUEST\n\n";
  std::cout << "Method: " << method_ << std::endl;
  std::cout << "Target: " << target_ << std::endl;
  std::cout << "Protocol: " << protocol_ << std::endl;
  for (std::map<std::string, std::string>::iterator it = headers_.begin(); it != headers_.end(); it++) {
    if (!it->second.empty())
      std::cout << it->first << ": " << it->second << std::endl;;
  }
  std::cout << "Body: [" << req_body_ << "]" << std::endl;
  // std::cout << msg_ << std::endl; 
  std::cout << "###" << std::endl;
}