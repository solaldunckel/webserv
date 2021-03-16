#include "CGI.hpp"

std::string const CGI_TMP = "/tmp/__cgi_tmp__";

CGI::CGI(File &file, RequestConfig &config, std::map<std::string, std::string, ft::comp> &req_headers) : file_(file), config_(config), req_headers_(req_headers) {
  char *cwd = getcwd(NULL, 0);
  cwd_ = cwd;
  free(cwd);

  env_ = nullptr;

  req_body_ = file_.getContent();
  extension_ = file_.getExtension();
  cgi_exe_ = config.getCGI()[extension_];
  cgi_path_ = cwd_ + "/" + config.getCGIBin() + "/" + cgi_exe_;
  tmp_file_.set_path(CGI_TMP.c_str());
  tmp_file_.open(true);
}

CGI::CGI(File &file, RequestConfig &config, std::map<std::string, std::string, ft::comp> &req_headers, std::string &req_body) : file_(file), config_(config), req_headers_(req_headers) {
  char *cwd = getcwd(NULL, 0);
  cwd_ = cwd;
  free(cwd);

  env_ = nullptr;

  req_body_ = req_body;
  extension_ = file_.getExtension();
  cgi_exe_ = config.getCGI()[extension_];
  cgi_path_ = cwd_ + "/" + config.getCGIBin() + "/" + cgi_exe_;
  tmp_file_.set_path(CGI_TMP.c_str());
  tmp_file_.open(true);
}

CGI::~CGI() {
  if (env_)
    ft::free_tab(env_);
  tmp_file_.unlink();
}

void CGI::execute() {
  file_path_ = cwd_ + "/" + file_.getPath();
  std::cout << file_path_ << std::endl;
  chdir(file_path_.substr(0, file_path_.find_last_of('/')).c_str());
  setCGIEnv();

  argv_[0] = ft::strdup(cgi_path_.c_str());
  argv_[1] = ft::strdup(file_path_.c_str());
  argv_[2] = nullptr;

  int pip[2];
  pipe(pip);

  pid_t pid = fork();

  if (pid == 0) {
    close(pip[1]);
    dup2(pip[0], 0);
    dup2(tmp_file_.getFd(), 1);
    close(pip[0]);
    execve(argv_[0], argv_, env_);
    exit(1);
  }
  else if (pid > 0) {
    close(pip[0]);
    write(pip[1], req_body_.c_str(), req_body_.length());
    close(pip[1]);
    if (waitpid(pid, NULL, 0) == -1)
      perror("wait");
    close(tmp_fd_);
  }
  else
    perror("fork");
  free(argv_[0]);
  free(argv_[1]);
  chdir(cwd_.c_str());
  body_ = tmp_file_.getContent();
}

void CGI::parseHeaders(std::map<std::string, std::string> &headers) {
  size_t end, last;
  std::string header;

  while ((end = body_.find("\r\n")) != std::string::npos) {
    if (body_.find("\r\n") == 0) {
      body_.erase(0, end + 2);
      break;
    }
    if ((last = body_.find(':', 0)) != std::string::npos) {
      header = body_.substr(0, last);
      headers[header] = ft::trim_left(body_.substr(last + 1, end - last - 1), ' ');
    }
    body_.erase(0, end + 2);
  }
}

std::string &CGI::getBody() {
  return body_;
}

void CGI::setCGIEnv() {
	cgi_env_["GATEWAY_INTERFACE"] = "CGI/1.1";
	cgi_env_["SERVER_SOFTWARE"] = "WEBSERV/1.0";
	cgi_env_["SERVER_PROTOCOL"] = config_.getProtocol();
	cgi_env_["SERVER_PORT"] = std::to_string(config_.getPort());
	cgi_env_["SCRIPT_NAME"] = cgi_path_;
	cgi_env_["SERVER_NAME"] = config_.getHost();
	cgi_env_["REMOTE_ADDR"] = config_.getClient().getAddr();
	cgi_env_["REQUEST_METHOD"] = config_.getMethod();
	cgi_env_["QUERY_STRING"] = config_.getQuery();
	if (config_.getMethod() == "POST") {
		cgi_env_["CONTENT_TYPE"] = req_headers_["Content-Type"];
		cgi_env_["CONTENT_LENGTH"] = std::to_string(req_body_.length());
	}
	cgi_env_["REQUEST_URI"] = file_path_;
	cgi_env_["PATH_INFO"] = file_path_;
	cgi_env_["PATH_TRANSLATED"] = file_path_;

  for (std::map<std::string, std::string, ft::comp>::iterator it = req_headers_.begin(); it != req_headers_.end(); it++) {
    if (!it->second.empty())
      cgi_env_["HTTP_" + it->first] = it->second;
  }

	if (extension_ == ".php")
		cgi_env_["REDIRECT_STATUS"] = "200";

	if (!(env_ = (char **)malloc(sizeof(char *) * (cgi_env_.size() + 1))))
		return;

	int i = 0;

	for (std::map<std::string, std::string>::iterator it = cgi_env_.begin(); it != cgi_env_.end(); it++) {
		std::string tmp = it->first + "=" + it->second;
		env_[i] = ft::strdup(tmp.c_str());
		i++;
	}
	env_[i] = NULL;
}