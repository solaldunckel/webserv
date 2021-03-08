#include "CGI.hpp"

std::string const CGI_TMP = "__cgi_tmp__.txt";

CGI::CGI(File &file, RequestConfig &config) : file_(file), config_(config) {
  extension_ = file_.getExtension();
  cgi_exe_ = config.getCGI()[extension_];
  cgi_bin_ = "/cgi-bin";
  cgi_path_ = "." + cgi_bin_ + "/" + cgi_exe_;
  tmp_fd_ = open(CGI_TMP.c_str(), O_CREAT | O_RDWR, 0666);

  char *cwd = getcwd(NULL, 0);
  cwd_ = cwd;
  free(cwd);
}

CGI::~CGI() {
  close(tmp_fd_);
  // unlink(CGI_TMP.c_str());
  free(argv_[0]);
  free(argv_[1]);
}

void CGI::execute() {
  std::cout << "EXECUTE " << cgi_path_ << std::endl;
  std::cout << "WORKING DIR " << cwd_ << std::endl;

  argv_[0] = ft::strdup(cgi_path_.c_str());
  argv_[1] = ft::strdup(file_.getPath().c_str());
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
    write(pip[1], file_.getContent().c_str(), file_.getContent().length());
		close(pip[1]);
    if (waitpid(pid, NULL, 0) == -1)
			perror("wait");
	}
  else
    perror("fork");
  close(tmp_fd_);
}

std::string CGI::getBody() {
  char buffer[4096 + 1];
  int ret;
  std::string body;

  int fd = open(CGI_TMP.c_str(), O_RDWR, 0666);

  while ((ret = read(fd, buffer, 4096)) > 0)
  {
    buffer[ret] = '\0';
    body += buffer;
  }
  close(fd);
  return body;
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
	// cgi_env_["REQUEST_URI"] = file_path_;		 // The interpreted pathname of the requested document or CGI (relative to the document root)
	// cgi_env_["PATH_INFO"] = file_path_;		 // ?
	// cgi_env_["PATH_TRANSLATED"] = file_path_;	 // ?
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