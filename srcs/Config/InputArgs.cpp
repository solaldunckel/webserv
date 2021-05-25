# include "InputArgs.hpp"

InputArgs::InputArgs(int argc, char **argv) : argc_(argc), argv_(argv), path_("./config/default.conf"), log_level_(INFO) {
  options_["h"];
  options_["-help"];
  options_["t"];
  options_["-test"];
  options_["u"];
  options_["-uri"];
}

InputArgs::~InputArgs() {};

void InputArgs::parse() {
  for (int i = 1; i < argc_; i++) {
    std::string arg = argv_[i];

    if (arg.find('-') == 0) {
      std::string opt = arg.substr(arg.find('-') + 1);

      if (options_.find(opt) != options_.end())
        options_[opt] = true;
      else if (opt == "l" || opt == "-log") {
        if (i + 1 < argc_) {
          std::string opt2 = argv_[i + 1];
          if (opt2.find_first_not_of("0123456789") == std::string::npos) {
            i++;
            int level = ft::stoi(argv_[i]);
            if (level <= 2)
              log_level_ = (LogLevel)level;
            else
              throw webserv_exception("invalid option %\n\n" + helpText(), 0, opt + " " + opt2);
          }
        }
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

  text += "Usage: webserv [options] [config_file]\n";
  text += "\nOptions: \n";
  text += "  -h, --help         : this help text\n";
  text += "  -l, --log [LEVEL]  : set log level (between 0 and 2)\n";
  text += "  -t, --test         : test config and exit\n";
  text += "  -u, --uri          : keep location uri on rooting (similar to nginx)";

  return text;
}

std::string &InputArgs::getPath() {
  return path_;
}

bool InputArgs::help() {
  return options_["h"] || options_["-help"];
}

LogLevel InputArgs::log() {
  return log_level_;
}

bool InputArgs::test() {
  return options_["t"] || options_["-test"];
}

bool InputArgs::location() {
  return options_["u"] || options_["-uri"];
}