cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

# set the project name
project(webserv)

option(DEBUG "PRINT DEBUG MESSAGES" OFF)

if (DEBUG)
  add_definitions(-DDEBUG)
endif()

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add_custom_target(fclean
#   COMMAND rm -rf CMakeCache.txt
#   COMMAND rm -rf CMakeFiles/
#   COMMAND rm -rf cmake_install.cmake
#   COMMAND rm -rf Makefile
# )

set(SRCS
  srcs/main.cpp
  srcs/Config.cpp
  srcs/File.cpp
  srcs/MimeTypes.cpp
  srcs/Request.cpp
  srcs/RequestConfig.cpp
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