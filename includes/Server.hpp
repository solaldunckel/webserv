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
  void newConnection(int fd);

  static void interruptionHandler(int sig_num) {
    (void)sig_num;
    std::cout << "\b\b  \b\b";
    running_ = false;
  };

 private:
  static bool running_;
  std::vector<ServerConfig> &servers_;
  std::map<int, std::string> clients_;
  struct sockaddr_in address_;
  std::vector<int> server_fds_;
  fd_set master_fds_;
  fd_set read_fds_;
  int max_fd_;
};

#endif