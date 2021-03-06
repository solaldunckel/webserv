#include "Response.hpp"

/*
** Constructors & Deconstructors
*/

Response::Response(RequestConfig &config) : config_(config) {
  status_code_ = 200;
  headers_["Server"] = "webserv/1.0";
  initErrorCodes();
  initMethodMap();
}

Response::~Response() {
}

std::map<std::string, Response::type> Response::methods_;
void Response::initMethodMap() {
  Response::methods_["GET"] = &Response::handleGet;
  Response::methods_["HEAD"] = &Response::handleGet;
  Response::methods_["POST"] = &Response::handleGet;
  Response::methods_["PUT"] = &Response::handlePut;
  Response::methods_["DELETE"] = &Response::handleDelete;
}

std::map<int, std::string> Response::error_codes_;
void Response::initErrorCodes() {
  // Successful 2xx
  Response::error_codes_[200] = "OK";
  Response::error_codes_[201] = "Created";
  Response::error_codes_[202] = "Accepted";
  Response::error_codes_[204] = "No Content";

  // Redirection 3xx


  // Client Error 4xx
  Response::error_codes_[400] = "Bad Request";
  Response::error_codes_[401] = "Unauthorized";
  Response::error_codes_[403] = "Forbidden";
  Response::error_codes_[404] = "Not Found";
  Response::error_codes_[405] = "Method Not Allowed";
  Response::error_codes_[408] = "Request Timeout";
  Response::error_codes_[411] = "Length Required";
  Response::error_codes_[413] = "Payload Too Large";
  Response::error_codes_[414] = "URI Too Long";

  // Server Error 5xx
  Response::error_codes_[500] = "Internal Server Error";
  Response::error_codes_[501] = "Not Implemented";
  Response::error_codes_[505] = "HTTP Version Not Supported";
}

bool Response::isCGI(std::string &path, std::string extension) {
  std::cout << "IS CGI ?" << std::endl;
  std::cout << "PATH = [" << path << "]" << std::endl;
  std::cout << "EXTENSION = [" << extension << "]" << std::endl;
  std::map<std::string, std::string> &cgi = config_.getCGI();
  for (std::map<std::string, std::string>::iterator it = cgi.begin(); it != cgi.end(); it++) {
    std::cout << it->first << " / " << it->second << std::endl;
    if (it->first == extension)
      return true;
  }
  return false;
}

void Response::buildErrorPage(int status_code) {
  body_ += "<!DOCTYPE html>\n\
            <html>\n\
            <head>\n\
              <title>Webserv: " + std::to_string(status_code) + " " + error_codes_[status_code] + "</title>\n\
            </head>\n\
            <body>\n\
              <h1>" + std::to_string(status_code) + " " + error_codes_[status_code] + "</h1>\n\
            </body>\n\
            </html>";
  headers_["Content-Type"] = MimeTypes::getType(".html");
  headers_["Content-Length"] = std::to_string(body_.length());
  if (status_code == 401)
    headers_["WWW-Authenticate"] = "Basic realm=\"Access to restricted area\"";
}

bool Response::checkAuth() {
  if (config_.getHeader("Authorization").empty())
    return false;
  std::string aut_cred = config_.getHeader("Authorization");
  std::string token = ft::b64decode(aut_cred.substr(aut_cred.find(' ') + 1));
  return (token == config_.getAuth());
}

void Response::build() {
  int status_code = 200;
  std::string method = config_.getMethod();

  if (!config_.methodAccepted(method)) {
    status_code = 405;
    std::cout << "METHOD NOT ACCEPTED" << std::endl;
  } else if (config_.getClientMaxBodySize() > 0 && config_.getBody().length() > config_.getClientMaxBodySize()) {
    status_code = 413;
  }
  else if (config_.getAuth() != "off" && !checkAuth()) {
    status_code = 401;
  }
  else if (Response::methods_[method]) {
    std::cout << "HANDLING " << method << " REQUEST" << std::endl;
    status_code = (this->*(Response::methods_[method]))();
  }

  if (status_code >= 400) {
    std::cout << "HANDLING ERROR " << status_code_ << std::endl;
    buildErrorPage(status_code);
  }

  response_ = response_ + "HTTP/1.1" + " " + std::to_string(status_code) + " " + error_codes_[status_code] + "\r\n";

  for (std::map<std::string, std::string>::iterator it = headers_.begin(); it != headers_.end(); it++)
    response_ += it->first + ": " + it->second + "\r\n";

  std::cout << "\n### RESPONSE\n\n" << response_ <<  "\n###\n" << std::endl;

  response_ += "\r\n";

  if (!body_.empty())
    response_ = response_ + body_;
}

int Response::handleGet() {
  File file;
  std::string path = "." + config_.getRoot() + "/" + config_.getTarget();

  if (file.is_directory(path)) {
    std::string index = file.find_index("." + config_.getRoot() + config_.getTarget(), config_.getIndexes());
    if (index.length()) {
      path = "." + config_.getRoot() + config_.getTarget() + "/" + index;
      // file.open("." + config_.getRoot() + config_.getTarget() + "/" + file.find_index("." + config_.getRoot() + config_.getTarget(), config_.getIndexes()));
    }
  }

  if (!file.exists(path))
    return 404;

  file.open(path);

  if (!file.is_open())
    return 403;

  if (isCGI(path, file.getExtension())) {
    std::cout << "MATCHING CGI" << std::endl;
  }
  body_ = file.getContent();

  headers_["Content-Type"] = MimeTypes::getType(file.getExtension());
  headers_["Content-Length"] = std::to_string(body_.length());

  if (config_.getMethod() == "HEAD")
    body_.clear();

  return 200;
}

int Response::handlePut() {
  int status_code = 204;
  File file;
  std::string path = "." + config_.getRoot() + "/" + config_.getTarget();

  if (!file.exists(path)) {
    file.create(path, config_.getBody());
    headers_["Content-Length"] = "0";
    status_code = 201;
  }
  else {
    file.create(path, config_.getBody());
  }
  headers_["Content-Location"] = config_.getUri() + config_.getTarget();
  return status_code;
}

int Response::handleDelete() {
  int status_code = 200;
  File file;
  std::string path = "." + config_.getRoot() + "/" + config_.getTarget();

  if (!file.exists(path))
    return 404;

  unlink(path.c_str());

  body_ += "<!DOCTYPE html>\n\
            <html>\n\
            <body>\n\
              <h1>File deleted</h1>\n\
            </body>\n\
            </html>";
  headers_["Content-Type"] = MimeTypes::getType(".html");
  headers_["Content-Length"] = std::to_string(body_.length());
  return status_code;
}

int Response::send(int fd) {
  write(fd, response_.c_str(), response_.length());
  return 1;
}
