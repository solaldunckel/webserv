#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>

# include <fcntl.h>

# include "Request.hpp"
# include "Config.hpp"
# include "MimeTypes.hpp"
# include "RequestConfig.hpp"
# include "File.hpp"

class Response {
 public:
  // Constructors & Deconstructors
  Response(RequestConfig &config);
  ~Response();

  static void initErrorCodes();

  void build();

  int handleGet();

  std::string getResponseBody();
  int send(int fd);

 private:
  RequestConfig &config_;
  int status_code_;
  std::string response_;
  std::map<std::string, std::string> headers_;
  static std::map<int, std::string> error_codes_;
};

#endif
