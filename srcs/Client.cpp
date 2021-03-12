#include "Client.hpp"


Client::Client(std::string addr, std::string &host) : addr_(addr), host_(host), request_(nullptr), config_(nullptr), response_(nullptr) {
}

Client::~Client() {
  clear();
}

void Client::createRequest() {
  request_ = new Request();
}

void Client::setupResponse(std::vector<ServerConfig> &servers) {
  config_ = new RequestConfig(*request_, addr_, servers);

  config_->setup();

  response_ = new Response(*config_);

  response_->build();

  if (request_) {
    delete request_;
    request_ = nullptr;
  }
}

void Client::clear() {
  if (request_) {
    delete request_;
    request_ = nullptr;
  }
  if (config_) {
    delete config_;
    config_ = nullptr;
  }
  if (response_) {
    delete response_;
    response_ = nullptr;
  }
}

Request *Client::getRequest() {
  return request_;
}

RequestConfig *Client::getConfig() {
  return config_;
}

Response *Client::getResponse() {
  return response_;
}