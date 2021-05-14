#include "Server.hpp"

bool Server::running_ = false;

static void interruptHandler(int sig_int) {
  (void)sig_int;
	std::cout << "\b\b \b\b";
  std::cout << "[Server] Shutdown." << std::endl;
	Server::running_ = false;
}

Server::Server(std::vector<ServerConfig> &servers) : servers_(servers) {
  FD_ZERO(&master_fds_);
  FD_ZERO(&read_fds_);
  FD_ZERO(&write_fds_);
  setup();
}

Server::~Server() {
  for (std::map<int, Client*>::iterator it = clients_.begin(); it != clients_.end(); it++)
    it->second->clear();
}

void Server::setup() {
  int yes = 1;
  int server_fd = 0;

  std::vector<Listen> is_bind;

  for (std::vector<ServerConfig>::iterator it = servers_.begin(); it != servers_.end(); it++) {
    if (it->getListens().empty())
      it->getListens().push_back(Listen("0.0.0.0", 80));
    for (std::vector<Listen>::iterator list = it->getListens().begin(); list != it->getListens().end(); list++) {
      if (std::find(is_bind.begin(), is_bind.end(), *list) == is_bind.end()) {
        if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
          throw webserv_exception("socket() failed", errno);

        fcntl(server_fd, F_SETFL, O_NONBLOCK);

        struct sockaddr_in address;
        memset(&address, 0, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = inet_addr(list->ip_.c_str());
        address.sin_port = htons(list->port_);

        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
          throw webserv_exception("bind() to %s failed", errno, list->ip_ + ":" + ft::to_string(list->port_));

        if (listen(server_fd, MAX_CONNECTION) < 0)
          throw webserv_exception("listen() failed", errno);

        running_server_[server_fd] = Listen(list->ip_, list->port_);

        std::cout << "[Webserv] Setup server " << server_fd << " on " << list->ip_ << ":" << list->port_ << std::endl;

        FD_SET(server_fd, &master_fds_);

        max_fd_ = server_fd;
        is_bind.push_back(*list);
      }
    }
  }

  if (is_bind.empty())
    throw webserv_exception("listen() failed", errno);
}

void Server::newConnection(int fd) {
  struct sockaddr_storage their_addr;
  socklen_t addr_size = sizeof(their_addr);

  FD_CLR(fd, &read_fds_);

  int clientFd = accept(fd, (struct sockaddr *)&their_addr, &addr_size);

  if (clientFd == -1) {
    std::cerr << "accept : " << strerror(errno) << std::endl;
    return ;
  }

  std::cout << "[Server] New client " << clientFd << " on " << running_server_[fd].ip_ << ":" << running_server_[fd].port_ << std::endl;
  fcntl(clientFd, F_SETFL, O_NONBLOCK);

  std::string client_addr = ft::inet_ntop(ft::get_in_addr((struct sockaddr *)&their_addr));
  clients_[clientFd] = new Client(clientFd, client_addr, running_server_[fd], clients_.size() >= MAX_CLIENT);

  FD_SET(clientFd, &master_fds_);
  if (clientFd > max_fd_)
    max_fd_ = clientFd;
}

void Server::clientDisconnect(int fd) {
  std::cout << "[Server] Connection closed (" << fd << ")" << std::endl;

  FD_CLR(fd, &master_fds_);

  if (max_fd_ == fd) {
    std::map<int, Client*>::iterator it = clients_.find(fd);

    if (clients_.size() > 1) {
      it--;
      max_fd_ = it->first;
    } else {
      max_fd_ = max_fd_tmp_;
    }
  }

  delete clients_[fd];
  clients_.erase(fd);

  FD_CLR(fd, &master_fds_);

  std::cout << "Client successfully disconnected" << std::endl;
}

int Server::readData(int fd) {
  Request *req = clients_[fd]->getRequest();

  if (!req)
    req = clients_[fd]->getRequest(true);

  char buf[BUF_SIZE];

  FD_CLR(fd, &read_fds_);

  int nbytes = recv(fd, buf, BUF_SIZE, 0);

  if (nbytes == 0)
    return -1;

  if (nbytes < 0) {
    std::cerr << "recv : " << strerror(errno) << std::endl;
    return -1;
  }

  std::string buffer(buf, nbytes);
  int ret = req->parse(buffer);

  if (ret >= 1) {
    // #ifdef DEBUG
      req->print();
    // #endif
    // std::cout << "HANDLE CLEARED" << std::endl;
    clients_[fd]->setupResponse(servers_, ret);
  }
  return 0;
}

void Server::writeData(int fd) {
  FD_CLR(fd, &write_fds_);

  Response *response = clients_[fd]->getResponse();

  if (response) {
    int ret = response->send(fd);

    if (ret < 0) {
      clientDisconnect(fd);
      return;
    } else if (ret == 0) {
      clients_[fd]->clear();
    }
  }
}

void Server::run() {
  int ret = 0;
  signal(SIGINT, interruptHandler);
  running_ = true;
  std::cout << "[Server] Starting." << std::endl;
  max_fd_tmp_ = max_fd_;
  while (running_) {
    read_fds_ = master_fds_;
    write_fds_ = master_fds_;

    ret = select(max_fd_ + 1, &read_fds_, &write_fds_, NULL, NULL);

    if (ret > 0) {
      for (std::map<int, Listen>::iterator it = running_server_.begin(); it != running_server_.end(); it++) {
        if (FD_ISSET(it->first, &read_fds_))
          newConnection(it->first);
      }

      std::map<int, Client*>::iterator it = clients_.begin();

      while (it != clients_.end()) {
        if (FD_ISSET(it->first, &read_fds_) && readData(it->first) == -1) {
          clientDisconnect(it->first);
          it = clients_.begin();
          continue;
        }

        if (it->second->timeout())
          it->second->setupResponse(servers_, 408);

        if (it->second->disconnect())
          it->second->setupResponse(servers_, 503);

        if (FD_ISSET(it->first, &write_fds_))
          writeData(it->first);

        it++;
      }
    } else if (ret == -1)
      std::cerr << "select : " << strerror(errno) << std::endl;
  }
}