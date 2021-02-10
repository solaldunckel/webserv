#include "Listen.hpp"

bool isValidIp(std::string ip) {
  size_t pos = 0;
  int count = 1;

  std::string token;
  std::string delimiter = ".";
  while ((pos = ip.find(delimiter)) != std::string::npos) {
    token = ip.substr(0, pos);
    if (token.find_first_not_of("0123456789") != std::string::npos)
      return false;
    ip.erase(0, pos + delimiter.length());
    count++;
  }
  if (count != 4)
    return false;
  return true;
}

bool isValidPort(int port) {
  return port > 0 && port < 50000;
}

/*
** Constructors & Deconstructors
*/

Listen::Listen() : ip_("0.0.0.0"), port_(80) {
}

Listen::~Listen() {
}

void Listen::setup(std::string &str) {
  if (str.find(':') != std::string::npos) {
    ip_ = str.substr(0, str.find(':'));

    std::string port = str.substr(str.find(':') + 1);

    if (port.find_first_not_of("0123456789") != std::string::npos) {
      std::cout << "Invalid port" << std::endl;
    }

    port_ = std::stoi(port);
  }
  else if (str.find_first_not_of("0123456789") != std::string::npos) {
    ip_ = str;
    std::cout << "test" << std::endl;
  }
  else
    port_ = std::stoi(str);
}

std::string &Listen::getIp() {
  return ip_;
}

int Listen::getPort() {
  return port_;
}

bool Listen::valid() {
  return (isValidIp(ip_) && isValidPort(port_));
}
