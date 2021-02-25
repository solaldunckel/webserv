#include "Server.hpp"

/*
** Constructors & Deconstructors
*/

Server::Server(std::vector<ServerConfig> &servers) : servers_(servers) {
  (void)servers_;
  FD_ZERO(&master_fds_);
  FD_ZERO(&read_fds_);
}

Server::~Server() {
  std::cout << "[Server] Shutdown." << std::endl;

  for (int fd = 0; fd <= max_fd_; fd++) {
    if (FD_ISSET(fd, &master_fds_)) {
      close(fd);
      FD_CLR(fd, &master_fds_);
    }
  }
}

bool Server::running_ = 0;

void Server::setup() {
  int yes = 1;
  int server_fd;

  for (std::vector<ServerConfig>::iterator it = servers_.begin(); it != servers_.end(); it++) {
    for (std::vector<Listen>::iterator list = it->getListens().begin(); list != it->getListens().end(); list++) {
      if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        throw std::runtime_error("Cannot create listening socket");

      fcntl(server_fd, F_SETFL, O_NONBLOCK);

      ft::memset(&address_, 0, sizeof(address_));
      address_.sin_family = AF_INET;
      address_.sin_addr.s_addr = inet_addr(list->ip_.c_str());
      address_.sin_port = htons(list->port_);

      setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

      if (bind(server_fd, (struct sockaddr *)&address_, sizeof(address_)) < 0)
        throw std::runtime_error(strerror(errno)) ;

      if (listen(server_fd, MAX_CONNECTION) < 0) {
        std::cout << "IN LISTEN" << std::endl;
        exit(EXIT_FAILURE);
      }

      FD_SET(server_fd, &master_fds_);

      max_fd_ = server_fd;
    }
  }
}

# define BUF_SIZE 50000

void Server::newConnection(int fd) {
  struct sockaddr_storage their_addr;
  socklen_t addr_size = sizeof(their_addr);

  // handle new connections
  int clientFd = accept(fd, (struct sockaddr *)&their_addr, &addr_size);
  fcntl(clientFd, F_SETFL, O_NONBLOCK);

  clients_[clientFd] = ft::inet_ntop(ft::get_in_addr((struct sockaddr *)&their_addr));

  std::cout << "[Server] New connection from " << clients_[clientFd] << " (socket " << fd << ")" << std::endl;

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

  int nbytes = 0;
  char buf[BUF_SIZE + 1];

  if ((nbytes = recv(fd, buf, BUF_SIZE, 0)) <= 0) {
    if (nbytes == 0) {
      std::cout << "[Server] Connection closed (socket " << fd << ")." << std::endl;
    } else {
      perror("recv");
    }
    close(fd); // bye!
    FD_CLR(fd, &master_fds_); // remove from master set
  } else {
    std::cout << "[Server] Receiving data from " << clients_[fd] << std::endl;

    buf[nbytes] = '\0';
    msg += buf;

    Request request(msg, servers_);

    request.parse();
    request.print();
  
    RequestConfig config(request, clients_[fd]);

    config.setup();

    Response response(config);

    response.build();
    response.send(fd);
  }
}

void Server::run() {
  std::signal(SIGINT, Server::interruptionHandler);

  running_ = true;

  server_fds_ = master_fds_;

  std::cout << "[Server] Starting." << std::endl;
  while (running_) {
    read_fds_ = master_fds_;
    if (select(max_fd_ + 1, &read_fds_, NULL, NULL, NULL) == -1) {
      strerror(errno);
      break ;
    }

    for (int fd = 0; fd <= max_fd_; fd++) {
      if (FD_ISSET(fd, &read_fds_)) { // we got one!!
        if (FD_ISSET(fd, &server_fds_)) {
          newConnection(fd);
        }
        else
          readData(fd);
      }
    }
  }
}
