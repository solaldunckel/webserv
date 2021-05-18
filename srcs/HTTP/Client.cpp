#include "Client.hpp"

Client::Client(int fd, std::string &addr, Listen &host_port, int worker_id, bool disconnect) : fd_(fd), addr_(addr), host_port_(host_port), worker_id_(worker_id), disconnect_(disconnect) {
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

void Client::setupResponse(std::vector<ServerConfig> &servers, InputArgs &options, int error_code) {
  if (!request_)
    request_ = getRequest(true);

  if (!config_)
    setupConfig(servers);

  config_->setup(options);

  response_ = new Response(*config_, worker_id_, error_code);

  int loop = 0;

  for (int ret = 1; ret != 0; loop++) {
    ret = 0;

    response_->build();

    if (response_->redirect()) {
      ret = 1;
      config_->redirectLocation(response_->redirect_target());
      response_->clear();
    }
    if (loop >= 10) {
      ft::delete_(response_);
      response_ = new Response(*config_, worker_id_, 500);
      response_->build();
      break ;
    }
  }

  ft::delete_(request_);
}

bool Client::timeout() {
  if (request_) {
    time_t current_time = ft::get_current_time_in_sec();
    if (current_time - request_->get_start_timer_in_sec() > START_TIMEOUT
      || current_time - request_->get_last_timer_in_sec() > LAST_TIMEOUT) {
      if (request_->timeout())
        return true;
    }
  }
  return false;
}

bool Client::disconnect() {
  if (disconnect_)
    return true;
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