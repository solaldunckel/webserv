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

  if (len > width)
    width = 0;

  for (size_t i = 0; i < width - len; i++) {
    w += " ";
  }
  w += str;
  return w;
}

bool sort_auto_listing(auto_listing i, auto_listing j) {
  if (i.is_dir_ && j.is_dir_) {
    return (i.name_ < j.name_);
  } else if (!i.is_dir_ && !j.is_dir_)
    return (i.name_ < j.name_);
  return (i.is_dir_ > j.is_dir_);
}

std::string File::autoIndex(std::string &target) {
  std::string body;
  struct tm	*tm;
  char buf[32];
  DIR *dir;
  struct stat statbuf;
  struct dirent *ent;

  dir = opendir(path_.c_str());
  body += "<html>\r\n";
  body += "<head><title>Index of " + target + "</title></head>\r\n";
  body += "<body>\r\n";
  body += "<h1>Index of " + target + "</h1><hr><pre>";
  std::vector<auto_listing> listing;

  readdir(dir);
  while ((ent = readdir(dir))) {
    auto_listing li;

    li.name_ = ent->d_name;
    if (li.name_.length() > 50) {
      li.name_.erase(47);
      li.name_ += "..>";
    }
    std::string path(path_ + "/" + ent->d_name);
    stat(path.c_str(), &statbuf);

    if (S_ISDIR(statbuf.st_mode)) {
      li.is_dir_ = true;
      li.name_ += "/";
    }

    tm = gmtime(&statbuf.st_mtime);
    int ret = strftime(buf, 32, "%d-%b-%Y %H:%M", tm);
    li.date_ = std::string(buf, ret);
    li.size_ = statbuf.st_size;
    listing.push_back(li);
  }

  std::sort(listing.begin(), listing.end(), sort_auto_listing);

  for (std::vector<auto_listing>::iterator it = listing.begin(); it != listing.end(); it++) {
    body = body + "<a href=\"" + it->name_ + "\">" + it->name_ + "</a>";
    if (it != listing.begin()) {
      body += set_width(68 - it->name_.length(), it->date_);
      if (it->is_dir_)
        body += set_width(20, "-");
      else
        body += set_width(20, std::to_string(it->size_));
    }
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