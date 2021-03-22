#ifndef CGI_HPP
# define CGI_HPP

# include <iostream>
# include <map>

# include <sys/types.h>
# include <sys/wait.h>

# include "RequestConfig.hpp"
# include "File.hpp"
# include "StringUtils.hpp"

class RequestConfig;

class CGI {
 public:
  CGI(File &file, RequestConfig &config, std::map<std::string, std::string, ft::comp> &req_headers);
  CGI(File &file, RequestConfig &config, std::map<std::string, std::string, ft::comp> &req_headers, std::string &req_body);
  ~CGI();

  void init();
  int execute();
  bool setCGIEnv();
  void parseHeaders(std::map<std::string, std::string> &headers);
  std::string &getBody();

 private:
  File &file_;
  RequestConfig &config_;
  std::map<std::string, std::string, ft::comp> &req_headers_;
  std::string cgi_path_;
  std::string cgi_exe_;
  std::string extension_;
  std::string cwd_;
  std::string file_path_;
  std::string body_;
  std::string req_body_;
  File tmp_file_;
  int tmp_fd_;
  std::map<std::string, std::string> cgi_env_;
  char **env_;
  char *argv_[3];
};

#endif