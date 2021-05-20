#include "Response.hpp"

/*
** Constructors & Deconstructors
*/

Response::Response(RequestConfig &config, int worker_id, int error_code) : config_(config), worker_id_(worker_id) {
  headers_["Server"] = "webserv/1.0";
  error_code_ = error_code;
  status_code_ = 0;
  total_sent_ = 0;
  header_size_ = 0;
  body_size_ = 0;
  redirect_code_ = 0;
  redirect_ = false;
  initMethodMap();
}

Response::~Response() {}

void Response::clear() {
  error_code_ = 0;
  status_code_ = 0;
  total_sent_ = 0;
  header_size_ = 0;
  body_size_ = 0;
  redirect_ = false;
  response_.clear();
  body_.clear();
  headers_.clear();
  headers_["Server"] = "webserv/1.0";
}

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

bool Response::checkAuth() {
  if (config_.getHeader("Authorization").empty())
    return false;
  std::string auth_cred = config_.getHeader("Authorization");
  std::string token = ft::b64decode(auth_cred.substr(auth_cred.find(' ') + 1));
  return (token == config_.getAuth());
}

int Response::buildErrorPage(int status_code) {
  if (!config_.getErrorPages()[status_code].empty()) {
    std::string target = config_.getErrorPages()[status_code];

    config_.getMethod() = "GET";

    redirect_ = true;
    redirect_code_ = status_code;
    redirect_target_ = target;

    return 0;
  } else {
    body_ += "<html>\r\n";
    body_ += "<head><title>" + ft::to_string(status_code) + " " + status_[status_code] + "</title></head>\r\n";
    body_ += "<body>\r\n";
    body_ += "<center><h1>" + ft::to_string(status_code) + " " + status_[status_code] + "</h1></center>\r\n";
    body_ += "<hr><center>" + headers_["Server"] + "</center>\r\n";
    body_ += "</body>\r\n";
    body_ += "</html>\r\n";
    headers_["Content-Type"] = MimeTypes::getType(".html");
  }
  headers_["Content-Length"] = ft::to_string(body_.length());
  if (status_code == 401)
    headers_["WWW-Authenticate"] = "Basic realm=\"Access to restricted area\"";
  if (status_code == 408 || status_code == 503)
    headers_["Connection"] = "close";
  return status_code;
}

std::string Response::buildMethodList() {
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
  std::string &method = config_.getMethod();

  std::cout << "BUILD ! method : " << method << " / target : " << config_.getTarget() << std::endl;

  file_.set_path(config_.getRoot() + "/" + config_.getTarget());

  if (error_code_ > 1)
    status_code_ = error_code_;
  else if (!config_.methodAccepted(method)) {
    status_code_ = 405;
    headers_["Allow"] = buildMethodList();
  }
  else if (config_.getClientMaxBodySize() > 0 && config_.getBody().length() > config_.getClientMaxBodySize()) {
    status_code_ = 413;
  }
  else if (config_.getAuth() != "off" && !checkAuth())
    status_code_ = 401;
  else
    status_code_ = handleMethods();

  if (status_code_ >= 300 && !body_.length())
    status_code_ = buildErrorPage(status_code_);

  if (!redirect_)
    createResponse();
}

int Response::handleMethods() {
  std::string &method = config_.getMethod();
  std::string path;

  if (method == "GET" || method == "HEAD") {
    if (file_.is_directory()) {
      std::string index = file_.find_index(config_.getIndexes());
      if (index.length()) {
        redirect_ = true;
        redirect_target_ = ft::unique_char("/" + config_.getRequestTarget() + "/" + index);
        return 200;
      }
      else if (!config_.getAutoindex())
        return 404;
    }

    path = file_.getPath();
    if (!file_.is_directory()) {
      if (!file_.exists())
        return 404;
      file_.parse_match();
      std::vector<std::string> &matches = file_.getMatches();

      if (!config_.getHeader("Accept-Language").empty()) {
        if (localization(matches)) {
          if (matches.size() > 1)
            return 300;
          else
            file_.set_path(path.substr(0, path.find_last_of("/") + 1) + matches.front());
        }
      }

      if (!config_.getHeader("Accept-Charset").empty()) {
        charset_ = accept_charset(matches);
        if (matches.size() > 1)
          return 300;
        else
          file_.set_path(path.substr(0, path.find_last_of("/") + 1) + matches.front());
      }

      if (!file_.open())
        return 403;

      headers_["Last-Modified"] = file_.last_modified();
    }
  }

  if (isCGI(file_.getMimeExtension())) {
    CGI cgi(file_, config_, config_.getHeaders(), config_.getBody());
    cgi.init(worker_id_);
    if ((status_code_ = cgi.execute()) > 400)
      return status_code_;
    cgi.parseHeaders(headers_);
    body_ = cgi.getBody();
    headers_["Content-Length"] = ft::to_string(body_.length());
  }
  else
    status_code_ = (this->*(Response::methods_[method]))();
  return status_code_;
}

void Response::createResponse() {
  if (config_.getMethod() == "HEAD")
    body_.clear();

  if (status_code_ < 400 && redirect_code_)
    status_code_ = redirect_code_;

  std::string status_code;

  if (headers_.count("Status")) {
    status_code = headers_["Status"];
    headers_.erase("Status");
  }
  else
    status_code = ft::to_string(status_code_) + " " + status_[status_code_];

  response_ = response_ + config_.getProtocol() + " " + status_code + "\r\n";

  headers_["Date"] = ft::get_http_date();

  for (std::map<std::string, std::string>::iterator it = headers_.begin(); it != headers_.end(); it++)
    response_ += it->first + ": " + it->second + "\r\n";

  response_ += "\r\n";

  header_size_ = response_.size();

  if (!body_.empty()) {
    body_size_ = body_.size();
    response_ += body_;
    body_.clear();
  }
}

/* METHODS */

int Response::GET() {
  if (config_.getAutoindex() && file_.is_directory()) {
    headers_["Content-Type"] = MimeTypes::getType(".html");
    body_ = file_.autoIndex(config_.getRequestTarget());
    headers_["Content-Length"] = ft::to_string(body_.length());
  }
  else {
    headers_["Content-Type"] = MimeTypes::getType(file_.getMimeExtension());
    body_ = file_.getContent();
    headers_["Content-Length"] = ft::to_string(body_.length());
  }
  return 200;
}

int Response::POST() {
  int status_code = 200;

  std::string path = config_.getUri() + "/" + config_.getUpload() + "/" + config_.getTarget();

  if (!config_.getUpload().empty())
    file_.set_path(config_.getUpload() + "/" + config_.getTarget());

  body_ = config_.getBody();

  if (!file_.exists()) {
    file_.create(body_);
    status_code = 201;
    headers_["Location"] = ft::unique_char(path);
  }
  else {
    file_.append(body_);
    status_code = 200;
    headers_["Content-Location"] = ft::unique_char(path);
  }

  headers_["Content-Length"] = ft::to_string(body_.length());
  return status_code;
}

int Response::PUT() {
  int status_code = 204;

  if (!config_.getUpload().empty())
    file_.set_path(config_.getUpload() + "/" + config_.getTarget());

  if (!file_.exists()) {
    file_.create(config_.getBody());
    status_code = 201;
  }
  else
    file_.create(config_.getBody());
  headers_["Location"] = ft::unique_char(config_.getUri() + "/" + config_.getUpload() + "/" + config_.getTarget());
  return status_code;
}

int Response::DELETE() {
  if (!file_.exists())
    return 404;

  file_.unlink();

  body_ += "<!DOCTYPE html>\n\
            <html>\n\
            <body>\n\
              <h1>File deleted</h1>\n\
            </body>\n\
            </html>";
  headers_["Content-Type"] = MimeTypes::getType(".html");
  headers_["Content-Length"] = ft::to_string(body_.length());
  return 200;
}

/* Content Negociation */

bool Response::localization(std::vector<std::string> &matches){
  std::string path = file_.getPath();
  std::string all = config_.getHeader("Accept-Language");
  int q = 10;
  int max = 0;
  std::vector<std::string> new_matches;
  std::vector<std::string> select_matches;

  headers_["Content-Language"] = "fr";
  while (1) {
    std::string str = all.substr(0, all.find_first_of(" ,;\0"));
    new_matches.clear();
    if (str.find("*") == std::string::npos){
      for (std::vector<std::string>::iterator it = matches.begin() ; it != matches.end() ; it++)
        if (it->find("." + str) != std::string::npos)
          new_matches.push_back(*it);
    }
    else
      new_matches = matches;
    if (!new_matches.empty() && (q > max)) {
      select_matches = new_matches;
      if (str[0] != '*')
        headers_["Content-Language"] = str;
      max = q;
    }
    if (all.find(".") != std::string::npos)
      q = ft::stoi(all.substr(all.find_first_of(".") + 1, 1));
    if (all.find(",") == std::string::npos){
      if (!select_matches.empty()){
        matches = select_matches;
        return (1);
      }
      return (0);
    }
    all = all.substr(all.find_first_of(" ,;"));
    all = all.substr(all.find_first_of("abcdefghijklmnoprstuvwxyz*"));
  }
}

std::string Response::accept_charset(std::vector<std::string> &matches){
  std::string path = file_.getPath();
  std::string all = config_.getHeader("Accept-Charset");
  int q = 10;
  int max = 0;
  std::string ret;
  std::vector<std::string> new_matches;
  std::vector<std::string> select_matches;

  ret = "";
  while (1) {
    std::string str = all.substr(0, all.find_first_of(" ,;\0"));

    if (str.find("*") == std::string::npos){
      for(std::vector<std::string>::iterator it = matches.begin() ; it != matches.end() ; it++)
        if (it->find("." + str) != std::string::npos)
          new_matches.push_back(*it);
    }
    else
      new_matches = matches;;
    if (!new_matches.empty() && (q > max)) {
        select_matches = new_matches;
      if(str[0] != '*')
         ret = str;
      max = q;
    }
    if (all.find(".") != std::string::npos)
      q = ft::stoi(all.substr(all.find_first_of(".") + 1, 1));
    if (all.find(",") == std::string::npos){
        if (!select_matches.empty())
          matches = select_matches;
      break ;
    }
    all = all.substr(all.find_first_of(" ,;"));
    all = all.substr(all.find_first_of("abcdefghijklmnoprstuvwxyz*"));
  }
  return (ret);
}

bool Response::shouldDisconnect() {
  if (headers_.find("Connection") != headers_.end() && headers_["Connection"] == "close")
    return true;
  return false;
}

bool Response::redirect() {
  return redirect_;
}

std::string Response::redirect_target() {
  return redirect_target_;
}

/* Send */

int Response::send(int fd) {
  int ret = ::send(fd, response_.c_str() + total_sent_, response_.length() - total_sent_, 0);

  if (ret < 0) {
    std::cout << "send : " << strerror(errno) << std::endl;
    return -1;
  }
  total_sent_ += ret;
  if (total_sent_ >= response_.length())
    return 0;
  return 1;
}

void Response::print() {
  std::cout << "\n-> RESPONSE <-\n" << response_.substr(0, header_size_ - 2) << std::endl;
  if (body_size_) {
    if (body_size_ < 200)
      std::cout << response_.substr(header_size_) << std::endl;
    else
      std::cout << response_.substr(header_size_, 200) + "..." << std::endl;
  }
}
