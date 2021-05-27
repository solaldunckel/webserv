# webserv

![webserv](https://i.imgur.com/7rVOORs.png)

## Description

webserv is a HTTP/1.1 server written in C++98. It must be conditionnal compliant with RFC 7230 to 7235.

## Usage

```shell
# Compile the sources
make
```
```shell
# Run the server
./webserv [options] [config_file]
```
#### Options
- ```-h, --help```  : Display help text
- ```-t, --test```  : Test the config file, dump it and exit
- ```-l, --log [LEVEL]```  : Set the log level (between 0 and 2)
- ```-u, --uri```  : Keep location uri on rooting (similar to nginx)

```
$ ./webserv -h

Usage: webserv [options] [config_file]

Options:
  -h, --help         : this help text
  -l, --log [LEVEL]  : set log level (between 0 and 2)
  -t, --test         : test config and exit
  -u, --uri          : keep location uri on rooting (similar to nginx)
```

## Configuration File

### Directives

#### workers : ```workers number;```

sets a number of workers thread

#### location : ```location [ = | ~ | ~* | ^~ ] uri;```

sets configuration depending on the given uri.

#### listen : ```listen address[:port];``` or ```listen port;```

bind the given address to the port. if no address is given, binds 0.0.0.0. if no port is given, binds 80.

#### server_name : ```server_name name ...;```

sets names of a virtual server.

#### root : ```root path;```

sets the directory for requests.

#### auth : ```auth login:password;```

restrict a route to a user.

#### error_page : ```error_page code ... uri;```

defines the URI that will be shown for the specified errors.

#### upload : ```upload directory;```

defines a directory to upload file.

#### autoindex : ```autoindex on | off;``` (default off)

enables or disables the directory listing output.

#### index : ```index file ...;```

defines files that will be used as an index.

#### cgi : ```cgi extension cgi_path;```

defines a CGI binary that will be executed for the given extension.

#### cgi-bin : ```cgi-bin folder_path;``` (default ./cgi-bin)

defines a folder where to search CGI binaries.

### Example

```
workers 4;

server {
  listen 80;

  root www;
  error_page 404 /my_errors/404.html;

  location / {
    index index.html;
  }
  
  location /data {
    cgi .php php-cgi;
  }

  location = /autoindex/ {
    root www/data;
    autoindex on;
  }
  
  location /my_errors {
    root www/errors;
  }
}
```

## Sources
- [Beej's Guide to Network Programming](http://beej.us/guide/bgnet/)
- [Server/Location matching](https://www.digitalocean.com/community/tutorials/understanding-nginx-server-and-location-block-selection-algorithms)
- [Configuration File](http://nginx.org/en/docs/dirindex.html)

## Testers
- [webserv_tester](https://github.com/acoudert/webserv_tester) by [@acoudert](https://github.com/acoudert)
- [webserv_tester](https://github.com/fredrikalindh/webserv_tester) by [@fredrikalindh](https://github.com/fredrikalindh)
- 42 tester :)

## Authors

* [solaldunckel](https://github.com/solaldunckel)
* [hguerni](https://github.com/hguerni)
