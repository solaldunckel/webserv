#include "InputArgs.hpp"
#include "Config.hpp"
#include "Server.hpp"
#include "Logger.hpp"
#include <pthread.h>

#ifdef BONUS
pthread_mutex_t g_accept;
pthread_mutex_t g_write;

struct Worker {
  Server *serv_;
  int id_;
  pthread_t thr_;
  int pip_[2];
};

std::vector<Worker> g_workers;

void *startWorker(void *arg) {
  Server *serv = ((Worker*)arg)->serv_;
  int id = ((Worker*)arg)->id_;
  serv->run(id);
  return NULL;
}
#endif

Logger Log;

int main(int argc, char **argv) {
  try {
    InputArgs options(argc, argv);

    options.parse();

    if (options.help()) {
      std::cout << options.helpText() << std::endl;
      return 0;
    }

    Log.setLogLevel(options.log());

    Config config(options.getPath());

    config.parse();

    if (options.test()) {
      Log.print(INFO, "# configuration file " + config.getPath() + "\n\n" + config.getFileContent());
      return 0;
    }

    Log.print(INFO, "# configuration file " + config.getPath());

    config.clear();

    Server serv(config.getServers(), options);

    serv.setup();

    #ifdef BONUS
    if (config.getWorkers() > 0) {
      g_workers.resize(config.getWorkers());

      pthread_mutex_init(&g_accept, NULL);
	    pthread_mutex_init(&g_write, NULL);

      for (int i = 0; i < config.getWorkers(); i++) {
        g_workers[i].id_ = i + 1;
        g_workers[i].serv_ = new Server(serv);
        pthread_create(&g_workers[i].thr_, NULL, startWorker, &g_workers[i]);
        usleep(500);
      }

      for (int i = 0; i < config.getWorkers(); i++) {
        pthread_join(g_workers[i].thr_, NULL);
        delete g_workers[i].serv_;
      }

      pthread_mutex_destroy(&g_accept);
	    pthread_mutex_destroy(&g_write);
    } else {
      serv.run();
    }
    #else
      serv.run();
    #endif
  }
  catch (std::exception &e) {
    Log.print(INFO, e.what(), RED, true);
    return 1;
  }
  return 0;
}