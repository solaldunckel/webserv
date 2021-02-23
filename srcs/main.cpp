#include "Config.hpp"
#include "Server.hpp"

#define DEBUG true

int main(int argc, char **argv) {
  std::string path = "./config/default.conf";

  if (argc > 2) {
    std::cout << "error: too many arguments" << std::endl;
    return 1;
  }
  else if (argc == 2) {
    path = argv[1];
  }

  try {
    Config config(path);

    config.parse();

    if (DEBUG)
      config.print();
    std::cout << "TEST" << std::endl;

    Server serv(config.getServers());

    serv.setup();
    serv.run();
  }
  catch (std::exception &e) {
    std::cout << "error: " << e.what() << std::endl;
    return 1;
  }
}