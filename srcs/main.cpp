#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>

#include "Parser.hpp"
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

  std::vector<Config> servers;

  try {
    Parser parse_(path, servers);

    parse_.Parse();

    Server serv(servers);

    serv.Setup();
    for (std::vector<Config>::iterator it = servers.begin(); it != servers.end(); it++) {
      it->print();
    }
  }
  catch (std::exception &e) {
    std::cout << "error: " << e.what() << std::endl;
  }

  // try {
  //   Config config_(path);
    
  //   config_.Parse();

  //   Server server_("localhost", 80);

  //   server_.Setup();
  //   server_.Run();
  // }
  // catch (std::exception &e) {
  //   std::cout << "error: " << e.what() << std::endl;
  // }
}