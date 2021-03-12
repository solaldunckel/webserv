#include "Config.hpp"

Config::Config(std::string const &path) : file_(path) {
  if (!file_.is_open() || !file_.good()) {
    throw std::runtime_error("could not open config file");
  }
}

Config::~Config() {}

void Config::tokenize() {
  std::string line, tmp;
  std::string::size_type first, last;
  std::stack<bool> brackets;

  int line_idx = 1;

  while (std::getline(file_, line)) {
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
          throw std::runtime_error("extra closing '}' on line " + std::to_string(line_idx));
        brackets.pop();
      }
      if (isValidDirective(tmp) && line[line.find_last_not_of(" \t", line.length())] != ';')
        throw std::runtime_error("missing ';' on line " + std::to_string(line_idx));
      if (tmp.find(';', tmp.length() - 1) != std::string::npos) {
        tmp.pop_back();
        tokens_.push_back(tmp);
        tokens_.push_back(";");
      }
      else
        tokens_.push_back(tmp);
    }
    line_idx++;
  }
  file_.close();
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
    else
      throw std::runtime_error("invalid directive '" + *it + "' in main block");
  }
  if (servers_.empty())
    throw std::runtime_error("missing server block");
  tokens_.clear();
}

std::vector<ServerConfig> &Config::getServers() {
  return servers_;
}

void Config::print() {
  std::cout << "### CONFIG :\n" << std::endl;
  for (std::vector<ServerConfig>::iterator it = servers_.begin(); it != servers_.end(); it++) {
    it->print();
    std::cout << std::endl;
  }
  std::cout << "###\n" << std::endl;
}
