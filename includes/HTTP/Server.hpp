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
# include <string.h>

# include "Client.hpp"
# include "ServerConfig.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "Utils.hpp"

# define MAX_CONNECTION 128
# define BUF_SIZE 65000

class webserv_exception : virtual public std::exception {
 public:
  webserv_exception(std::string msg, int err_num = 0, std::string arg = "") : errno_(err_num) {
    error_msg_ = msg;
    error_msg_.replace(error_msg_.find('%'), 2, arg);
    if (errno_) {
      error_msg_ = error_msg_ + " (" + ft::to_string(errno_) + ": " + strerror(errno_) + ")";
    }
  }
  virtual ~webserv_exception() throw () {}

  const char* what() const throw () {
    return error_msg_.c_str();
  }

 private:
  int errno_;
  std::string error_msg_;
};

class Server {
 public:
  // Constructors & Deconstructors
  Server(std::vector<ServerConfig> &servers);
  ~Server();

  void setup();
  void run();

  // int readData(int fd, std::string &buffer);
  int readData(int fd);
  void writeData(int fd);
  void newConnection(int fd);
  void clientDisconnect(int fd);

  static bool running_;

 private:
  std::vector<ServerConfig> &servers_;
  std::map<int, Listen> running_server_;
  std::map<int, Client*> clients_;
  std::vector<int> clients_bin_;
  fd_set master_fds_;
  fd_set read_fds_;
  fd_set write_fds_;
  int handle_client_fd_;
  int max_fd_;
};

#endif