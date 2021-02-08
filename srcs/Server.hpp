#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <netinet/in.h>
# include <sys/socket.h>
# include <unistd.h>
# include <fcntl.h>

class Server {
 public:
  // Constructors & Deconstructors
  Server(std::string addr, int port);
  ~Server();

  void Setup();
  void Run();

 private:
  std::string addr_;
  int port_;
  struct sockaddr_in address_;
  int server_fd_;
  int addrlen_;
};

#endif
