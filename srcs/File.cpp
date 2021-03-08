#include "File.hpp"

/*
** Constructors & Deconstructors
*/

File::File() {}

File::File(std::string path) : path_(path) {
}

File::~File() {
  if (stream_.is_open())
    stream_.close();
}

void File::set_path(std::string path) {
  path_ = path;
}

void File::open() {
  if (stream_.is_open())
    stream_.close();

  stream_.open(path_, std::ios::binary | std::ios::in);
}

void File::create(std::string &body) {
  stream_.open(path_, std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
  stream_ << body;
}

void File::unlink() {
  ::unlink(path_.c_str());
}

std::string File::getContent() {
  char buf[512 + 1];
  std::string final;

  while (!stream_.eof()) {
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
  if (path_.length() && path_.find_last_of('.') != std::string::npos)
    return path_.substr(path_.find_last_of('.'));
  return "";
}

std::string &File::getPath() {
  return path_;
}

bool File::is_directory() {
  struct stat statbuf;
  stat(path_.c_str(), &statbuf);
  return S_ISDIR(statbuf.st_mode);
}

bool File::exists() {
  struct stat statbuf;
  return stat(path_.c_str(), &statbuf) == 0;
}

std::string File::last_modified() {
  struct stat statbuf;
  struct tm	*tm;
  char buf[32];

  stat(path_.c_str(), &statbuf);
  tm = gmtime(&statbuf.st_mtime);
  int ret = strftime(buf, 32, "%a, %d %b %Y %T GMT", tm);
  return std::string(buf, ret);
}

std::string File::find_index(std::vector<std::string> indexes) {
  DIR *dir;
  struct dirent *ent;

  if ((dir = opendir(path_.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      for (std::vector<std::string>::iterator it = indexes.begin(); it != indexes.end(); it++) {
        if (*it == ent->d_name) {
          #ifdef DEBUG
          std::cout << "FOUND INDEX: " << ent->d_name << std::endl;
          #endif
          closedir(dir);
          return ent->d_name;
        }
      }
    }
    closedir(dir);
  } else {
    strerror(errno);
    return "";
  }
  return "";
}