#include "Config.hpp"
#include "Server.hpp"

#define DEBUG true

int main(int argc, char **argv) {
  std::string path = "./config/default.conf";

  if (argc > 2) {
    std::cout << "error: too many arguments" << std::endl;
    Request req;

    // req.parse("POST /chunk HTTP/1.1\r\n");
    // req.parse("Content-Type:   text");
    // req.parse("/html\r\n");
    // req.parse("Transfer-Encoding:  chunked\r\n");
    // req.parse("User-Agent: Go-http-client/1.1\r\n");
    // req.parse("Host: localhost:80\r\n");
    // req.parse("Accept-Encoding: gzip\r\n");
    // req.parse("\r\n");
    // req.parse("26\r\nVoici les donnees du premier morceau\r\n\r\n1C\r\net voici un second morceau\r\n\r\n20\r\net voici deux derniers morceaux \r\n12\r\nsans saut de ligne\r\n0\r\n\r\n");

    // req.print();
    // req.clear();

    req.parse("POST /chunk HTTP/1.1\r\n");
    req.parse("Host: localhost\r\n");
    req.parse("Transfer-Encoding: chunked\r\n");
    req.parse("User-Agent: Go-http-client/1.1\r\n");
    req.parse("Accept-Encoding: gzip\r");
    req.parse("\n\r\n");
    req.parse("0\r\n");
    // req.parse("26\r\nVoici les donnees du premier morceau\r\n\r\n1C\r\net voici un second morceau\r\n\r\n20\r\net voici deux derniers morceaux \r\n12\r\nsans saut de ligne\r\n0\r\n\r\n");

    req.print();
    req.clear();
    // req.parse("PUT /body");
    // req.parse("r HTTP/1.1\r\n");
    // req.parse("Content-Length: 22\r\n");
    // req.parse("Content-Type:   text");
    // req.parse("/html\r\n");
    // req.parse("Host: localhost:80\r\n");
    // req.parse("\r\n");
    // req.parse("Hello my boy");
    // req.parse(" from hell");

    // req.print();
    // req.clear();
  
    // req.parse("GET /");
    // req.parse(" HTTP/1.1\r\n");
    // req.parse("Host: localhost:80\r\n");
    // req.parse("\r\n");

    // req.print();
    // req.clear();
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

    Server serv(config.getServers());

    serv.setup();
    serv.run();
  }
  catch (std::exception &e) {
    std::cout << "error: " << e.what() << std::endl;
    return 1;
  }
}