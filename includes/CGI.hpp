#ifndef CGI_HPP
# define CGI_HPP

# include <iostream>
# include <map>

# include "RequestConfig.hpp"
# include "File.hpp"
# include "Utils.hpp"

class CGI {
 public:
  CGI(File &file, RequestConfig &config);
  ~CGI();

  void execute();
  void setCGIEnv();
  std::string getBody();

 private:
  File &file_;
  RequestConfig &config_;
  std::string cgi_path_;
  std::string cgi_exe_;
  std::string cgi_bin_;
  std::string extension_;
  std::string cwd_;
  int tmp_fd_;
  std::map<std::string, std::string> cgi_env_;
  char **env_;
  char *argv_[3];
};

#endif