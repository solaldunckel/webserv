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

# include <vector>

# include "Config.hpp"

# define MAX_CONNECTION 10

class Server {
 public:
  // Constructors & Deconstructors
  Server(std::vector<Config> &config);
  ~Server();

  void Setup();
  void Run();
  void readData(int fd);

 private:
  std::vector<Config> &config_;
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
