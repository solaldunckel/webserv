#include "Config.hpp"
#include "Server.hpp"
#include <regex.h>

int main(int argc, char **argv) {
  std::string path = "./config/default.conf";

  if (argc > 2) {
    regex_t test;
    char reg[] = ".(png|ico|gif|jpg|jpeg|css|js)$";
    char add[] = "/images/TEST.CSS";
    int err = regcomp(&test, reg, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    if (err == 0)
    {
    /* (2) */
      int match = regexec (&test, add, 0, NULL, 0);
      regfree(&test);
      if (match == 0)
        std::cout << "REG VALID" << std::endl;
      else if (match == REG_NOMATCH)
        std::cout << "REG INVALID" << std::endl;
    }

    std::cout << "webserv: too many arguments" << std::endl;
    return 1;
  }
  else if (argc == 2) {
    path = argv[1];
  }

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