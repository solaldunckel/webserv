cmake_minimum_required(VERSION 3.10)

# set the project name
project(webserv)

# set variables
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SRCS
  srcs/main.cpp
  srcs/Config.cpp
  srcs/Client.cpp
  srcs/CGI.cpp
  srcs/File.cpp
  srcs/StatusCode.cpp
  srcs/MimeTypes.cpp
  srcs/Request.cpp
  srcs/RequestConfig.cpp
  srcs/Response.cpp
  srcs/Server.cpp
  srcs/ServerConfig.cpp
  srcs/Utils.cpp
)

add_compile_options(-O1 -Wall -Werror -Wextra)

option(DEBUG "PRINT DEBUG MESSAGES" OFF)

# custom fclean target
add_custom_target(fclean
  COMMAND rm -rf CMakeCache.txt
  COMMAND rm -rf CMakeFiles/
  COMMAND rm -rf cmake_install.cmake
  COMMAND rm -rf Makefile
  COMMAND rm -rf webserv
)

# add the executable
add_executable(webserv ${SRCS})
target_include_directories(webserv PUBLIC includes)

if (DEBUG)
  target_compile_definitions(webserv PUBLIC -DDEBUG)
endif()