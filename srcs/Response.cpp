#include "Response.hpp"

/*
** Constructors & Deconstructors
*/

Response::Response(Request &request) : request_(request) {
  status_code_ = 200;
  initErrorCodes();
}

// Response::Response(Request &request, Config &config) : request_(request), config_(config) {
// }

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
  std::string str;

  // std::cout << "test" << std::endl;
  if (request_.method_ == "GET") {
    status_code_ = openFile();
  }
  if (status_code_ >= 400) {
    body_ = std::to_string(status_code_) + " " + error_codes_[status_code_];
  }
  str = str + request_.protocol_ + " " + std::to_string(status_code_) + " " + error_codes_[status_code_] + "\n";
  str = str + "Content-Type: " + "text/plain" + "\n";
  str = str + "Content-Length: " + std::to_string(body_.length());
  if (!body_.empty()) {
    str = str + "\r\n\r\n";
    str = str + body_;
  }
  return str;
}

int Response::openFile() {
  std::string root = "./data/";
  std::string file_name = root + request_.target_;
  bool isdir;
  int fd = open(file_name.c_str(), O_DIRECTORY);

	if (fd == -1)
		isdir = false;
	else
		isdir = true;

  // std::cout << file_name << " = " << isdir << std::endl;

  if (isdir)
    file_name = root + "index.hml";

  std::ifstream file(file_name);

  if (!file.is_open() || !file.good()) {
    return 404;
  }

  std::string line;

  while (std::getline(file, line))
    body_ += line;
  return 200;
}