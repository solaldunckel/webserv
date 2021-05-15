# include "InputArgs.hpp"

InputArgs::InputArgs(int argc, char **argv) : argc_(argc), argv_(argv), path_("./config/default.conf") {
  options_["v"];
  options_["h"];
  options_["t"];
  options_["u"];
}

InputArgs::~InputArgs() {};

void InputArgs::parse() {
  for (int i = 1; i < argc_; i++) {
    std::string arg = argv_[i];

    if (arg.find('-') == 0) {
      std::string opt = arg.substr(arg.find('-') + 1);

      if (options_.find(opt) != options_.end())
        options_[opt] = true;
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

  text += "Usage: webserv [options] [config_file]\n";
  text += "\nOptions: \n";
  text += "  -h        : this help text\n";
  text += "  -v        : print response/requests status\n";
  text += "  -t        : test config and exit\n";
  text += "  -u        : keep location uri on rooting (similar to nginx)";

  return text;
}

std::string &InputArgs::getPath() {
  return path_;
}

bool InputArgs::help() {
  return options_["h"];
}

bool InputArgs::verbose() {
  return options_["v"];
}

bool InputArgs::test() {
  return options_["t"];
}

bool InputArgs::location() {
  return options_["u"];
}