#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <iostream>
# include <vector>
# include <map>

class Location {
 public:
  // Constructors & Deconstructors
  Location();
  ~Location();

  typedef void (Location::*type)(std::vector<std::string>::iterator &);
  static std::map<std::string, type> dir;
  static void initMap();

  void setup(std::vector<std::string>::iterator &it);
  void index(std::vector<std::string>::iterator &it);
  void cgi(std::vector<std::string>::iterator &it);

  void print();

 private:
  std::string uri_;
  std::vector<std::string> limit_except_;
  std::vector<std::string> index_;
  std::map<std::string, std::string> cgi_;
  // bool autoindex_;
  std::string root_;
};

#endif
