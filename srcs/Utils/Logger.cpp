# include "Logger.hpp"

Logger::Logger() {
};

void Logger::setLogLevel(LogLevel level) {
  level_ = level;
}

LogLevel Logger::getLogLevel() {
  return level_;
}

void Logger::print(LogLevel level, std::string str, std::string color, bool error) {
  (void)error;
  if (level <= level_) {
    struct tm	*tm;
    time_t rawtime;
    char buf[32];

    time(&rawtime);
    tm = localtime (&rawtime);
    int ret = strftime(buf, 32, "%T", tm);
    buf[ret] = '\0';

    #ifdef BONUS
    pthread_mutex_lock(&g_write);
    #endif

    if (error) {
      std::cerr << CYAN << "[" << buf << "] " << RESET;
      std::cerr << color << str << RESET;
      std::cerr << "\n";
    } else {
      std::cout << CYAN << "[" << buf << "] " << RESET;
      std::cout << color << str << RESET;
      std::cout << "\n";
    }
    #ifdef BONUS
    pthread_mutex_unlock(&g_write);
    #endif
  }
};