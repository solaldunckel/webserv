#include "Server.hpp"

bool Server::running_ = false;

StatusCode g_status;
MimeTypes g_mimes;

static void interruptHandler(int sig_int) {
  (void)sig_int;
  std::cout << "\b\b \b\b";
	Server::running_ = false;
}

Server::Server(const Server &copy) : servers_(copy.servers_), options_(copy.options_) {
  *this = copy;
}

Server::Server(std::vector<ServerConfig> &servers, InputArgs &options) : servers_(servers), options_(options), max_fd_(0) {
  FD_ZERO(&master_fds_);
  FD_ZERO(&read_fds_);
  FD_ZERO(&write_fds_);
}

Server	&Server::operator=(const Server &copy) {
  servers_ = copy.servers_;
  options_ = copy.options_;
  running_server_ = copy.running_server_;
  clients_ = copy.clients_;
  master_fds_ = copy.master_fds_;
  fd_set_ = copy.fd_set_;
  max_fd_ = copy.max_fd_;
  return (*this);
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

        Log.print(INFO, "listening on " + ft::to_string(list->ip_) + ":" + ft::to_string(list->port_), GREEN);

        add_to_fd_set(server_fd);
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

  Log.print(INFO, head_ + "new connection on " + ft::to_string(running_server_[fd].ip_) + ":" + ft::to_string(running_server_[fd].port_), GREEN);

  fcntl(clientFd, F_SETFL, O_NONBLOCK);

  std::string client_addr = ft::inet_ntop(ft::get_in_addr((struct sockaddr *)&their_addr));
  clients_[clientFd] = new Client(clientFd, client_addr, running_server_[fd], worker_id_, clients_.size() >= MAX_CLIENT);

  add_to_fd_set(clientFd);
}

void Server::clientDisconnect(int fd) {
  remove_from_fd_set(fd);
  if (clients_.find(fd) != clients_.end()) {
    Log.print(INFO, head_ + "connection closed", GREEN);
    delete clients_[fd];
    clients_.erase(fd);
  }
}

bool Server::recv(int fd) {
  Request *req = clients_[fd]->getRequest();

  if (!req)
    req = clients_[fd]->getRequest(true);

  char buf[BUF_SIZE];

  FD_CLR(fd, &read_fds_);

  int nbytes = ::recv(fd, buf, BUF_SIZE, 0);

  if (nbytes <= 0)
    return false;

  std::string buffer(buf, nbytes);

  int ret = req->parse(buffer);

  if (ret >= 1) {
    clients_[fd]->setupConfig(servers_, options_);
    Log.print(INFO, head_ + "<< " + clients_[fd]->getConfig()->log(Log.getLogLevel()));
    clients_[fd]->setupResponse(servers_, options_, ret);
  }
  return true;
}

bool Server::send(int fd) {
  FD_CLR(fd, &write_fds_);

  Response *response = clients_[fd]->getResponse();

  if (response) {
    int ret = response->send(fd);

    if (ret < 0)
      return false;
    else if (ret == 0) {
      bool disconnect = response->shouldDisconnect() || clients_[fd]->disconnect();
      Log.print(INFO, head_ + ">> " + response->response_log(Log.getLogLevel()));
      clients_[fd]->clear();
      if (disconnect)
        return false;
    }
  }
  return true;
}

void Server::add_to_fd_set(int fd) {
  fd_set_.push_back(fd);
  fd_set_.sort();

  FD_SET(fd, &master_fds_);

  if (fd > max_fd_)
    max_fd_ = fd;
}

void Server::remove_from_fd_set(int fd) {
  for (std::list<int>::iterator it = fd_set_.begin(); it != fd_set_.end(); it++) {
    if (*it == fd) {
      fd_set_.erase(it);
      break ;
    }
  }

  FD_CLR(fd, &master_fds_);

  if (fd == max_fd_)
    max_fd_ = *fd_set_.rbegin();
}

void Server::check_timeout_disconnect(Client *client) {
  if (client->timeout()) {
    client->setupResponse(servers_, options_, 408);
  }

  if (client->disconnect()) {
    client->setupResponse(servers_, options_, 503);
  }
}

void Server::run(int worker_id) {
  worker_id_ = worker_id;

  head_ = "server : ";
  if (worker_id_ > 0)
    head_ = "worker[" + ft::to_string(worker_id_) + "] : ";

  if (!worker_id_ || worker_id_ == 1) {
    signal(SIGINT, interruptHandler);
    signal(SIGQUIT, interruptHandler);
  }

  running_ = true;
  Log.print(INFO, head_ + "starting", GREEN);

  struct timeval timeout;

  timeout.tv_sec = 1;
  timeout.tv_usec = 0;

  while (running_) {
    read_fds_ = master_fds_;
    write_fds_ = master_fds_;

    int ret = select(max_fd_ + 1, &read_fds_, &write_fds_, NULL, &timeout);

    if (ret >= 0) {
      for (std::map<int, Listen>::iterator it = running_server_.begin(); it != running_server_.end(); it++) {
        if (FD_ISSET(it->first, &read_fds_)) {
          pthread_mutex_lock(&g_accept);
          newConnection(it->first);
          pthread_mutex_unlock(&g_accept);
        }
      }

      std::map<int, Client*>::iterator next_it;

      for (std::map<int, Client*>::iterator it = clients_.begin(), next_it = it; it != clients_.end(); it = next_it) {
        int client_fd = it->first;

        next_it++;

        if (FD_ISSET(client_fd, &read_fds_) && !recv(client_fd)) {
          clientDisconnect(client_fd);
          continue;
        }

        check_timeout_disconnect(it->second);

        if (FD_ISSET(client_fd, &write_fds_) && !send(client_fd)) {
          clientDisconnect(client_fd);
          continue;
        }
      }
    } else if (ret == -1 && running_)
      throw webserv_exception("select() failed", errno);
    usleep(500);
  }

  std::map<int, Client*>::iterator next_it;

  for (std::map<int, Client*>::iterator it = clients_.begin(), next_it = it; it != clients_.end(); it = next_it) {
    next_it++;
    clientDisconnect(it->first);
  }

  Log.print(INFO, head_ + "shutdown", GREEN);
}