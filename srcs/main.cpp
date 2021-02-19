#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>

#include "Config.hpp"
#include "Server.hpp"

int main(int argc, char **argv) {
  std::string path = "./config/default.conf";

  if (argc > 2) {
    std::cout << "error: too many arguments" << std::endl;
  }
  else if (argc == 2) {
    path = argv[1];
  }

  try {
    Config config(path);

    config.init();
    std::cout << "### CONFIG :\n" << std::endl;
    config.print();
    std::cout << "###\n" << std::endl;

    Server serv(config.getServers());

    serv.Setup();
    serv.Run();
  }
  catch (std::exception &e) {
    std::cout << "error: " << e.what() << std::endl;
  }
}