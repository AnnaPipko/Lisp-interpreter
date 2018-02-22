#pragma once

#include "tokenizer.h"
#include "parser.h"
#include <memory>
#include <iostream>

class Lispp{
public:
    Lispp();
    Lispp(std::istream* in, std::ostream* out);
    ~Lispp();
    void Run();
private:
    std::shared_ptr<Tokenizer> tokenizer_;
    std::shared_ptr<Parser> parser_;
    std::shared_ptr<Scope> global_scope_;
    std::istream* in_;
    std::ostream* out_;
};
