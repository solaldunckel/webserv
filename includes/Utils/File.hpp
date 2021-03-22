#ifndef FILE_HPP
# define FILE_HPP

# include <iostream>

# include <fcntl.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <dirent.h>
# include <time.h>
# include <string.h>
# include <errno.h>

# include <algorithm>
# include <vector>

# include "StringUtils.hpp"

struct auto_listing {
  std::string name_;
  bool is_dir_;
  std::string date_;
  size_t size_;

  auto_listing() : is_dir_(false) {};
};

bool sort_auto_listing(auto_listing i, auto_listing j);

class File {
 public:
  File();
  File(std::string path);
  ~File();

  bool open(bool create = false);
  void close();
  void create(std::string &body);
  void unlink();
  void set_path(std::string path);
  bool is_directory();
  std::string last_modified();
  bool exists();

  std::string find_index(std::vector<std::string> &indexes);

  std::string autoIndex(std::string &target);
  std::string getExtension();
  std::string getContent();
  std::string &getPath();
  int &getFd();

 private:
  int fd_;
  std::string path_;
};

#endif
