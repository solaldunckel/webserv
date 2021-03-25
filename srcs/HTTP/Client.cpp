#include "Client.hpp"

Client::Client(int fd, std::string &addr, Listen &host_port) : fd_(fd), addr_(addr), host_port_(host_port) {
  request_ = NULL;
  config_ = NULL;
  response_ = NULL;
}

Client::~Client() {
  close(fd_);
  clear();
}

void Client::clear() {
  ft::delete_(request_);
  ft::delete_(config_);
  ft::delete_(response_);
}

void Client::setupConfig(std::vector<ServerConfig> &servers) {
  config_ = new RequestConfig(*request_, host_port_, servers, *this);
}

void Client::setupResponse(std::vector<ServerConfig> &servers, int error_code) {
  if (!config_)
    setupConfig(servers);

  response_ = new Response(*config_, error_code);

  ft::delete_(request_);
}

bool Client::timeout() {
  if (request_) {
    if (ft::get_current_time_in_sec() - request_->get_timer_in_sec() > TIMEOUT) {
      if (request_->timeout())
        return true;
    }
  }
  return false;
}

int Client::getFd() {
  return fd_;
}

std::string &Client::getAddr() {
  return addr_;
}

Request *Client::getRequest(bool force) {
  if (!request_ && force)
    request_ = new Request();
  return request_;
}

RequestConfig *Client::getConfig() {
  return config_;
}

Response *Client::getResponse() {
  return response_;
}