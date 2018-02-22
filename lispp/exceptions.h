#pragma once

#include <exception>
#include <string>
#include <stdexcept>

class SyntaxError : public std::runtime_error {
public:
    explicit SyntaxError(const std::string& what_arg);
};

class NameError : public std::runtime_error {
public:
    explicit NameError(const std::string& what_arg);
};

class RuntimeError : public std::runtime_error {
public:
    explicit RuntimeError(const std::string& what_arg);
};