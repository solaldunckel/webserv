#ifndef FILE_HPP
# define FILE_HPP

# include <iostream>
# include <fstream>

# include <fcntl.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <dirent.h>

# include <vector>

class File {
 public:
  // Constructors & Deconstructors
  File();
  File(const std::string &path);
  ~File();

  void open(std::string path);
  void create(std::string path, std::string &body);
  void set_path(std::string &path);
  bool is_open();
  bool is_directory(std::string &path);

  bool exists(std::string &path);

  std::string find_index(std::string path, std::vector<std::string> indexes);

  std::string getExtension();
  std::string getContent();

 private:
  std::fstream  stream_;
  std::string   name_;
  std::string   full_path_;
};

#endif
