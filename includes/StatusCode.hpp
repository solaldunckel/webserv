#ifndef STATUSCODE_HPP
# define STATUSCODE_HPP

# include <map>
# include <string>

class StatusCode {
 public:
  std::string &operator[](int status_code) const;

 private:
  static bool init_;
  static std::map<int, std::string> error_codes_;
  static void initMap();
};

#endif