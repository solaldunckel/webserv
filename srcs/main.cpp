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
    Request req;

    req.parse("GET /md");
    req.parse("r HTTP/1.1\r\n");
    req.parse("Content-Length: 24\r\n");
    req.parse("Host: localhost:80\r\n");
    req.parse("\r\n");
    req.parse("the content");
    req.parse(" of your life\r\n");
    req.print();
    // Config config(path);

    // config.parse();

    // if (DEBUG)
    //   config.print();
    // std::cout << "TEST" << std::endl;

    // Server serv(config.getServers());

    // serv.setup();
    // serv.run();
  }
  catch (std::exception &e) {
    std::cout << "error: " << e.what() << std::endl;
    return 1;
  }
}