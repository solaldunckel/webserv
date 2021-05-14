#include "Config.hpp"
#include "Server.hpp"

int main(int argc, char **argv) {
  std::string path = "./config/default.conf";

  if (argc > 2) {
    std::cout << "webserv: too many arguments" << std::endl;
    return 1;
  }
  else if (argc == 2) {
    path = argv[1];
  }

  #ifdef BONUS
    std::cout << "Launched with bonus" << std::endl;
  #endif

  try {
    Config config(path);

    config.parse();

    #ifdef DEBUG
      config.print();
    #endif

    Server serv(config.getServers());

    serv.run();
  }
  catch (std::exception &e) {
    std::cerr << "webserv: " << e.what() << std::endl;
    return 1;
  }
}