# include "InputArgs.hpp"

InputArgs::InputArgs(int argc, char **argv) : argc_(argc), argv_(argv), verbose_(false), help_(false), path_("./config/default.conf") {}

InputArgs::~InputArgs() {};

void InputArgs::parse() {
  for (int i = 1; i < argc_; i++) {
    std::string arg = argv_[i];

    if (arg.find('-') == 0) {
      std::string opt = arg.substr(arg.find('-') + 1);

      if (opt == "v" || opt == "-verbose") {
        verbose_ = true;
      } else if (opt == "h" || opt == "-help") {
        help_ = true;
        std::cout << helpText() << std::endl;
        return;
      }
      else
        throw webserv_exception("invalid option -%\n\n" + helpText(), 0, opt);

    } else {
      path_ = arg;
      if (i != argc_ - 1)
        throw webserv_exception("too many arguments\n\n" + helpText(), 0, "test");
    }
  }
}

std::string InputArgs::helpText() {
  std::string text = "";

  text += "Usage:\n   webserv [options] [config_file]\n";
  text += "\nOptions: \n";
  text += "   -v, --verbose     VERBOSE, print response/requests status\n";
  text += "   -h, --help        HELP, print help text";

  return text;
}

bool InputArgs::help() {
  return help_;
}

std::string &InputArgs::getPath() {
  return path_;
}

bool InputArgs::verbose() {
  return verbose_;
}