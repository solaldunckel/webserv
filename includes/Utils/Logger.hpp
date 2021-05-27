#ifndef LOGGER_HPP
# define LOGGER_HPP

# include <iostream>
# include <map>
# include <string>

# define RESET   "\033[0m"
# define BLACK   "\033[30m"
# define RED     "\033[31m"
# define GREEN   "\033[32m"
# define YELLOW  "\033[33m"
# define BLUE    "\033[34m"
# define MAGENTA "\033[35m"
# define CYAN    "\033[36m"
# define WHITE   "\033[37m"
# define BOLDBLACK   "\033[1m\033[30m"
# define BOLDRED     "\033[1m\033[31m"
# define BOLDGREEN   "\033[1m\033[32m"
# define BOLDYELLOW  "\033[1m\033[33m"
# define BOLDBLUE    "\033[1m\033[34m"
# define BOLDMAGENTA "\033[1m\033[35m"
# define BOLDCYAN    "\033[1m\033[36m"
# define BOLDWHITE   "\033[1m\033[37m"

enum LogLevel {
  NONE,
  INFO,
  DEBUG
};

extern pthread_mutex_t g_write;

class Logger {
 public:
  Logger();

  void setLogLevel(LogLevel level);
  LogLevel getLogLevel();
  void print(LogLevel level, std::string str, std::string color = YELLOW, bool error = false);

 private:
  LogLevel level_;
};

#endif