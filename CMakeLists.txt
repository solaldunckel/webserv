cmake_minimum_required(VERSION 3.10)

# set the project name
project(webserv)

# set variables
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SRCS
  srcs/main.cpp
  srcs/Config/Config.cpp
  srcs/Config/ServerConfig.cpp
  srcs/Config/RequestConfig.cpp
  srcs/HTTP/CGI.cpp
  srcs/HTTP/Client.cpp
  srcs/HTTP/Request.cpp
  srcs/HTTP/Response.cpp
  srcs/HTTP/Server.cpp
  srcs/Utils/Base64.cpp
  srcs/Utils/File.cpp
  srcs/Utils/MimeTypes.cpp
  srcs/Utils/StatusCode.cpp
  srcs/Utils/StringUtils.cpp
  srcs/Utils/Utils.cpp
)

set(INCLUDES
  includes/Config
  includes/HTTP
  includes/Utils
)

add_compile_options(-Wall -Werror -Wextra -std=c++98)

option(DEBUG "PRINT DEBUG MESSAGES" OFF)
option(BONUS "COMPILE WITH BONUS" OFF)

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
target_include_directories(webserv PUBLIC ${INCLUDES})

if (DEBUG)
  target_compile_definitions(webserv PUBLIC -DDEBUG)
endif()