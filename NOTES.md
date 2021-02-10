# PARSING

## functions

  ### socket
    int socket(int domain, int type, int protocol);
    domain = IPv4 or IPv6 -> PF_INET or PF_INET6
    type = TCP or UDP -> SOCK_STREAM or SOCK_DGRAM
    protocol = 0 to use proper protocol

  ### accept
    int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    returns a new socket for the connection. fills addr and addrlen with client info

## directives

  ### listen [server]
    listen address[:port];
    listen port;

  ### server_name [server]
    server_name name ...;

  ### root [*server, location]
    root path;

  ### index [*server, location]
    index file ...;

  ### error_page [server, *location]
    error_page code ... uri;

  ### client_max_body_size [server, *location]
    client_max_body_size size;

  ### autoindex
    autoindex on | off;

  ### location [server, *location]
    location uri;

  ### limit_except [location]
    limit_except method ...;

  ### cgi [location]
    cgi extension file;