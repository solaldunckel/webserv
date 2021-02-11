#include "Server.hpp"

/*
** Constructors & Deconstructors
*/

Server::Server(std::vector<Config> &config) : config_(config) {
  (void)config_;
}

Server::~Server() {
}

// void Server::interruptionHandler(int sig_num) {

// }

bool Server::running_ = 0;

void Server::Setup() {
  std::string addr = "127.0.0.1";
  // char *addr = "192.168.2.13";
  uint32_t port = 8000;

  FD_ZERO(&master_fds_);    // clear the master and temp sets
  FD_ZERO(&read_fds_);

  if ((server_fd_ = socket(PF_INET, SOCK_STREAM, 0)) < 0)
      throw std::runtime_error("Cannot create listening socket");

  memset(&address_, 0, sizeof(address_));
  address_.sin_family = AF_INET;
  address_.sin_addr.s_addr = inet_addr(addr.c_str());
  address_.sin_port = htons(port);

  int yes = 1;
  setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

  if (bind(server_fd_, (struct sockaddr *)&address_, sizeof(address_)) < 0) {
      std::cout << "CANNOT BIND SOCKET" << std::endl;
      std::cout << strerror(errno) << std::endl;
      return ;
  }

  if (listen(server_fd_, MAX_CONNECTION) < 0) {
      std::cout << "IN LISTEN" << std::endl;
      exit(EXIT_FAILURE);
  }

  FD_SET(server_fd_, &master_fds_);

  max_fd_ = server_fd_;

  std::signal(SIGINT, Server::interruptionHandler);

  running_ = true;

  while (running_) {
    read_fds_ = master_fds_; // copy it
    if (select(max_fd_ + 1, &read_fds_, NULL, NULL, NULL) == -1) {
      strerror(errno);
      break ;
    }
      
    // run through the existing connections looking for data to read
    for (int i = 0; i <= max_fd_; i++) {
      if (FD_ISSET(i, &read_fds_)) { // we got one!!
        if (i == server_fd_)
          newConnection();
        else
          readData(i);
      }
    }
  }
  std::cout << "[Server] Shutdown." << std::endl;
  close(server_fd_);
}

# define BUF_SIZE 500

void Server::newConnection() {
  // handle new connections
  int clientFd = accept(server_fd_, nullptr, nullptr);
  fcntl(clientFd, F_SETFL, O_NONBLOCK);

  if (clientFd == -1)
    perror("accept");
  else {
    //std::cout << "[Server] Connection from " << buf << std::endl;
    FD_SET(clientFd, &master_fds_); // add to master set
    if (clientFd > max_fd_)   // keep track of the max
        max_fd_ = clientFd;
  }
}

void Server::readData(int fd) {
  std::string msg;
  std::string response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
  std::cout << "READ DATA" << std::endl;

  int nbytes = 1;
  char buf[BUF_SIZE + 1];

  while (nbytes > 0) {
    memset(buf, 0, BUF_SIZE + 1);	//clear the variable
    if ((nbytes = recv(fd, buf, BUF_SIZE, 0)) <= 0) {
      // std::cout << "ERROR READING DATA" << std::endl;
      // close(fd); // bye!
      // FD_CLR(fd, &master_fds_); // remove from master set
      break ;
    } else {
      buf[BUF_SIZE] = '\0';
      msg += buf;
    }
	}

  std::cout << "Parse request" << std::endl;

  Request request(msg);

  request.parse();

  // We should create Request Class
  // Then we should create Response Class
  write(fd, response.c_str(), response.length());

    // Finish Connection ?
  // close(fd); // bye!
  // FD_CLR(fd, &master_fds_); // remove from master set
}

void Server::Run() {
  // std::string hello = "Hello from server";

  // while (1) {

  //   /* Here we need to use select and check fds */
  //   int client;

  //   std::cout << " *** WAITING FOR NEW CONNECTION ***" << std::endl;

  //   if ((client = accept(server_fd_, (struct sockaddr *)&address_, (socklen_t*)&addrlen_)) < 0)
  //   {
  //       std::cout << "IN ACCEPT" << std::endl;
  //       exit(EXIT_FAILURE);
  //   }

  //   fcntl(client, F_SETFL, O_NONBLOCK);  // set it as non-blocking

  //   /* We read the request and process it */

  //   /* We write our response into the socket */

  //   write(client, hello.c_str(), hello.length());

  //   std::cout << " MESSAGE SENT" << std::endl;

  //   close(client);
  // }
}
