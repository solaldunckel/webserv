#ifndef INPUTARGS_HPP
# define INPUTARGS_HPP

# include <string>
# include <map>

# include "Utils.hpp"

class InputArgs {
  public:
    InputArgs(int argc, char **argv);
    ~InputArgs();

    void parse();
    std::string helpText();
    std::string &getPath();
    bool verbose();
    bool help();
    bool test();
    bool location();

  private:
    int argc_;
    char **argv_;
    std::string path_;
    std::map<std::string, bool> options_;
};

#endif