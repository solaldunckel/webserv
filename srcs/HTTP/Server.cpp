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
  for (std::map<int, Client*>::iterator it = clients_.begin(); it != clients_.end(); it++) {
    it->second->clear();
  }
}

void Server::setup() {
  int yes = 1;
  int server_fd;

  std::vector<Listen> is_bind;

  for (std::vector<ServerConfig>::iterator it = servers_.begin(); it != servers_.end(); it++) {
    for (std::vector<Listen>::iterator list = it->getListens().begin(); list != it->getListens().end(); list++) {
      if (std::find(is_bind.begin(), is_bind.end(), *list) == is_bind.end()) {
        if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
          throw webserv_exception("socket() failed", errno);

        fcntl(server_fd, F_SETFL, O_NONBLOCK);

        struct sockaddr_in address;
        ft::memset(&address, 0, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = inet_addr(list->ip_.c_str());
        address.sin_port = htons(list->port_);

        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
          throw webserv_exception("bind() to %s failed", errno, list->ip_ + ":" + std::to_string(list->port_));

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
}

void Server::newConnection(int fd) {
  struct sockaddr_storage their_addr;
  socklen_t addr_size = sizeof(their_addr);

  int clientFd = accept(fd, (struct sockaddr *)&their_addr, &addr_size);

  FD_CLR(fd, &read_fds_);

  if (clientFd == -1)
    strerror(errno);
  else {
    std::cout << "[Server] New client " << clientFd << " on " << running_server_[fd].ip_ << ":" << running_server_[fd].port_ << std::endl;
    fcntl(clientFd, F_SETFL, O_NONBLOCK);

    std::string client_addr = ft::inet_ntop(ft::get_in_addr((struct sockaddr *)&their_addr));
    clients_[clientFd] = new Client(clientFd, client_addr, running_server_[fd]);

    FD_SET(clientFd, &master_fds_);
    if (clientFd > max_fd_)
      max_fd_ = clientFd;
  }
}

void Server::clientDisconnect(int fd, int nbytes) {
  if (nbytes == 0)
    std::cout << "[Server] Connection closed (" << fd << ")" << std::endl;
  else
    strerror(fd);

  delete clients_[fd];
  clients_.erase(fd);

  FD_CLR(fd, &master_fds_);
}

int Server::readData(int fd, std::string &buffer) {
  Request *req = clients_[fd]->getRequest(true);

  int ret = req->parse(buffer);

  if (ret >= 1) {
    // #ifdef DEBUG
      // req->print();
    // #endif
    clients_[fd]->setupResponse(servers_, ret);
  }
  return 1;
}

void Server::writeData(int fd) {
  if (!clients_.count(fd))
    return;

  Response *response = clients_[fd]->getResponse();

  if (response && !response->send(fd))
    clients_[fd]->clear();
}

void Server::run() {
  signal(SIGINT, interruptHandler);
  running_ = true;
  std::cout << "[Server] Starting." << std::endl;

  while (running_) {
    read_fds_ = master_fds_;
    write_fds_ = master_fds_;

    if (select(max_fd_ + 1, &read_fds_, &write_fds_, NULL, NULL) == -1)
      strerror(errno);

    for (std::map<int, Listen>::iterator it = running_server_.begin(); it != running_server_.end(); it++) {
      if (FD_ISSET(it->first, &read_fds_))
        newConnection(it->first);
    }

    for (int fd = 0; fd <= max_fd_; fd++) {
      if (FD_ISSET(fd, &read_fds_)) {
        char buf[BUF_SIZE];
        int nbytes = recv(fd, buf, BUF_SIZE, 0);

        if (nbytes <= 0) {
          clientDisconnect(fd, nbytes);
        } else {
          std::string buffer(buf, nbytes);
          readData(fd, buffer);
        }
      }
      if (FD_ISSET(fd, &write_fds_))
        writeData(fd);
    }

    for (std::map<int, Client*>::iterator it = clients_.begin(); it != clients_.end(); it++) {
      if (it->second->timeout())
        it->second->setupResponse(servers_, 408);
    }
  }
}