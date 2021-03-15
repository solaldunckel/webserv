# include "StatusCode.hpp"

std::map<int, std::string> StatusCode::error_codes_;

bool StatusCode::init_ = false;

void StatusCode::initMap() {
  // Informational 1xx
  error_codes_[100] = "Continue";
  error_codes_[101] = "Switching Protocols";

  // Successful 2xx
  error_codes_[200] = "OK";
  error_codes_[201] = "Created";
  error_codes_[202] = "Accepted";
  error_codes_[203] = "Non-Authoritative Information";
  error_codes_[204] = "No Content";
  error_codes_[205] = "Reset Content";
  error_codes_[206] = "Partial Content";

  // Redirection 3xx
  error_codes_[300] = "Multiple Choices";
  error_codes_[301] = "Moved Permanently";
  error_codes_[302] = "Found";
  error_codes_[303] = "See Other";
  error_codes_[304] = "Not Modified";
  error_codes_[305] = "Use Proxy";
  error_codes_[307] = "Temporary Redirect";

  // Client Error 4xx
  error_codes_[400] = "Bad Request";
  error_codes_[401] = "Unauthorized";
  error_codes_[402] = "Payment Required";
  error_codes_[403] = "Forbidden";
  error_codes_[404] = "Not Found";
  error_codes_[405] = "Method Not Allowed";
  error_codes_[406] = "Not Acceptable";
  error_codes_[407] = "Proxy Authentication Required";
  error_codes_[408] = "Request Timeout";
  error_codes_[409] = "Conflict";
  error_codes_[410] = "Gone";
  error_codes_[411] = "Length Required";
  error_codes_[412] = "Precondition Failed";
  error_codes_[413] = "Payload Too Large";
  error_codes_[414] = "URI Too Long";
  error_codes_[415] = "Unsupported Media Type";
  error_codes_[416] = "Range Not Satisfiable";
  error_codes_[417] = "Expectation Failed";
  error_codes_[426] = "Upgrade Required";

  // Server Error 5xx
  error_codes_[500] = "Internal Server Error";
  error_codes_[501] = "Not Implemented";
  error_codes_[502] = "Bad Gateway";
  error_codes_[503] = "Service Unavailable";
  error_codes_[504] = "Gateway Timeout";
  error_codes_[505] = "HTTP Version Not Supported";

  init_ = true;
}

std::string &StatusCode::operator[](int status_code) const {
  if (!init_)
    initMap();
  return error_codes_[status_code];
}