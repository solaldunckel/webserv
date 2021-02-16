#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/select.h>

# include <csignal>
# include <vector>

# include "Config.hpp"
# include "Request.hpp"
# include "Response.hpp"

# define MAX_CONNECTION 10

class Server {
 public:
  // Constructors & Deconstructors
  Server(std::vector<ServerConfig> &servers);
  ~Server();

  void Setup();
  void Run();
  void readData(int fd);
  void newConnection();

  static void interruptionHandler(int sig_num) {
    (void)sig_num;
    running_ = false;
  };

 private:
  static bool running_;
  std::vector<ServerConfig> &servers_;
  struct sockaddr_in address_;
  int server_fd_;
  fd_set master_fds_;
  fd_set read_fds_;
  int max_fd_;
  // std::string addr_;
  // int port_;
  // struct sockaddr_in address_;
  // int server_fd_;
  // int addrlen_;
};

#endif
