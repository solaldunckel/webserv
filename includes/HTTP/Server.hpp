#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <vector>
# include <cstring>
# include <set>

# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/select.h>
# include <signal.h>
# include <pthread.h>

# include "Logger.hpp"
# include "InputArgs.hpp"
# include "Client.hpp"
# include "ServerConfig.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "Utils.hpp"

# define MAX_CONNECTION 128
# define MAX_CLIENT 1000
# define BUF_SIZE 65536

extern pthread_mutex_t g_accept;
extern pthread_mutex_t g_write;
extern Logger Log;

class Client;

class Server {
 public:
  // Constructors & Deconstructors
  Server(const Server &copy);
  Server(std::vector<ServerConfig> &servers, InputArgs &options);
  ~Server();
  Server &operator=(const Server &copy);

  void setup();
  void run(int worker_id = 0);

  bool recv(int fd);
  bool send(int fd);
  bool createWorker();
  void newConnection(int fd);
  void clientDisconnect(int fd);
  void closeClient(int fd);
  void add_to_fd_set(int fd);
  void remove_from_fd_set(int fd);
  void check_timeout_disconnect(Client *client);
  void print(std::string str);

  static bool running_;
  int worker_id_;

 private:
  std::vector<ServerConfig> &servers_;
  std::string head_;
  InputArgs &options_;
  std::map<int, Listen> running_server_;
  std::map<int, Client*> clients_;
  fd_set master_fds_;
  fd_set read_fds_;
  fd_set write_fds_;
  std::set<int> fd_set_;
  int max_fd_;
};

#endif