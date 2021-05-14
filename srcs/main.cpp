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

    if (options.help())
      return 0;

    Config config(options.getPath());

    config.parse();

    if (options.verbose())
      config.print();

    Server serv(config.getServers(), options);

    serv.run();
  }
  catch (std::exception &e) {
    std::cerr << "webserv: " << e.what() << std::endl;
    return 1;
  }
}