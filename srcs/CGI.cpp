#include "CGI.hpp"

CGI::CGI() {}

CGI::~CGI() {}

int CGI::execute() {
  return 1;
}

void CGI::setCGIEnv()
{
	m_env_["GATEWAY_INTERFACE"] = "CGI/1.1";
	m_env_["SERVER_SOFTWARE"] = "WEBSERV/1.0";
	m_env_["SERVER_PROTOCOL"] = "HTTP/1.1";
	m_env_["SERVER_PORT"] = std::to_string(config_.listen().port());
	m_env_["SCRIPT_NAME"] = cgi_path_;
	m_env_["SERVER_NAME"] = request_.headers.value("Host").substr(0, request_.headers.value("Host").find_first_of(':'));  // ??
	m_env_["REMOTE_ADDR"] = "";																							  // client ip
	m_env_["REQUEST_METHOD"] = request_.method;
	m_env_["QUERY_STRING"] = request_.query;  // everything after ?
	if (request_.method == "POST")
	{
		m_env_["CONTENT_TYPE"] = request_.headers.value("Content-Type");  // For queries which have attached information, such as HTTP POST and PUT, this is the content type of the data.
		m_env_["CONTENT_LENGTH"] = std::to_string(request_.body.length());				  // The length of the said content only for POST requests.
	}
	// if (authentification) {
	// m_env_["REMOTE_IDENT"] = ""; // Distant username of client. Only when authentificated?
	// m_env_["REMOTE_USER"] = ""; // Username of client if script is protected and server handle authentification
	// }
	m_env_["REQUEST_URI"] = file_path_;		 // The interpreted pathname of the requested document or CGI (relative to the document root)
	m_env_["PATH_INFO"] = file_path_;		 // ?
	m_env_["PATH_TRANSLATED"] = file_path_;	 // ?
	if (ext_ == "php")
		m_env_["REDIRECT_STATUS"] = "200";

  Headers::HeaderCollection head = request_.headers.getAll();
  for (Headers::HeaderCollection::const_iterator it = head.begin(); it != head.end(); it++)
		m_env_["HTTP_" + (std::string)it->name] = (std::string)it->value;

	if (!(env_ = (char **)malloc(sizeof(char *) * m_env_.size() + 1)))
		return;	 // !

	int i = 0;

	for (std::map<std::string, std::string>::iterator it = m_env_.begin(); it != m_env_.end(); it++)
	{
		std::string tmp = it->first + "=" + it->second;
		env_[i] = ft::strdup(tmp.c_str());
		i++;
	}
	env_[i] = NULL;
}