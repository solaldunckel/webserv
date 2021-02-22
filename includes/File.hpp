#ifndef FILE_HPP
# define FILE_HPP

# include <iostream>
# include <fstream>

# include <fcntl.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <dirent.h>

class File {
 public:
  // Constructors & Deconstructors
  File();
  File(const std::string &path);
  ~File();

  void open(std::string path);
  void set_path(std::string &path);
  bool is_open();
  bool is_directory();

  std::string &getExtension();
  std::string getContent();

 private:
  int           fd_;
  int           status_;
  std::ifstream stream_;
  bool          is_dir_;
  std::string   name_;
  std::string   full_path_;
  std::string   body_;
  std::string   mime_;
  std::string   extension_;
};

#endif