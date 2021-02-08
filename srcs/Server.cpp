#include "Server.hpp"

/*
** Constructors & Deconstructors
*/

Server::Server(std::string addr, int port) : addr_(addr), port_(port), addrlen_(sizeof(address_)) {
  memset((char *)&address_, 0, sizeof(address_));
}

Server::~Server() {
}

void Server::Setup() {
  if ((server_fd_ = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    std::cout << "CANNOT CREATE SOCKET" << std::endl;
    return ;
  }

  address_.sin_family = AF_INET;
  address_.sin_addr.s_addr = INADDR_ANY;
  address_.sin_port = htons(port_);

  if (bind(server_fd_, (struct sockaddr *)&address_, sizeof(address_)) < 0)
  {
      std::cout << "CANNOT BIND SOCKET" << std::endl;
      return ;
  }

  if (listen(server_fd_, -1) < 0)
  {
      std::cout << "IN LISTEN" << std::endl;
      exit(EXIT_FAILURE);
  }
}

void Server::Run() {
  std::string hello = "Hello from server";

  while (1) {

    /* Here we need to use select and check fds */
    int client;

    std::cout << " *** WAITING FOR NEW CONNECTION ***" << std::endl;

    if ((client = accept(server_fd_, (struct sockaddr *)&address_, (socklen_t*)&addrlen_)) < 0)
    {
        std::cout << "IN ACCEPT" << std::endl;
        exit(EXIT_FAILURE);
    }

    fcntl(client, F_SETFL, O_NONBLOCK);  // set it as non-blocking

    /* We read the request and process it */

    /* We write our response into the socket */

    write(client, hello.c_str(), hello.length());

    std::cout << " MESSAGE SENT" << std::endl;

    close(client);
  }
}
