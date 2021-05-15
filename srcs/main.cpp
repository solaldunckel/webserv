#include "InputArgs.hpp"
#include "Config.hpp"
#include "Server.hpp"

int main(int argc, char **argv) {

  #ifdef BONUS
    std::cout << "Launched with bonus" << std::endl;
  #endif

  try {
    InputArgs options(argc, argv);

    options.parse();

    if (options.help()) {
      std::cout << options.helpText() << std::endl;
      return 0;
    }

    Config config(options.getPath());

    config.parse();

    if (options.test()) {
      config.print();
      return 0;
    }

    config.clear();

    Server serv(config.getServers(), options);

    serv.run();
  }
  catch (std::exception &e) {
    std::cerr << "webserv: " << e.what() << std::endl;
    return 1;
  }
}