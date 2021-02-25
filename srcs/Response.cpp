#include "Response.hpp"

/*
** Constructors & Deconstructors
*/

Response::Response(RequestConfig &config) : config_(config) {
  status_code_ = 200;
  initErrorCodes();
}

Response::~Response() {
}

std::map<int, std::string> Response::error_codes_;
void Response::initErrorCodes()
{
  Response::error_codes_[200] = "OK";
  Response::error_codes_[201] = "Created";
  Response::error_codes_[202] = "Accepted";
  Response::error_codes_[204] = "No Content";
  Response::error_codes_[400] = "Bad Request";
  Response::error_codes_[401] = "Unauthorized";
  Response::error_codes_[403] = "Forbidden";
  Response::error_codes_[404] = "Not Found";
  Response::error_codes_[405] = "Method Not Allowed";
  Response::error_codes_[411] = "Length Required";
  Response::error_codes_[413] = "Payload Too Large";
  Response::error_codes_[505] = "HTTP Version Not Supported";
}

void Response::build() {
  if (!config_.methodAccepted(config_.getMethod())) {
    std::cout << config_.getMethod() << " METHOD NOT ACCEPTED" << std::endl;
  }
  if (config_.getMethod() == "GET") {
    std::cout << "GET METHOD" << std::endl;
    handleGet();
  }
}

int Response::handleGet() {
  File file;
  std::string path = "." + config_.getRoot() + "/" + config_.getTarget();

  if (file.is_directory(path)) {
    std::string index = file.find_index("." + config_.getRoot() + config_.getTarget(), config_.getIndexes());
    if (index.length()) {
      file.open("." + config_.getRoot() + config_.getTarget() + "/" + file.find_index("." + config_.getRoot() + config_.getTarget(), config_.getIndexes()));
    }
  }
  else {
    file.open(path);
  }

  if (file.is_open())
    status_code_ = 200;
  else
    status_code_ = 404;

  std::string content_type = MimeTypes::getType(file.getExtension());
  response_ = response_ + "HTTP/1.1" + " " + std::to_string(status_code_) + " " + error_codes_[status_code_] + "\n";

  std::string body;

  if (status_code_ >= 400) {
    body = std::to_string(status_code_) + " " + error_codes_[status_code_] + ". Franksmon is dead :(";
    content_type = "text/html";
  } else if (status_code_ < 300) {
    body = file.getContent();
  }

  if (!body.empty()) {
    response_ = response_ + "Content-Type: " + content_type + "\r\n";
    response_ = response_ + "Content-Length: " + std::to_string(body.length()) + "\r\n";
    response_ = response_ + "\r\n";
    response_ = response_ + body;
  }
  return 1;
}

std::string Response::getResponseBody() {
  // std::string response;
  // File file;

  // file.open("." + config_.getRoot() + "/" + config_.getTarget());

  // if (file.is_directory())
  //   file.open("." + config_.getRoot() + config_.getTarget() + "/" + file.find_index("." + config_.getRoot() + config_.getTarget(), config_.getIndexes()));

  // if (file.is_open())
  //   status_code_ = 200;
  // else
  //   status_code_ = 404;

  // response = response + "HTTP 1.1" + " " + std::to_string(status_code_) + " " + error_codes_[status_code_] + "\n";

  // if (status_code_ >= 400) {
  //   body_ = std::to_string(status_code_) + " " + error_codes_[status_code_] + ". Franksmon is dead :(";
  // } else if (status_code_ < 300) {
  //   body_ = file.getContent();
  // }

  // if (!body_.empty()) {
  //   response = response + "Content-Type: " + MimeTypes::getType(file.getExtension()) + "\r\n";
  //   response = response + "Content-Length: " + std::to_string(body_.length()) + "\r\n";
  //   response = response + "\r\n";
  //   response = response + body_;
  // }

  // return response;
  return "";
}

int Response::send(int fd) {
  write(fd, response_.c_str(), response_.length());
  return 1;
}