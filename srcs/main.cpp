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

    #ifdef BONUS
    if (config.getWorkers() > 0) {
      std::vector<pid_t> workers(config.getWorkers());

      sem_unlink("/SEM_WEBSERV");
      sem_t *sem = sem_open("/SEM_WEBSERV", O_CREAT | O_EXCL, S_IRWXU, 1);

      for (int i = 0; i < config.getWorkers(); i++) {
        pid_t pid = fork();

        if (pid == 0)
          serv.run(i + 1, sem);
        workers[i] = pid;
      }

      int status = 0;
      if (waitpid(-1, &status, 0) < 0 || ((WIFEXITED(status)
          || WIFSIGNALED(status)) && status != 0)) {
        for (int i = 0; i < config.getWorkers(); i++)
          kill(workers[i], 0);
      }
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