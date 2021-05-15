#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <vector>
# include <cstring>

# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/select.h>
# include <signal.h>
# include <semaphore.h>

# include "InputArgs.hpp"
# include "Client.hpp"
# include "ServerConfig.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "Utils.hpp"

# define MAX_CONNECTION 128
# define MAX_CLIENT 1000
# define BUF_SIZE 65536

class Client;

class Server {
 public:
  // Constructors & Deconstructors
  Server(std::vector<ServerConfig> &servers, InputArgs &options);
  ~Server();

  void setup();
  void run(int worker_id = 0, sem_t *sem = NULL);

  int readData(int fd);
  void writeData(int fd);
  void newConnection(int fd);
  void clientDisconnect(int fd);
  void closeClient(int fd);

  static bool running_;
  int worker_id_;

 private:
  std::vector<ServerConfig> &servers_;
  InputArgs &options_;
  std::map<int, Listen> running_server_;
  std::map<int, Client*> clients_;
  fd_set master_fds_;
  fd_set read_fds_;
  fd_set write_fds_;
  int max_fd_;
  int max_fd_tmp_;
};

#endif