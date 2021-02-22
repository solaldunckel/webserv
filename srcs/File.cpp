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

std::string &File::getExtension() {
  if (extension_.empty())
    extension_ = name_.substr(name_.find_last_of('.'));
  return extension_;
}

bool File::is_directory() {
  struct stat statbuf;
  stat(name_.c_str(), &statbuf);
  return S_ISDIR(statbuf.st_mode);
}

// void File::open_index(std::vector<std::string> index) {
//   DIR *dir;
//   struct dirent *ent;
//   if ((dir = opendir(name_)) != NULL) {
//     /* print all the files and directories within directory */
//     while ((ent = readdir (dir)) != NULL) {
//       if (ent->d_name == "")
//     }
//     closedir (dir);
//   } else {
//     /* could not open directory */
//     perror ("");
//     return EXIT_FAILURE;
//   }
// }