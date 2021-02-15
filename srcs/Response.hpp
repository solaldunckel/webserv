#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>

# include <fcntl.h>

# include "Request.hpp"
# include "Config.hpp"

class Response {
 public:
  // Constructors & Deconstructors
  Response(Request &request);
 // Response(Request &request, Config &config);
  ~Response();

  static void initErrorCodes();

  std::string getResponseBody();
  int openFile();

 private:
  Request &request_;
  int status_code_;
  std::string body_;
  static std::map<int, std::string> error_codes_;
  //Config &config_;
};

#endif
