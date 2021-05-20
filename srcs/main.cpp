#include "InputArgs.hpp"
#include "Config.hpp"
#include "Server.hpp"

void catch_sigint(int sig) {
  (void)sig;
}

int main(int argc, char **argv) {
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

    serv.setup();

    #ifdef BONUS
    if (config.getWorkers() > 0) {
      std::vector<pid_t> workers(config.getWorkers());

      signal(SIGINT, catch_sigint);

      for (int i = 0; i < config.getWorkers(); i++) {
        pid_t pid = fork();

        if (pid == 0) {
          serv.run(i + 1);
          exit(0);
        }
        else if (pid == -1) {
          std::cout << strerror(errno) << std::endl;
          return 1;
        }
        else
          workers[i] = pid;
      }
      wait(NULL);
      for (std::vector<pid_t>::iterator it = workers.begin(); it != workers.end(); it++)
        kill(*it, SIGINT);
    } else {
      serv.run();
    }
    #else
      serv.run();
    #endif
  }
  catch (std::exception &e) {
    std::cerr << "webserv: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}