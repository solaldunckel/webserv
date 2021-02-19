#include "Response.hpp"

/*
** Constructors & Deconstructors
*/

Response::Response(Request &request) : request_(request) {
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

  file.open("./data" + request_.target_);

  if (file.is_directory()) {
    file.open("./data" + request_.target_ + "index.html");
  }

  if (file.is_open())
    status_code_ = 200;
  else
    status_code_ = 404;

  response = response + request_.protocol_ + " " + std::to_string(status_code_) + " " + error_codes_[status_code_] + "\n";

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