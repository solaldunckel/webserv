#include "Response.hpp"

/*
** Constructors & Deconstructors
*/

Response::Response(RequestConfig &config) : config_(config) {
  headers_["Server"] = "Webserv/1.0";
  initMethodMap();
}

Response::~Response() {}

std::map<std::string, Response::type> Response::methods_;
void Response::initMethodMap() {
  Response::methods_["GET"] = &Response::GET;
  Response::methods_["HEAD"] = &Response::HEAD;
  Response::methods_["POST"] = &Response::GET;
  Response::methods_["PUT"] = &Response::PUT;
  Response::methods_["DELETE"] = &Response::DELETE;
}

bool Response::isCGI(std::string extension) {
  #ifdef DEBUG
  std::cout << "IS CGI ?" << std::endl;
  std::cout << "EXTENSION = [" << extension << "]" << std::endl;
  #endif
  std::map<std::string, std::string> &cgi = config_.getCGI();
  for (std::map<std::string, std::string>::iterator it = cgi.begin(); it != cgi.end(); it++) {
    #ifdef DEBUG
    std::cout << it->first << " / " << it->second << std::endl;
    #endif
    if (it->first == extension)
      return true;
  }
  return false;
}

void Response::buildErrorPage(int status_code) {
  body_ += "<!DOCTYPE html>\n\
            <html>\n\
            <head>\n\
              <title>Webserv: " + std::to_string(status_code) + " " + status_[status_code] + "</title>\n\
            </head>\n\
            <body>\n\
              <h1>" + std::to_string(status_code) + " " + status_[status_code] + "</h1>\n\
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
    #ifdef DEBUG
    std::cout << "METHOD NOT ACCEPTED" << std::endl;
    #endif
  } else if (config_.getClientMaxBodySize() > 0 && config_.getBody().length() > config_.getClientMaxBodySize()) {
    status_code = 413;
  }
  else if (config_.getAuth() != "off" && !checkAuth()) {
    status_code = 401;
  }
  else if (Response::methods_[method]) {
    #ifdef DEBUG
    std::cout << "HANDLING " << method << " REQUEST" << std::endl;
    #endif
    status_code = (this->*(Response::methods_[method]))();
  }

  if (status_code >= 400) {
    #ifdef DEBUG
    std::cout << "HANDLING ERROR " << status_code << std::endl;
    #endif
    buildErrorPage(status_code);
  }

  response_ = response_ + "HTTP/1.1" + " " + std::to_string(status_code) + " " + status_[status_code] + "\r\n";

  headers_["Date"] = ft::get_http_date();
  for (std::map<std::string, std::string>::iterator it = headers_.begin(); it != headers_.end(); it++)
    response_ += it->first + ": " + it->second + "\r\n";

  #ifdef DEBUG
  std::cout << "\n### RESPONSE\n\n" << response_ <<  "\n###\n" << std::endl;
  #endif

  response_ += "\r\n";

  if (!body_.empty())
    response_ = response_ + body_;
}

int Response::GET() {
  File file("." + config_.getRoot() + "/" + config_.getTarget());

  if (file.is_directory()) {
    std::string index = file.find_index(config_.getIndexes());
    if (index.length())
      file.set_path("." + config_.getRoot() + config_.getTarget() + "/" + index);
    else
      return 404;
  }

  if (!file.exists())
    return 404;

  file.open();

  if (!file.is_open())
    return 403;

  if (isCGI(file.getExtension())) {
    #ifdef DEBUG
    std::cout << "MATCHING CGI" << std::endl;
    #endif
    CGI cgi(file, config_);

    cgi.execute();
    std::cout << "GETTING BODY" << std::endl;
    body_ = cgi.getBody();
    std::cout << " [" << body_ << "] " << std::endl;
    headers_["Content-Type"] = "text/html";
  }
  else {
    headers_["Content-Type"] = MimeTypes::getType(file.getExtension());
    body_ = file.getContent();
  }

  headers_["Content-Length"] = std::to_string(body_.length());

  if (config_.getMethod() == "HEAD")
    body_.clear();

  headers_["Last-Modified"] = file.last_modified();
  return 200;
}

int Response::HEAD() {
  int status_code = 200;

  status_code = GET();

  body_.clear();

  return status_code;
}

int Response::PUT() {
  int status_code = 204;
  File file("." + config_.getRoot() + "/" + config_.getTarget());

  if (!file.exists()) {
    file.create(config_.getBody());
    headers_["Content-Length"] = "0";
    status_code = 201;
  }
  else {
    file.create(config_.getBody());
  }
  headers_["Content-Location"] = config_.getUri() + config_.getTarget();
  return status_code;
}

int Response::DELETE() {
  File file("." + config_.getRoot() + "/" + config_.getTarget());

  if (!file.exists())
    return 404;

  file.unlink();

  body_ += "<!DOCTYPE html>\n\
            <html>\n\
            <body>\n\
              <h1>File deleted</h1>\n\
            </body>\n\
            </html>";
  headers_["Content-Type"] = MimeTypes::getType(".html");
  headers_["Content-Length"] = std::to_string(body_.length());
  return 200;
}

int Response::send(int fd) {
  write(fd, response_.c_str(), response_.length());
  return 1;
}
