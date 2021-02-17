#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <sstream>
# include <string>
# include <map>
# include <vector>

# include <algorithm>
# include <string.h>

inline int ft_stricmp(const std::string &lhs, const std::string &rhs) {
  char *s1 = new char[lhs.length() + 1];
  char *s2 = new char[rhs.length() + 1];

  int ret = -1;
  strcpy(s1, lhs.c_str());
  strcpy(s2, rhs.c_str());

  int i = 0;
  while (s1[i]) {
    s1[i] = std::tolower(s1[i]);
    i++;
  }

  i = 0;
  while (s2[i]) {
    s2[i] = std::tolower(s2[i]);
    i++;
  }

  i = 0;
  while (s1[i] && s2[i] && s1[i] == s2[i])
		i++;

	if (!s1[i] && !s2[i])
		ret = 0;

  delete [] s1;
  delete [] s2;

  return ret;
};

struct comp {
  bool operator() (const std::string& lhs, const std::string& rhs) const {
    return ft_stricmp(lhs.c_str(), rhs.c_str()) < 0;
  };
};

class Request {
 public:
  // Constructors & Deconstructors
  Request();
  Request(std::string &msg);
  ~Request();

  friend class Response;

  void isValid();
  void parse();

  void print();

 private:
  std::string msg_;
  std::string method_;
  std::string target_;
  std::string protocol_;
  std::string req_body_;
  std::map<std::string, std::string, comp> headers_;
};

#endif
