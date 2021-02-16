#include "Response.hpp"

/*
** Constructors & Deconstructors
*/

Response::Response(Request &request) : request_(request) {
  status_code_ = 200;
  initErrorCodes();
  initMimeTypes();
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

std::map<std::string, std::string> Response::mime_types_;
void Response::initMimeTypes()
{
  Response::mime_types_[".css"] = "text/css";
  Response::mime_types_[".jpg"] = "image/jpeg";
  Response::mime_types_[".jpeg"] = "image/jpeg";
  Response::mime_types_[".html"] = "text/html";
  Response::mime_types_[".htm"] = "text/html";
  Response::mime_types_[".png"] = "image/png";
}

std::string Response::getResponseBody() {
  std::string response;
  struct FileHandler file;

  if (request_.method_ == "GET") {
    status_code_ = openFile(file);
  }

  response = response + request_.protocol_ + " " + std::to_string(status_code_) + " " + error_codes_[status_code_] + "\n";

  if (status_code_ >= 400) {
    body_ = std::to_string(status_code_) + " " + error_codes_[status_code_] + ". Franksmon is dead :(";
  } else if (status_code_ < 300) {
    body_ = getFileContent(file);
  }

  if (!body_.empty()) {
    response = response + "Content-Type: " + getMimeType(file) + "\r\n";
    response = response + "Content-Length: " + std::to_string(body_.length()) + "\r\n";
    response = response + "\r\n";
    response = response + body_;
  }

  return response;
}

std::string Response::getMimeType(FileHandler &file) {
  std::string extension = file.name_.substr(file.name_.find_last_of('.'));

  if (mime_types_.count(extension))
    return mime_types_[extension];
  return std::string("text/plain");
}

int Response::openFile(FileHandler &file) {
  std::string root = "./data";

  file.name_ = root + request_.target_;
  file.fd_ = open(file.name_.c_str(), O_DIRECTORY);

	if (file.fd_ == -1)
		file.is_dir_ = false;
	else
		file.is_dir_ = true;

  if (file.is_dir_)
    file.name_ = root + "/" + "index.html";

  // Must close!
  file.stream_.open(file.name_, std::ios::binary | std::ios::in);

  if (!file.stream_.is_open() || !file.stream_.good()) {
    return 404;
  }
  return 200;
}

std::string Response::getFileContent(FileHandler &file) {
  char buf[512 + 1];
  std::string final;
  while (!file.stream_.eof()) {
    memset(buf, 0, 512);
    file.stream_.read(buf, 512);
    final += std::string(buf, file.stream_.gcount());
  }
  return final;
}