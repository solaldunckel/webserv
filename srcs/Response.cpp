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

std::string Response::getResponseBody() {
  std::string response;
  File file;

  std::cout << "\n###\n" << std::endl;

  std::cout << "ROOT: " << config_.getRoot() << std::endl;
  std::cout << "PATH: " << config_.getRoot() + config_.getTarget() << std::endl;
  // request_.removeUriFromTarget();
  file.open("." + config_.getRoot() + "/" + config_.getTarget());

  return "";
  if (file.is_directory()) {
    file.open("." + config_.getRoot() + config_.getTarget() + "/" + file.find_index("." + config_.getRoot() + config_.getTarget(), config_.getIndexes()));
  }

  std::cout << "\n###\n" << std::endl;

  if (file.is_open())
    status_code_ = 200;
  else
    status_code_ = 404;

  response = response + config_.getRequest().getProtocol() + " " + std::to_string(status_code_) + " " + error_codes_[status_code_] + "\n";

  if (status_code_ >= 400) {
    body_ = std::to_string(status_code_) + " " + error_codes_[status_code_] + ". Franksmon is dead :(";
  } else if (status_code_ < 300) {
    body_ = file.getContent();
  }

  if (!body_.empty()) {
    response = response + "Content-Type: " + MimeTypes::getType(file.getExtension()) + "\r\n";
    response = response + "Content-Length: " + std::to_string(body_.length()) + "\r\n";
    response = response + "\r\n";
    response = response + body_;
  }

  return response;
}

int Response::send(int fd) {
  std::string response_msg = getResponseBody();
  write(fd, response_msg.c_str(), response_msg.length());
  return 1;
}