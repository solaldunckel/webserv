#ifndef MIMETYPES_HPP
# define MIMETYPES_HPP

# include <map>
# include <string>

# include "Utils.hpp"

class MimeTypes {
 public:
  static std::string getType(std::string extension);

 private:
  static std::map<std::string, std::string, ft::comp> mimes_;
  static void initMap();
};

#endif