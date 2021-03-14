#include "File.hpp"

File::File(std::string path) : path_(path) {}

File::~File() {
  if (stream_.is_open())
    stream_.close();
}

void File::set_path(std::string path) {
  path_ = path;
}

bool File::open() {
  if (stream_.is_open())
    stream_.close();

  stream_.open(path_, std::ios::binary | std::ios::in);
  return stream_.is_open() && stream_.good();
}

void File::create(std::string &body) {
  stream_.open(path_, std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
  stream_ << body;
}

void File::unlink() {
  ::unlink(path_.c_str());
}

bool File::is_open() {
  if (stream_.is_open() && stream_.good())
    return true;
  return false;
}

std::string set_width(size_t width, std::string str) {
  size_t len = str.length();
  std::string w;

  for (size_t i = 0; i < width - len; i++) {
    w += " ";
  }
  w += str;
  return w;
}

std::string File::autoIndex(std::string &target) {
  std::string body;
  DIR *dir;
  struct dirent *ent;

  dir = opendir(path_.c_str());
  body += "<html>\r\n";
  body += "<head><title>Index of /" + target + "</title></head>\r\n";
  body += "<body>\r\n";
  body += "<h1>Index of /" + target + "</h1><hr><pre>";
  readdir(dir);
  while ((ent = readdir(dir))) {
    struct stat statbuf;
    std::string path(path_ + "/" + ent->d_name);
    stat(path.c_str(), &statbuf);
    std::string name = ent->d_name;
    if (S_ISDIR(statbuf.st_mode))
      name += "/";
    body = body + "<a href=\"" + name + "\">" + name + "</a>";
    body += set_width(70 - name.length(), "DATE");

    if (S_ISDIR(statbuf.st_mode))
      body += set_width(50, "-");
    else
      body += set_width(50, std::to_string(statbuf.st_size));
    body += "\r\n";
  }
  body += "</pre><hr></body>\r\n";
  body += "</html>\r\n";
  closedir(dir);
  return body;
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

std::string File::find_index(std::vector<std::string> &indexes) {
  DIR *dir;
  struct dirent *ent;

  if ((dir = opendir(path_.c_str()))) {
    while ((ent = readdir(dir))) {
      for (std::vector<std::string>::iterator it = indexes.begin(); it != indexes.end(); it++) {
        if (*it == ent->d_name) {
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

std::string File::getContent() {
  char buf[512 + 1];
  std::string final;

  while (!stream_.eof()) {
    stream_.read(buf, 512);
    final += std::string(buf, stream_.gcount());
  }
  return final;
};

std::string File::getExtension() {
  if (path_.length() && path_.find_last_of('.') != std::string::npos)
    return path_.substr(path_.find_last_of('.'));
  return "";
}

std::string &File::getPath() {
  return path_;
}