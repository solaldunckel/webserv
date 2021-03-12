#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <vector>

# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/select.h>
# include <signal.h>

# include "ServerConfig.hpp"
# include "Request.hpp"
# include "Response.hpp"

# define MAX_CONNECTION 128
# define BUF_SIZE 4096

struct Client {
  std::string addr_;
  Request *req_;

  Client() : addr_(""), req_(nullptr) {};
  Client(std::string addr) : addr_(addr), req_(nullptr) {};
};

class Server {
 public:
  // Constructors & Deconstructors
  Server(std::vector<ServerConfig> &servers);
  ~Server();

  void setup();
  void run();

  void readData(int fd);
  void writeData(int fd);
  void newConnection(int fd);

  static bool running_;

 private:
  std::vector<ServerConfig> &servers_;
  std::map<int, Listen> running_server_;
  std::map<int, Client> clients_;
  fd_set master_fds_;
  fd_set read_fds_;
  fd_set write_fds_;
  int max_fd_;
};

#endif