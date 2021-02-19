# include "MimeTypes.hpp"

std::map<std::string, std::string> MimeTypes::mimes_;

void MimeTypes::initMap() {
  mimes_[".css"] = "text/css";
  mimes_[".jpg"] = "image/jpeg";
  mimes_[".jpeg"] = "image/jpeg";
  mimes_[".html"] = "text/html";
  mimes_[".htm"] = "text/html";
  mimes_[".png"] = "image/png";
}

std::string MimeTypes::getType(std::string extension) {
  initMap();
  std::string ext(ft::to_lower(extension));
  if (mimes_.count(ext))
    return mimes_[ext];
  return "application/octet-stream";
};