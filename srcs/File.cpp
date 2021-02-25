#include "File.hpp"

/*
** Constructors & Deconstructors
*/

File::File() {}

File::File(const std::string &path) {
  stream_.open(path);
}

File::~File() {
  if (stream_.is_open())
    stream_.close();
}

void File::open(std::string path) {
  if (stream_.is_open())
    stream_.close();

  name_ = path;

  stream_.open(name_, std::ios::binary | std::ios::in);
}

void File::create(std::string path, std::string &body) {
  // int exists = 0;
  name_ = path;

  stream_.open(name_, std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
  stream_ << body;
}

std::string File::getContent() {
  char buf[512 + 1];
  std::string final;

  while (!stream_.eof()) {
    memset(buf, 0, 512);
    stream_.read(buf, 512);
    final += std::string(buf, stream_.gcount());
  }
  return final;
};

bool File::is_open() {
  if (stream_.is_open() && stream_.good())
    return true;
  return false;
}

std::string File::getExtension() {
  // extension_ = name_.substr(name_.find_last_of('.'));
  if (name_.length())
    return name_.substr(name_.find_last_of('.'));
  return "";
}

bool File::is_directory(std::string &path) {
  struct stat statbuf;
  stat(path.c_str(), &statbuf);
  return S_ISDIR(statbuf.st_mode);
}

bool File::exists(std::string &path) {
  struct stat statbuf;
  return stat(path.c_str(), &statbuf) == 0;
}

std::string File::find_index(std::string path, std::vector<std::string> indexes) {
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(path.c_str())) != NULL) {
    /* print all the files and directories within directory */
    while ((ent = readdir (dir)) != NULL) {
      for (std::vector<std::string>::iterator it = indexes.begin(); it != indexes.end(); it++) {
        if (*it == ent->d_name) {
          std::cout << "FOUND INDEX: " << ent->d_name << std::endl;
          return ent->d_name;
        }
      }
    }
    closedir (dir);
  } else {
    /* could not open directory */
    perror ("");
    return "";
  }
  return "";
}