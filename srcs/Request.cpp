#include "Request.hpp"

/*
** Constructors & Deconstructors
*/

Request::Request() {
  body_offset_ = 0;
  chunk_size_ = 0;
  status_ = FIRST_LINE;
  initHeadersMap();
}

Request::~Request() {}

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

int Request::parse(std::string &buffer) {
  size_t ret = 0;
  buffer_ += buffer;

  if (status_ == FIRST_LINE)
    ret = method_line();
  if (status_ == HEADERS)
    ret = headers();
  if (status_ == PREBODY)
    ret = prebody();
  if (status_ == BODY)
    ret = body();
  if (status_ == CHUNK)
    ret = chunk();
  if (status_ == COMPLETE || ret == 1) {
    status_ = COMPLETE;
    return ret;
  }
  else if (status_ == ERROR || ret > 1) {
    status_ = ERROR;
    return ret;
  }
  return ret;
}

int Request::method_line() {
  if (buffer_.find("\r\n") != std::string::npos) {
    std::string tmp = buffer_.substr(0, buffer_.find(' '));

    if (isValidMethod(tmp)) {
      method_ = tmp;
      buffer_.erase(0, method_.length() + 1);
    } else {
      status_ = ERROR;
      return 400;
    }

    tmp = buffer_.substr(0, buffer_.find(' '));

    if (tmp.length() < 10000) {
      target_ = tmp;
      buffer_.erase(0, target_.length() + 1);
    } else {
      status_ = ERROR;
      return 414;
    }

    size_t end = buffer_.find("\r\n");
    tmp = buffer_.substr(0, end);

    if (tmp == "HTTP/1.1") {
      protocol_ = tmp;
      buffer_.erase(0, end + 2);
    } else {
      status_ = ERROR;
      return 505;
    }

    status_ = HEADERS;
  }

  return 0;
}

int Request::headers() {
  size_t end, last;
  std::string header;

  while ((end = buffer_.find("\r\n")) != std::string::npos) {
    if (buffer_.find("\r\n") == 0) {
      buffer_.erase(0, end + 2);
      status_ = PREBODY;
      break;
    }
    if ((last = buffer_.find(':', 0)) != std::string::npos) {
      header = buffer_.substr(0, last);
      headers_[header] = ft::trim_left(buffer_.substr(last + 1, end - last - 1), ' ');
    }
    buffer_.erase(0, end + 2);
  }
  return 0;
}

int Request::prebody() {
  body_offset_ = 0;

  if (headers_["Host"].empty())
    return 400;

  if (method_ != "POST" && method_ != "PUT") {
    status_ = COMPLETE;
    return 1;
  }

  if (!headers_["Transfer-Encoding"].empty() && headers_["Transfer-Encoding"] == "chunked") {
    status_ = CHUNK;
    chunk_status_ = CHUNK_SIZE;
  } else if (!headers_["Content-Length"].empty()) {
    if (headers_["Content-Length"].find_first_not_of("0123456789") != std::string::npos)
      return 400;
    try {
      length_ = std::stoi(headers_["Content-Length"]);
    }
    catch (std::exception &e) {
      return 501;
    }
    status_ = BODY;
  }
  else
    return 400;
  return 0;
}

int Request::chunk() {
  size_t end;

  while ((end = buffer_.find("\r\n")) != std::string::npos) {
    if (chunk_status_ == CHUNK_SIZE) {
      std::string hex = buffer_.substr(0, end);
      chunk_size_ = ft::to_hex(hex);
      buffer_.erase(0, end + 2);
      chunk_status_ = CHUNK_BODY;
    } else if (chunk_status_ == CHUNK_BODY) {
      if (chunk_size_ == 0) {
        status_ = COMPLETE;
        return 1;
      }
      req_body_ += buffer_.substr(0, end);
      buffer_.erase(0, end + 2);
      chunk_size_ = 0;
      chunk_status_ = CHUNK_SIZE;
    }
  }
  return 0;
}

int Request::body() {
  if (buffer_.length() + body_offset_ > length_) {
    status_ = ERROR;
    return 400;
  }

  req_body_.insert(body_offset_, buffer_, 0, length_);
  body_offset_ += buffer_.length();
  buffer_.clear();

  if (req_body_.length() == length_) {
    status_ = COMPLETE;
    return 1;
  }
  return 0;
}

void Request::print() {
  std::cout << "\n### REQUEST\n\n";
  std::cout << "Method: " << method_ << std::endl;
  std::cout << "Target: " << target_ << std::endl;
  std::cout << "Protocol: " << protocol_ << std::endl;
  for (std::map<std::string, std::string>::iterator it = headers_.begin(); it != headers_.end(); it++) {
    if (!it->second.empty())
      std::cout << it->first << ": " << it->second << std::endl;;
  }
  // std::cout << "Body: [" << req_body_ << "]" << std::endl;
  std::cout << "Status: " << status_ << std::endl;
  // std::cout << msg_ << std::endl;
  std::cout << "\n###" << std::endl;
}