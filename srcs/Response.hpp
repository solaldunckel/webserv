#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>

# include <fcntl.h>

# include "Request.hpp"
# include "Config.hpp"

struct FileHandler {
  int           fd_;
  std::ifstream stream_;
  bool          is_dir_;
  std::string   name_;
  std::string   full_path_;
  std::string   body_;
  std::string   mime_;
};

class Response {
 public:

  // Constructors & Deconstructors
  Response(Request &request);
 // Response(Request &request, Config &config);
  ~Response();

  static void initErrorCodes();
  static void initMimeTypes();

  std::string getResponseBody();
  int openFile(FileHandler &file);
  std::string getMimeType(FileHandler &file);
  std::string getFileContent(FileHandler &file);

 private:
  Request &request_;
  int status_code_;
  std::string body_;
  static std::map<int, std::string> error_codes_;
  static std::map<std::string, std::string> mime_types_;
  //Config &config_;
};

#endif
