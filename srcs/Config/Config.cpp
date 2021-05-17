#include "Config.hpp"

Config::Config(std::string &path) : path_(path) {
  #ifdef BONUS
  workers_ = 0;
  #endif
  fd_ = open(path_.c_str(), O_RDONLY);
  if (fd_ < 0)
    throw webserv_exception("could not open configuration file : %", 0, path_);
}

Config::~Config() {
  clear();
}

void Config::clear() {
  if (fd_ > 0) {
    close(fd_);
    fd_ = 0;
  }
  tokens_.clear();
  file_content_.clear();
}

void Config::tokenize() {
  std::string line, tmp;
  std::string::size_type first, last;
  std::stack<bool> brackets;

  int line_idx = 1;
  char *line_c;

  while (get_next_line(fd_, &line_c)) {
    line = line_c;
    file_content_ += line + "\n";
    last = 0;
    while ((first = line.find_first_not_of(" \t", last)) != std::string::npos) {
      if (line[first] == '#')
        break;
      last = line.find_first_of(" \t", first);
      tmp = line.substr(first, last - first);
      if (tmp == "{")
        brackets.push(true);
      else if (tmp == "}") {
        if (brackets.empty())
          throw webserv_exception("extra closing '}' on line %", 0, ft::to_string(line_idx));
        brackets.pop();
      }
      if (isValidDirective(tmp) && line[line.find_last_not_of(" \t", line.length())] != ';')
        throw webserv_exception("missing ';' on line %", 0, ft::to_string(line_idx));
      if (tmp.find(';', tmp.length() - 1) != std::string::npos) {
        tmp.erase(tmp.length() - 1, 1);
        tokens_.push_back(tmp);
        tokens_.push_back(";");
      }
      else
        tokens_.push_back(tmp);
    }
    line_idx++;
    free(line_c);
  }
  free(line_c);
}

void Config::parse() {
  tokenize();
  std::vector<std::string>::iterator it;

  for (it = tokens_.begin(); it != tokens_.end(); ++it) {
    if (*it == "server") {
      ServerConfig serv;

      serv.server(++it);
      servers_.push_back(serv);
    }
    #ifdef BONUS
    else if (*it == "workers") {
      workers_ = ft::stoi(*(++it));
      if (*++it != ";")
        throw std::runtime_error("missing ';' in 'workers'");
    }
    #endif
    else
      throw webserv_exception("invalid directive % in main block", 0, *it);
  }
  if (servers_.empty())
    throw webserv_exception("missing server block");
}

std::string &Config::getPath() {
  return path_;
}

std::vector<ServerConfig> &Config::getServers() {
  return servers_;
}

#ifdef BONUS
int Config::getWorkers() {
  return workers_;
}
#endif

void Config::print() {
  std::cout << "# configuration file " << path_ << ":\n" << std::endl;
  std::cout << file_content_ << std::endl;
}
