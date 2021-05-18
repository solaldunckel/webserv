#include "Server.hpp"

bool Server::running_ = false;

static void interruptHandler(int sig_int) {
  (void)sig_int;
	Server::running_ = false;
}

Server::Server(std::vector<ServerConfig> &servers, InputArgs &options) : servers_(servers), options_(options) {
  FD_ZERO(&master_fds_);
  FD_ZERO(&read_fds_);
  FD_ZERO(&write_fds_);

  setup();
}

Server::~Server() {
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
          throw webserv_exception("bind() to % failed", errno, list->ip_ + ":" + ft::to_string(list->port_));

        if (listen(server_fd, MAX_CONNECTION) < 0)
          throw webserv_exception("listen() failed", errno);

        running_server_[server_fd] = Listen(list->ip_, list->port_);

        std::cout << "[Server] Listening on " << list->ip_ << ":" << list->port_ << std::endl;

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

  if (clientFd == -1)
    return ;

  print("New connection on " + ft::to_string(running_server_[fd].ip_) + ":" + ft::to_string(running_server_[fd].port_));

  fcntl(clientFd, F_SETFL, O_NONBLOCK);

  std::string client_addr = ft::inet_ntop(ft::get_in_addr((struct sockaddr *)&their_addr));
  clients_[clientFd] = new Client(clientFd, client_addr, running_server_[fd], worker_id_, clients_.size() >= MAX_CLIENT);

  FD_SET(clientFd, &master_fds_);
  if (clientFd > max_fd_)
    max_fd_ = clientFd;
}

void Server::closeClient(int fd) {
  if (clients_.find(fd) != clients_.end()) {
    print("Connection closed");
    delete clients_[fd];
    clients_.erase(fd);
  }
}

void Server::clientDisconnect(int fd) {
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

  closeClient(fd);

  FD_CLR(fd, &master_fds_);
}

int Server::readData(int fd) {
  Request *req = clients_[fd]->getRequest();

  if (!req)
    req = clients_[fd]->getRequest(true);

  char buf[BUF_SIZE];

  FD_CLR(fd, &read_fds_);

  int nbytes = recv(fd, buf, BUF_SIZE, 0);

  if (nbytes <= 0)
    return -1;

  std::string buffer(buf, nbytes);
  int ret = req->parse(buffer);

  if (ret >= 1) {
    if (options_.verbose())
      req->print();
    clients_[fd]->setupResponse(servers_, options_, ret);
    if (options_.verbose())
      clients_[fd]->getResponse()->print();
  }
  return 0;
}

bool Server::writeData(int fd) {
  FD_CLR(fd, &write_fds_);

  Response *response = clients_[fd]->getResponse();

  if (response) {
    int ret = response->send(fd);

    if (ret < 0) {
      clientDisconnect(fd);
      return false;
    } else if (ret == 0) {
      bool disconnect = response->shouldDisconnect() || clients_[fd]->disconnect();
      clients_[fd]->clear();
      if (disconnect) {
        clientDisconnect(fd);
        return false;
      }
    }
  }
  return true;
}

void Server::run(int worker_id, sem_t *sem) {
  worker_id_ = worker_id;
  (void)sem;
  int ret = 0;

  #ifdef BONUS
  signal(SIGINT, interruptHandler);
  #else
  signal(SIGINT, interruptHandler);
  #endif

  running_ = true;
  print("Starting");

  max_fd_tmp_ = max_fd_;

  while (running_) {
    read_fds_ = master_fds_;
    write_fds_ = master_fds_;

    ret = select(max_fd_ + 1, &read_fds_, &write_fds_, NULL, NULL);

    if (ret >= 0) {
      for (std::map<int, Listen>::iterator it = running_server_.begin(); it != running_server_.end(); it++) {
        if (FD_ISSET(it->first, &read_fds_)) {
          newConnection(it->first);
          #ifdef BONUS
            usleep(1000);
            break ;
          #endif
        }
      }

      std::map<int, Client*>::iterator it = clients_.begin();

      while (it != clients_.end()) {
        if (FD_ISSET(it->first, &read_fds_) && readData(it->first) == -1) {
          clientDisconnect(it->first);
          it = clients_.begin();
          continue;
        }

        if (it->second->timeout()) {
          it->second->setupResponse(servers_, options_, 408);
          if (options_.verbose())
            it->second->getResponse()->print();
        }

        if (it->second->disconnect()) {
          it->second->setupResponse(servers_, options_, 503);
          if (options_.verbose())
            it->second->getResponse()->print();
        }

        if (FD_ISSET(it->first, &write_fds_)) {
          if (!writeData(it->first)) {
            it = clients_.begin();
            continue;
          }
        }

        it++;
      }
    } else if (ret == -1 && running_)
      std::cerr << "select : " << strerror(errno) << std::endl;
    usleep(500);
  }

  std::map<int, Client*>::iterator it = clients_.begin();

  while (it != clients_.end()) {
    std::map<int, Client*>::iterator tmp = it++;
    closeClient(tmp->first);
  }

  print("Shutdown");

  #ifdef BONUS
  if (worker_id_)
    exit(0);
  #endif
}

void Server::print(std::string str) {
  if (worker_id_ > 0)
    std::cout << "[Worker: " << worker_id_ << "] ";
  else
    std::cout << "[Server] ";
  std::cout << str << std::endl;
}