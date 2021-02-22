cmake_minimum_required(VERSION 3.10)

# set the project name
project(webserv)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SRCS
  srcs/Config.cpp
  srcs/File.cpp
  srcs/LocationConfig.cpp
  srcs/main.cpp
  srcs/MimeTypes.cpp
  srcs/Request.cpp
  srcs/Response.cpp
  srcs/Server.cpp
  srcs/ServerConfig.cpp
  srcs/Utils.cpp
)

add_compile_options(-Wall -Werror -Wextra)

# add the executable
add_executable(webserv ${SRCS})

# add includes
target_include_directories(webserv PUBLIC includes)