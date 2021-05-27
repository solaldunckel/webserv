#include "File.hpp"

File::File() : fd_(0) {}

File::File(std::string path) : fd_(0) {
  set_path(path);
}

File::~File() {
  close();
}

void File::set_path(std::string path, bool negociation) {
  path_ = ft::unique_char(path);

  if (negociation)
    parseExtensionsNegociation();
  else
    parseExtensions();
}

bool File::open(bool create) {
  close();

  if (create)
    fd_ = ::open(path_.c_str(), O_CREAT | O_RDWR | O_TRUNC, 00755);
  else
    fd_ = ::open(path_.c_str(), O_RDONLY);
  return fd_ > 0;
}

void File::close() {
  if (fd_ <= 0)
    return ;

  ::close(fd_);
  fd_ = 0;
}

void File::create(std::string &body) {
  if (!open(true)) {
    Log.print(DEBUG, "create : " + std::string(strerror(errno)) + " of " + path_, RED, true);
    return ;
  }
  if (body.length() && write(fd_, body.c_str(), body.length()) <= 0)
    Log.print(DEBUG, "create : " + std::string(strerror(errno)) + " of " + path_, RED, true);
}

void File::append(std::string &body) {
  close();
  fd_ = ::open(path_.c_str(), O_RDWR | O_APPEND);
  if (fd_ < 0)
    return ;
  if (body.length() && write(fd_, body.c_str(), body.length()) <= 0)
    Log.print(DEBUG, "append : " + std::string(strerror(errno)) + " of " + path_, RED, true);
}

void File::unlink() {
  if (!exists())
    return ;
  if (::unlink(path_.c_str()) == -1)
    Log.print(DEBUG, "unlink : " + std::string(strerror(errno)) + " of " + path_, RED, true);
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
    body = body + "<a href=\"" + ft::unique_char(target + + "/" + it->name_) + "\">" + it->name_ + "</a>";
    if (it != listing.begin()) {
      body += set_width(68 - it->name_.length(), it->date_);
      if (it->is_dir_)
        body += set_width(20, "-");
      else
        body += set_width(20, ft::to_string(it->size_));
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

bool File::exists(std::string &path) {
  struct stat statbuf;
  return stat(path.c_str(), &statbuf) == 0;
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
          std::string ret = "/" + std::string(ent->d_name);
          closedir(dir);
          return ret;
        }
      }
    }
    closedir(dir);
  } else {
    Log.print(DEBUG, "opendir : " + std::string(strerror(errno)) + " of " + path_, RED, true);
    return "";
  }
  return "";
}

std::string File::getContent() {
  std::string final;
  char buf[4096 + 1];
  int ret;

  lseek(fd_, 0, SEEK_SET);
  while ((ret = read(fd_, buf, 4096)) != 0) {
    if (ret == -1) {
      Log.print(DEBUG, "read : " + std::string(strerror(errno)), RED, true);
      return "";
    }
    buf[ret] = '\0';
    final.insert(final.length(), buf, ret);
  }
  return final;
};

std::string &File::getMimeExtension() {
  return mime_ext_;
}

void File::parse_match() {
  DIR *dir;
  struct dirent *ent;

  std::string path = path_.substr(0, path_.find_last_of("/"));
  if (!matches_.empty())
    matches_.clear();
  if ((dir = opendir(path.c_str()))) {
    while ((ent = readdir(dir))) {
      std::string name(ent->d_name);
      if (file_name_full_ != name && !name.find(file_name_)
        && name.find(mime_ext_) != std::string::npos) {
        matches_.push_back(ent->d_name);
      }
    }
    closedir(dir);
  } else {
    Log.print(DEBUG, "opendir : " + std::string(strerror(errno)) + " of " + path_, RED, true);
  }
}

void File::parseExtensions() {
  std::string file = path_.substr(path_.find_last_of("/") + 1);

  if (file.empty())
    return;

  file_name_full_ = file;

  file_name_ = file.substr(0, file.find("."));
  file.erase(0, file.find("."));
  if (file.find_last_of(".") != std::string::npos) {
    int last = file.find_last_of(".");
    mime_ext_ = file.substr(last);
  }
}

void File::parseExtensionsNegociation() {
  std::string file = path_.substr(path_.find_last_of("/") + 1);

  if (file.empty())
    return;

  file_name_full_ = file;

  file_name_ = file.substr(0, file.find("."));
  file.erase(0, file.find("."));
  if (file.find_last_of(".") != std::string::npos) {
    int last = file.find_last_of(".");
    mime_ext_ = file.substr(last);
    while (!g_mimes.getType(mime_ext_).compare("application/octet-stream")) {
      int last2 = last;
      if ((file.find_last_of(".", last - 1) != std::string::npos)) {
        last = file.find_last_of(".", last - 1);
        mime_ext_ = file.substr(last, last2 - last) ;
      }
      else
        break ;
      if (last <= 0)
        break ;
    }
  }
}

int &File::getFd() {
  return fd_;
}

std::vector<std::string> &File::getMatches() {
  return matches_;
}

std::string &File::getPath() {
  return path_;
}
