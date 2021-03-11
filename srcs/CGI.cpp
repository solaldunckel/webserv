#include "CGI.hpp"

std::string const CGI_TMP = "__cgi_tmp__.txt";

CGI::CGI(File &file, RequestConfig &config, std::map<std::string, std::string, comp> &req_headers) : file_(file), config_(config), req_headers_(req_headers) {
  char *cwd = getcwd(NULL, 0);
  cwd_ = cwd;
  free(cwd);

  env_ = nullptr;

  // std::cout << "INIT" << std::endl;
  req_body_ = file_.getContent();
  extension_ = file_.getExtension();
  cgi_exe_ = config.getCGI()[extension_];
  cgi_bin_ = "/cgi-bin";
  cgi_path_ = cwd_ + cgi_bin_ + "/" + cgi_exe_;
  tmp_fd_ = open(CGI_TMP.c_str(), O_CREAT | O_RDWR, 0666);
}

CGI::CGI(File &file, RequestConfig &config, std::map<std::string, std::string, comp> &req_headers, std::string &req_body) : file_(file), config_(config), req_headers_(req_headers) {
  char *cwd = getcwd(NULL, 0);
  cwd_ = cwd;
  free(cwd);

  env_ = nullptr;

  // std::cout << "INIT" << std::endl;
  req_body_ = req_body;
  extension_ = file_.getExtension();
  cgi_exe_ = config.getCGI()[extension_];
  cgi_bin_ = "/cgi-bin";
  cgi_path_ = cwd_ + cgi_bin_ + "/" + cgi_exe_;
  tmp_fd_ = open(CGI_TMP.c_str(), O_CREAT | O_RDWR, 0666);
}

void free_tab(char **tab) {
	int i = 0;
	while (tab[i]) {
		free(tab[i]);
    tab[i] = nullptr;
		i++;
	}
  free(tab);
}

CGI::~CGI() {
  if (env_) {
    free_tab(env_);
  }
  close(tmp_fd_);
  unlink(CGI_TMP.c_str());
}

void CGI::execute() {
  // std::cout << "EXECUTE " << cgi_path_ << std::endl;
  file_path_ = cwd_ + file_.getPath().erase(0, 1);
  // std::cout << "FILE " << file_path_ << std::endl;
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
		dup2(tmp_fd_, 1);
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
  createBody();
}

void CGI::createBody() {
  char buffer[4096 + 1];
  int ret;

  int fd = open(CGI_TMP.c_str(), O_RDWR, 0666);

  while ((ret = read(fd, buffer, 4096)) > 0) {
    buffer[ret] = '\0';
    body_ += buffer;
  }
  close(fd);
  // std::cout << "BODY : [" << body_ << "]" << std::endl;
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
	cgi_env_["SERVER_PROTOCOL"] = "HTTP/1.1";
	cgi_env_["SERVER_PORT"] = std::to_string(config_.getPort());
	cgi_env_["SCRIPT_NAME"] = cgi_path_;
	cgi_env_["SERVER_NAME"] = config_.getHost();  // ??
	// cgi_env_["REMOTE_ADDR"] = "";																							  // client ip
	cgi_env_["REQUEST_METHOD"] = config_.getMethod();
	// cgi_env_["QUERY_STRING"] = request_.query;  // everything after ?
	// if (request_.method == "POST") {
	// 	cgi_env_["CONTENT_TYPE"] = request_.headers.value("Content-Type");  // For queries which have attached information, such as HTTP POST and PUT, this is the content type of the data.
	// 	cgi_env_["CONTENT_LENGTH"] = std::to_string(request_.body.length());				  // The length of the said content only for POST requests.
	// }
	cgi_env_["REQUEST_URI"] = file_path_;		 // The interpreted pathname of the requested document or CGI (relative to the document root)
	cgi_env_["PATH_INFO"] = file_path_;		 // ?
	cgi_env_["PATH_TRANSLATED"] = file_path_;	 // ?
  for (std::map<std::string, std::string, comp>::iterator it = req_headers_.begin(); it != req_headers_.end(); it++) {
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