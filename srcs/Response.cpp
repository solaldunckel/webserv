#include "Response.hpp"

/*
** Constructors & Deconstructors
*/

Response::Response(RequestConfig &config, int error_code) : config_(config) {
  headers_["Server"] = "webserv/1.0";
  error_code_ = error_code;
  total_sent_ = 0;
  initMethodMap();
  build();
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

  if (!config_.getErrorPages()[status_code].empty()) {
    File file(config_.getRoot() + config_.getErrorPages()[status_code]);

    if (file.open())
      body += file.getContent();
  } else {
    body += "<html>\r\n";
    body += "<head><title>" + std::to_string(status_code) + " " + status_[status_code] + "</title></head>\r\n";
    body += "<body>\r\n";
    body += "<center><h1>" + std::to_string(status_code) + " " + status_[status_code] + "</h1></center>\r\n";
    body += "<hr><center>" + headers_["Server"] + "</center>\r\n";
    body += "</body>\r\n";
    body += "</html>\r\n";
    headers_["Content-Type"] = MimeTypes::getType(".html");
  }
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

  if (error_code_ > 1)
    status_code_ = error_code_;
  else if (!config_.methodAccepted(method)) {
    status_code_ = 405;
    headers_["Allow"] = methodList();
  }
  else if (config_.getClientMaxBodySize() > 0 && config_.getBody().length() > config_.getClientMaxBodySize()) {
    status_code_ = 413;
  }
  else if (config_.getAuth() != "off" && !checkAuth())
    status_code_ = 401;
  else if (Response::methods_[method])
    status_code_ = (this->*(Response::methods_[method]))();

  if (status_code_ >= 300) {
    body_ = buildErrorPage(status_code_);
  }
  createResponse();
}

void Response::createResponse() {
  if (config_.getMethod() == "HEAD")
    body_.clear();
  response_ = response_ + "HTTP/1.1" + " " + std::to_string(status_code_) + " " + status_[status_code_] + "\r\n";

  headers_["Date"] = ft::get_http_date();

  for (std::map<std::string, std::string>::iterator it = headers_.begin(); it != headers_.end(); it++)
    response_ += it->first + ": " + it->second + "\r\n";

  // #ifdef DEBUG
  std::cout << "\n-> RESPONSE <-\n" << response_ << std::endl;
  // #endif

  response_ += "\r\n";

  if (!body_.empty())
    response_ += body_;
}

int Response::GET() {
  File file(config_.getRoot() + "/" + config_.getTarget());

  if (file.is_directory()) {
    if (config_.getTarget().find_last_of("/") != config_.getTarget().length() - 1) {
      headers_["Location"] = "http://" + config_.getHeader("Host") + config_.getUri() + "/" + config_.getTarget() + "/";
      std::cout << "LOC " << headers_["Location"] << std::endl;
      return 301;
    }
    std::string index = file.find_index(config_.getIndexes());
    if (index.length())
      file.set_path(config_.getRoot() + "/" + config_.getTarget() + "/" + index);
    else if (!config_.getAutoindex())
      return 404;
  }

  if (!file.is_directory()) {
    if (!file.exists())
      return 404;

    if (!file.open())
      return 403;

    headers_["Last-Modified"] = file.last_modified();
  }

  if (isCGI(file.getExtension())) {
    CGI cgi(file, config_, config_.getHeaders());

    cgi.execute();
    cgi.parseHeaders(headers_);
    body_ = cgi.getBody();
  } else if (config_.getAutoindex() && file.is_directory()) {
    headers_["Content-Type"] = MimeTypes::getType(".html");
    body_ = file.autoIndex(config_.getTarget());
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
  File file(config_.getRoot() + "/" + config_.getTarget());

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
  std::string path = config_.getRoot() + "/" + config_.getTarget();

  if (!config_.getUpload().empty()) {
    File dir(config_.getRoot() + "/" + config_.getUpload());

    if (dir.exists() && !dir.is_directory()) {
      dir.unlink();
    }

    if (!dir.exists()) {
      if (mkdir(dir.getPath().c_str(), 0755))
        perror("/tmp/blah");
    }
    path = config_.getRoot() + "/" + config_.getUpload() + "/" + config_.getTarget();
  }

  File file(path);

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
  File file(config_.getRoot() + "/" + config_.getTarget());

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
