#include "Server.hpp"

/*
** Constructors & Deconstructors
*/

// client[fd] = Request

bool Server::running_ = false;

static void interruptHandler(int sig_int) {
  (void)sig_int;
	std::cout << "\b\b \b\b";
	Server::running_ = false;
}

Server::Server(std::vector<ServerConfig> &servers) : servers_(servers) {
  FD_ZERO(&master_fds_);
  FD_ZERO(&read_fds_);
  FD_ZERO(&write_fds_);
}

Server::~Server() {
  std::cout << "[Server] Shutdown." << std::endl;

  for (std::map<int, Client*>::iterator it = clients_.begin(); it != clients_.end(); it++) {
    Client *client = it->second;

    close(it->first);
    if (client)
      client->clear();
  }
}

void Server::setup() {
  int yes = 1;
  int server_fd;

  for (std::vector<ServerConfig>::iterator it = servers_.begin(); it != servers_.end(); it++) {
    for (std::vector<Listen>::iterator list = it->getListens().begin(); list != it->getListens().end(); list++) {
      if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        throw std::runtime_error("Cannot create listening socket");

      fcntl(server_fd, F_SETFL, O_NONBLOCK);

      struct sockaddr_in address;
      ft::memset(&address, 0, sizeof(address));
      address.sin_family = AF_INET;
      address.sin_addr.s_addr = inet_addr(list->ip_.c_str());
      address.sin_port = htons(list->port_);

      std::cout << "setup server " << server_fd << " on " << list->ip_ << ":" << list->port_ << std::endl;

      running_server_[server_fd] = Listen(list->ip_, list->port_);

      setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

      if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
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

void Server::newConnection(int fd) {
  struct sockaddr_storage their_addr;
  socklen_t addr_size = sizeof(their_addr);

  int clientFd = accept(fd, (struct sockaddr *)&their_addr, &addr_size);
  fcntl(clientFd, F_SETFL, O_NONBLOCK);

  std::cout << "[Server] New client " << clientFd << " on " << running_server_[fd].ip_ << ":" << running_server_[fd].port_ << std::endl;

  clients_[clientFd] = new Client(ft::inet_ntop(ft::get_in_addr((struct sockaddr *)&their_addr)), running_server_[fd].ip_);

  FD_CLR(fd, &read_fds_);

  if (clientFd == -1)
    strerror(errno);
  else {
    FD_SET(clientFd, &master_fds_); // add to master set
    if (clientFd > max_fd_)   // keep track of the max
      max_fd_ = clientFd;
  }
}

void Server::readData(int fd) {
  int nbytes = 0;
  char buf[BUF_SIZE + 1];

  if ((nbytes = recv(fd, buf, BUF_SIZE, 0)) <= 0) {
    if (nbytes == 0) {
      std::cout << "[Server] Connection closed (socket " << fd << ")." << std::endl;
    } else {
      strerror(errno);
    }
    close(fd); // bye!

    delete clients_[fd];
    clients_.erase(fd);

    FD_CLR(fd, &master_fds_); // remove from master set
    FD_CLR(fd, &read_fds_);
    return ;
  }

  FD_CLR(fd, &read_fds_);
  #ifdef DEBUG
  // std::cout << "[Server] Receiving data from " << clients_[fd] << std::endl;
  #endif

  std::string buffer(buf, nbytes);

  Client *client = clients_[fd];
  Request *req = client->getRequest();

  if (!req) {
    client->createRequest();
    req = client->getRequest();
  }

  int ret = req->parse(buffer);

  if (ret == 1) {
    if (req)
      req->print();
    client->setupResponse(servers_);
  }
}

void Server::writeData(int fd) {
  Client *client = clients_[fd];

  if (!client)
    return ;

  Response *response = client->getResponse();

  if (response) {
    if (!response->send(fd))
      client->clear();
  }
  FD_CLR(fd, &write_fds_);
}

void Server::run() {
  signal(SIGINT, interruptHandler);
  running_ = true;
  std::cout << "[Server] Starting." << std::endl;

  while (running_) {
    read_fds_ = master_fds_;
    write_fds_ = master_fds_;

    if (select(max_fd_ + 1, &read_fds_, &write_fds_, NULL, NULL) == -1) {
      strerror(errno);
      break ;
    }

    for (std::map<int, Listen>::iterator it = running_server_.begin(); it != running_server_.end(); it++) {
      if (FD_ISSET(it->first, &read_fds_))
        newConnection(it->first);
    }

    for (int fd = 0; fd <= max_fd_; fd++) {
      if (FD_ISSET(fd, &read_fds_)) {
        readData(fd);
      }
      if (FD_ISSET(fd, &write_fds_)) {
        writeData(fd);
      }
    }
  }
}