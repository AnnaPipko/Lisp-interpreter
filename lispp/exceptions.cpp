#include "exceptions.h"

SyntaxError::SyntaxError(const std::string &what_arg) :
        std::runtime_error("SyntaxError: " + what_arg) {}

NameError::NameError(const std::string &what_arg) :
        std::runtime_error("NameError: " + what_arg) {}

RuntimeError::RuntimeError(const std::string &what_arg) :
        std::runtime_error("RuntimeError: " + what_arg) {}
