#ifndef INPUTARGS_HPP
# define INPUTARGS_HPP

# include <string>

# include "Server.hpp"

class InputArgs {
  public:
    InputArgs(int argc, char **argv);
    ~InputArgs();

    void parse();
    std::string helpText();
    std::string &getPath();
    bool verbose();
    bool help();

  private:
    int argc_;
    char **argv_;
    bool verbose_;
    bool help_;
    std::string path_;
};

#endif