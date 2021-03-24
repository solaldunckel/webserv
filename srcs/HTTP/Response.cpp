#include "Response.hpp"

/*
** Constructors & Deconstructors
*/

Response::Response(RequestConfig &config, int error_code) : config_(config) {
  headers_["Server"] = "webserv/1.0";
  error_code_ = error_code;
  status_code_ = 0;
  total_sent_ = 0;
  redirect_ = 1;
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

void Response::localization(){
  std::string path = file_.getPath();
  std::string all = config_.getHeader("Accept-Language");
  int q = 10;
  int max = 0;
  std::string tmp;

  std::cout << path << std::endl;
  while (1) {
    std::string str = all.substr(0, all.find_first_of(" ,;-\0"));

    if (str.find("*") == std::string::npos)
      tmp = path.substr(0, path.find_last_of('.')) + "." + str + path.substr(path.find_last_of('.'));
    else
      tmp = path;
    if (file_.exist(tmp) && (q > max)) {
      file_.set_path(tmp);
      headers_["Language-Content"] = str;
      max = q;
    }
    q = ft::stoi(all.substr(all.find_first_of(".") + 1, 1));
    if (all.find(",") == std::string::npos)
      break ;
    all = all.substr(all.find_first_of(" ,;-"));
    all = all.substr(all.find_first_of("abcdefghijklmnoprstuvwxyz*"));
  }
}

bool Response::isCGI(std::string extension) {
  std::map<std::string, std::string> &cgi = config_.getCGI();
  for (std::map<std::string, std::string>::iterator it = cgi.begin(); it != cgi.end(); it++) {
    if (it->first == extension)
      return true;
  }
  return false;
}

int Response::buildErrorPage(int status_code) {
  if (!config_.getErrorPages()[status_code].empty()) {
    std::string path = config_.getErrorPages()[status_code];

    file_.set_path(config_.getRoot() + path);

    if (file_.open())
      body_ += file_.getContent();
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
  if (status_code == 408)
    headers_["Connection"] = "close";
  return status_code;
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
  std::string &method = config_.getMethod();

  file_.set_path(config_.getRoot() + config_.getTarget());

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

  if (status_code_ >= 300 && !body_.length())
    status_code_ = buildErrorPage(status_code_);

  createResponse();
}

void Response::createResponse() {
  if (config_.getMethod() == "HEAD")
    body_.clear();

  std::string status_code;

  if (headers_.count("Status"))
    status_code = headers_["Status"];
  else
    status_code = ft::to_string(status_code_) + " " + status_[status_code_];

  response_ = response_ + "HTTP/1.1" + " " + status_code + "\r\n";

  headers_["Date"] = ft::get_http_date();

  for (std::map<std::string, std::string>::iterator it = headers_.begin(); it != headers_.end(); it++)
    response_ += it->first + ": " + it->second + "\r\n";

  // #ifdef DEBUG
  std::cout << "\n-> RESPONSE <-\n" << response_ << std::endl;
  // #endif

  response_ += "\r\n";

  if (!body_.empty())
    response_ += body_;

  // std::cout << "\n-> RESPONSE <-\n" << response_ << std::endl;
}

int Response::GET() {
  if (file_.is_directory()) {
    std::string index = file_.find_index(config_.getIndexes());
    if (index.length())
      file_.set_path(config_.getRoot() + "/" + config_.getTarget() + index);
    else if (!config_.getAutoindex())
      return 404;
  }

  if (!file_.is_directory()) {
    if (!file_.exists())
      return 404;
    if (!config_.getHeader("Accept-Language").empty())
      localization();
    if (!file_.open())
      return 403;

    headers_["Last-Modified"] = file_.last_modified();
  }

  if (isCGI(file_.getExtension())) {
    CGI cgi(file_, config_, config_.getHeaders());

    if ((status_code_ = cgi.execute()) > 200)
      return status_code_;
    cgi.parseHeaders(headers_);
    body_ = cgi.getBody();
    headers_["Content-Length"] = ft::to_string(body_.length());
  } else if (config_.getAutoindex() && file_.is_directory()) {
    headers_["Content-Type"] = MimeTypes::getType(".html");
    body_ = file_.autoIndex(config_.getTarget());
    headers_["Content-Length"] = ft::to_string(body_.length());
  }
  else {
    headers_["Content-Type"] = MimeTypes::getType(file_.getExtension());
    body_ = file_.getContent();
    headers_["Content-Length"] = ft::to_string(body_.length());
  }
  return 200;
}

// OLD POST
// int status_code = 200;

//   if (isCGI(file_.getExtension())) {
//     CGI cgi(file_, config_, config_.getHeaders(), config_.getBody());

//     if ((status_code_ = cgi.execute()) > 200)
//       return status_code_;
//     cgi.parseHeaders(headers_);
//     body_ = cgi.getBody();
//     headers_["Content-Length"] = ft::to_string(body_.length());
//   } else if (!file_.exists()) {
//     file_.create(config_.getBody());
//     headers_["Content-Length"] = "0";
//     headers_["Content-Location"] = config_.getUri() + "/" + config_.getTarget();
//     status_code = 201;
//   }
//   else {
//     file_.create(config_.getBody());
//     headers_["Content-Location"] = config_.getUri() + "/" + config_.getTarget();
//     status_code = 204;
//   }
//   return status_code;

int Response::POST() {
  int status_code = 200;

  std::string path;
  if (!config_.getUpload().empty()) {
    File dir(config_.getRoot() + "/" + config_.getUpload());

    if (dir.exists() && !dir.is_directory()) {
      dir.unlink();
    }

    if (!dir.exists()) {
      if (mkdir(dir.getPath().c_str(), 0755))
        perror("/tmp/blah");
    }
    file_.set_path(config_.getRoot() + "/" + config_.getUpload() + "/" + config_.getTarget());
    path = config_.getUri() + "/" + config_.getUpload() + config_.getTarget();
  }

  if (isCGI(file_.getExtension())) {
    CGI cgi(file_, config_, config_.getHeaders(), config_.getBody());

    if ((status_code_ = cgi.execute()) > 200)
      return status_code_;
    cgi.parseHeaders(headers_);
    body_ = cgi.getBody();
    headers_["Content-Length"] = ft::to_string(body_.length());
  } else if (!file_.exists()) {
    path = config_.getUri() + "/" + config_.getTarget();
    file_.create(config_.getBody());
    headers_["Content-Length"] = "0";
    headers_["Location"] = ft::unique_char(path);
    status_code = 201;
  }
  else {
    path = config_.getUri() + "/" + config_.getTarget();
    file_.append(config_.getBody());
    headers_["Location"] = ft::unique_char(path);
    status_code = 204;
  }
  return status_code;
}

int Response::PUT() {
  int status_code = 204;

  if (!config_.getUpload().empty()) {
    File dir(config_.getRoot() + "/" + config_.getUpload());

    if (dir.exists() && !dir.is_directory()) {
      dir.unlink();
    }

    if (!dir.exists()) {
      if (mkdir(dir.getPath().c_str(), 0755))
        perror("/tmp/blah");
    }
    file_.set_path(config_.getRoot() + "/" + config_.getUpload() + "/" + config_.getTarget());
  }

  if (!file_.exists()) {
    file_.create(config_.getBody());
    headers_["Content-Length"] = "0";
    status_code = 201;
  }
  else {
    file_.create(config_.getBody());
  }
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

int Response::send(int fd) {
  int ret = ::send(fd, response_.c_str() + total_sent_, response_.length() - total_sent_, 0);
  total_sent_ += ret;
  if (total_sent_ >= response_.length())
    return 0;
  return 1;
}
