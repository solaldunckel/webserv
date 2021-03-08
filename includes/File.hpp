#ifndef FILE_HPP
# define FILE_HPP

# include <iostream>
# include <fstream>

# include <fcntl.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <dirent.h>
# include <time.h>

# include <vector>

class File {
 public:
  // Constructors & Deconstructors
  File();
  File(std::string path);
  ~File();

  void open();
  void create(std::string &body);
  void unlink();
  void set_path(std::string path);
  bool is_open();
  bool is_directory();
  std::string last_modified();
  bool exists();

  std::string find_index(std::vector<std::string> indexes);

  std::string getExtension();
  std::string getContent();
  std::string &getPath();

 private:
  std::fstream stream_;
  std::string path_;
};

#endif
