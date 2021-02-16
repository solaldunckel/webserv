#include "Config.hpp"

bool is_directive(std::string str) {
    return (str == "listen" || str == "server_name" || str == "root" ||
            str == "error_page" || str == "upload" || str == "autoIndex" ||
            str == "index" || str == "cgi");
}

/*
** Constructors & Deconstructors
*/

Config::Config() {
}

Config::Config(std::string &path) : file_(path) {
  if (!file_.is_open() || !file_.good()) {
    throw std::runtime_error("could not open config file");
  }
}

Config::~Config() {
  file_.close();
}

void Config::init() {
  tokenize();
  parse();
}

void Config::tokenize() {
  std::string line, tmp;
  std::string::size_type first, last;
  std::stack<bool> brackets;

  int line_idx = 1;

  while (std::getline(file_, line))
  {
    last = 0;
    while ((first = line.find_first_not_of(" \t", last)) != std::string::npos)
    {
      if (line[first] == '#')
          break;
      last = line.find_first_of(" \t", first);
      tmp = line.substr(first, last - first);
      if (tmp == "{")
        brackets.push(true);
      else if (tmp == "}")
      {
        if (brackets.empty())
          throw std::runtime_error("extra closing '}' on line " + std::to_string(line_idx));
        brackets.pop();
      }
      if (is_directive(tmp) && line[line.find_last_not_of(" \t", line.length())] != ';')
        throw std::runtime_error("missing ';' on line " + std::to_string(line_idx));
      if (tmp.find(';', tmp.length() - 1) != std::string::npos)
      {
        tmp.pop_back();
        tokens_.push_back(tmp);
        tokens_.push_back(";");
      }
      else
        tokens_.push_back(tmp);
    }
    line_idx++;
  }
}

void Config::parse() {
  std::vector<std::string>::iterator it;

  for (it = tokens_.begin(); it != tokens_.end(); ++it)
  {
    if (*it == "server") {
      ServerConfig serv;

      serv.server(++it);
      servers_.push_back(serv);
    }
    else
      throw std::runtime_error("invalid directive '" + *it + "' in main block");
  }
  if (servers_.empty())
      throw std::runtime_error("missing server block"); // servers_.push_back(base_);
}

std::vector<ServerConfig> &Config::getServers() {
  return servers_;
}

void Config::print() {
  for (std::vector<ServerConfig>::iterator it = servers_.begin(); it != servers_.end(); it++) {
    it->print();
  }
}


