#include "Response.hpp"

/*
** Constructors & Deconstructors
*/

Response::Response(RequestConfig &config) : config_(config) {
  headers_["Server"] = "Webserv/1.0";
  status_send_ = BUILD;
  total_sent_ = 0;
  initMethodMap();
}

Response::~Response() {}

std::map<std::string, Response::type> Response::methods_;
void Response::initMethodMap() {
  Response::methods_["GET"] = &Response::GET;
  Response::methods_["HEAD"] = &Response::GET;
  Response::methods_["POST"] = &Response::POST;
  Response::methods_["PUT"] = &Response::PUT;
  Response::methods_["DELETE"] = &Response::DELETE;
}

bool Response::isCGI(std::string extension) {
  std::map<std::string, std::string> &cgi = config_.getCGI();
  for (std::map<std::string, std::string>::iterator it = cgi.begin(); it != cgi.end(); it++) {
    if (it->first == extension)
      return true;
  }
  return false;
}

std::string Response::buildErrorPage(int status_code) {
  std::string body;
  body += "<!DOCTYPE html>\n\
            <html>\n\
            <head>\n\
              <title>Webserv: " + std::to_string(status_code) + " " + status_[status_code] + "</title>\n\
            </head>\n\
            <body>\n\
              <h1>" + std::to_string(status_code) + " " + status_[status_code] + "</h1>\n\
            </body>\n\
            </html>";
  headers_["Content-Type"] = MimeTypes::getType(".html");
  headers_["Content-Length"] = std::to_string(body.length());
  if (status_code == 401)
    headers_["WWW-Authenticate"] = "Basic realm=\"Access to restricted area\"";
  return body;
}

bool Response::checkAuth() {
  if (config_.getHeader("Authorization").empty())
    return false;
  std::string aut_cred = config_.getHeader("Authorization");
  std::string token = ft::b64decode(aut_cred.substr(aut_cred.find(' ') + 1));
  return (token == config_.getAuth());
}

std::string Response::methodList() {
  std::vector<std::string> methods = config_.getMethods();
  std::string list;

  std::vector<std::string>::iterator it = methods.begin();
  while (it != methods.end()) {
    list += *it;
    it++;
    if (it != methods.end())
      list += ", ";
  }
  return list;
}

void Response::build() {
  std::string method = config_.getMethod();

  std::map<std::string, std::string, ft::comp> head = config_.getHeaders();

  if (!config_.methodAccepted(method)) {
    status_code_ = 405;
    headers_["Allow"] = methodList();
  } else if (config_.getClientMaxBodySize() > 0 && config_.getBody().length() > config_.getClientMaxBodySize()) {
    status_code_ = 413;
  }
  else if (config_.getAuth() != "off" && !checkAuth())
    status_code_ = 401;
  else if (Response::methods_[method])
    status_code_ = (this->*(Response::methods_[method]))();

  if (status_code_ >= 400) {
    body_ = buildErrorPage(status_code_);
  }
  createResponse();
  status_send_ = SENDING;
}

void Response::createResponse() {
  if (config_.getMethod() == "HEAD")
    body_.clear();
  response_ = response_ + "HTTP/1.1" + " " + std::to_string(status_code_) + " " + status_[status_code_] + "\r\n";

  headers_["Date"] = ft::get_http_date();

  for (std::map<std::string, std::string>::iterator it = headers_.begin(); it != headers_.end(); it++)
    response_ += it->first + ": " + it->second + "\r\n";

  #ifdef DEBUG
  std::cout << "\n### RESPONSE\n\n" << response_ <<  "\n###\n" << std::endl;
  #endif

  response_ += "\r\n";

  if (!body_.empty())
    response_ += body_;
}

int Response::GET() {
  File file("." + config_.getRoot() + "/" + config_.getTarget());

  if (file.is_directory()) {
    std::string index = file.find_index(config_.getIndexes());
    if (index.length())
      file.set_path("." + config_.getRoot() + "/" + config_.getTarget() + "/" + index);
    else
      return 404;
  }

  if (!file.exists())
    return 404;

  if (!file.open())
    return 403;

  headers_["Last-Modified"] = file.last_modified();
  if (isCGI(file.getExtension())) {
    CGI cgi(file, config_, config_.getHeaders());

    cgi.execute();
    cgi.parseHeaders(headers_);
    body_ = cgi.getBody();
  }
  else {
    headers_["Content-Type"] = MimeTypes::getType(file.getExtension());
    body_ = file.getContent();
  }
  headers_["Content-Length"] = std::to_string(body_.length());
  return 200;
}

int Response::POST() {
  int status_code = 200;
  File file("." + config_.getRoot() + "/" + config_.getTarget());

  if (isCGI(file.getExtension())) {
    CGI cgi(file, config_, config_.getHeaders(), config_.getBody());

    cgi.execute();
    cgi.parseHeaders(headers_);
    body_ = cgi.getBody();
    headers_["Content-Length"] = std::to_string(body_.length());
  } else if (!file.exists()) {
    file.create(config_.getBody());
    headers_["Content-Length"] = "0";
    headers_["Content-Location"] = config_.getUri() + "/" + config_.getTarget();
    status_code = 201;
  }
  else {
    file.create(config_.getBody());
    headers_["Content-Location"] = config_.getUri() + "/" + config_.getTarget();
    status_code = 204;
  }
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
  headers_["Content-Location"] = config_.getUri() + "/" + config_.getTarget();
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
  int ret = ::send(fd, response_.c_str() + total_sent_, response_.length() - total_sent_, 0);
  total_sent_ += ret;
  if (total_sent_ >= response_.length())
    return 0;
  return 1;
}

int Response::getStatus() {
  return status_send_;
}
